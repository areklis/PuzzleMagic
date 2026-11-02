#include "PuzzleInputHandler.h"
#include "GridManager.h"
#include "PuzzleManager.h"
#include "PuzzleGameMode.h"
#include "PuzzleSDLGamepadSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"

APuzzleInputHandler::APuzzleInputHandler()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APuzzleInputHandler::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		GamepadSubsystem = GameInstance->GetSubsystem<UPuzzleSDLGamepadSubsystem>();
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		// Default game input captures and hides the mouse, which makes drag-to-place impossible.
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		// The game draws its own touch UI: remove the engine's default on-screen joysticks.
		PC->ActivateTouchInterface(nullptr);

		EnableInput(PC);
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APuzzleInputHandler::OnMousePressed);
			InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &APuzzleInputHandler::OnMouseReleased);
			InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &APuzzleInputHandler::OnCancelPressed);
			InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &APuzzleInputHandler::OnEscapePressed);
			InputComponent->BindTouch(IE_Pressed, this, &APuzzleInputHandler::OnTouchPressed);
			InputComponent->BindTouch(IE_Repeat, this, &APuzzleInputHandler::OnTouchRepeat);
			InputComponent->BindTouch(IE_Released, this, &APuzzleInputHandler::OnTouchReleased);
			InputComponent->BindKey(EKeys::R, IE_Pressed, this, &APuzzleInputHandler::OnRestartPressed);
		}
	}
}

void APuzzleInputHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInputAllowed())
	{
		if (bDragging || bHolyTargeting)
		{
			CancelInteraction();
		}
		return;
	}

	// Touch drags are driven by the touch-repeat event; only mouse drags (and mouse aiming of Holy Light) need polling.
	if (!bTouchDrag && (bDragging || bHolyTargeting))
	{
		FVector2D MousePosition;
		if (GetMouseScreenPosition(MousePosition))
		{
			if (bDragging)
			{
				HandleMoved(MousePosition);
			}
			else
			{
				ShowHolyGhost(MousePosition);
			}
		}
	}

	TickGamepadNavigation();
}

bool APuzzleInputHandler::IsInputAllowed() const
{
	const APuzzleGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APuzzleGameMode>() : nullptr;
	return GameMode && GameMode->IsBoardInputEnabled() && GridManager && PuzzleManager && !PuzzleManager->IsFinished();
}

bool APuzzleInputHandler::GetMouseScreenPosition(FVector2D& OutPosition) const
{
#if !UE_BUILD_SHIPPING
	if (bDevMouse)
	{
		OutPosition = DevMousePosition;
		return true;
	}
#endif
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	float MouseX = 0.f, MouseY = 0.f;
	if (PC && PC->GetMousePosition(MouseX, MouseY))
	{
		OutPosition = FVector2D(MouseX, MouseY);
		return true;
	}
	return false;
}

void APuzzleInputHandler::CancelInteraction()
{
	bMouseHolding = false;
	bDragging = false;
	DraggedSlot = -1;
	bOverReserve = false;
	bHolyTargeting = false;
	if (GridManager)
	{
		GridManager->HideGhostPreview();
		GridManager->HideAreaTarget();
		GridManager->HideHeldHalo();
	}
}

void APuzzleInputHandler::ToggleHolyTargeting()
{
	const bool bWasTargeting = bHolyTargeting;
	CancelInteraction();
	bHolyTargeting = !bWasTargeting && IsInputAllowed();
}

void APuzzleInputHandler::OnCancelPressed()
{
	CancelInteraction();
}

// --- Mouse / touch plumbing ---------------------------------------------

void APuzzleInputHandler::OnMousePressed()
{
#if !UE_BUILD_SHIPPING
	if (bDevMouse && !bDevInjecting)
	{
		return; // a click test is running: only its own clicks count
	}
#endif
	FVector2D MousePosition;
	if (!GetMouseScreenPosition(MousePosition))
	{
		return;
	}
	bTouchDrag = false;
	// A piece is in the hand: this click puts it down.
	if (bDragging && bMouseHolding)
	{
		DropHeldPiece(MousePosition);
		return;
	}
	HandlePressed(MousePosition);
	if (bDragging)
	{
		if (bHolyTargeting)
		{
			HandleReleased(MousePosition); // Holy Light is already aimed at the cursor: this click fires it
		}
		else
		{
			bMouseHolding = true; // a piece was picked up: it stays in the hand until the next click
			GridManager->ShowHeldHalo(DraggedSlot);
		}
	}
}

void APuzzleInputHandler::DropHeldPiece(FVector2D ScreenPosition)
{
	if (!IsInputAllowed() || !GridManager || !PuzzleManager || !GridManager->Tray.IsValidIndex(DraggedSlot))
	{
		CancelInteraction();
		return;
	}
	const int32 Slot = DraggedSlot;
	HandleMoved(ScreenPosition); // brings bOverReserve up to date for this click
	const int32 ClickedSlot = FindTraySlotUnderScreenPosition(ScreenPosition);
	if (ClickedSlot == Slot)
	{
		CancelInteraction(); // a click on its own slot puts it back
		return;
	}
	if (ClickedSlot >= 0 && GridManager->TraySlotUsed.IsValidIndex(ClickedSlot) && !GridManager->TraySlotUsed[ClickedSlot])
	{
		// Another piece (one of the three or the one on hold): the hand switches to it. Nothing moves between the slots.
		DraggedSlot = ClickedSlot;
		GridManager->HideGhostPreview();
		GridManager->ShowHeldHalo(ClickedSlot);
		HandleMoved(ScreenPosition);
		return;
	}
	if (bOverReserve)
	{
		CancelInteraction();
		PuzzleManager->ParkPiece(Slot);
		return;
	}
	const FPuzzlePieceShape Shape = GridManager->Tray[Slot];
	int32 OriginX = 0, OriginY = 0;
	if (ScreenPositionToBoardOrigin(ScreenPosition, Shape, OriginX, OriginY) && GridManager->CanPlacePieceAt(Shape, OriginX, OriginY))
	{
		CancelInteraction();
		PuzzleManager->TryPlacePiece(Slot, OriginX, OriginY);
	}
	// otherwise the piece stays in the hand
}

#if !UE_BUILD_SHIPPING
void APuzzleInputHandler::DevMouseTest(int32 Slot, int32 CellX, int32 CellY)
{
	// Slot >= 10: two digits, pick up the first slot, then switch to the second one by clicking it before the drop
	const int32 SwitchTo = Slot >= 10 ? Slot % 10 : -1;
	if (Slot >= 10) { Slot /= 10; }
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !GridManager)
	{
		return;
	}
	auto After = [this](float Delay, TFunction<void()> Step)
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, MoveTemp(Step)), Delay, false);
	};
	FVector2D TrayScreen, CellScreen;
	if (!PC->ProjectWorldLocationToScreen(GridManager->GetTrayAnchorWorldLocation(Slot), TrayScreen)
		|| !PC->ProjectWorldLocationToScreen(GridManager->GetWorldLocationForCell(CellX, CellY), CellScreen))
	{
		return;
	}
	bDevMouse = true;
	DevMousePosition = TrayScreen;
	UE_LOG(LogTemp, Display, TEXT("ClickTest: start, tray slot %d, board cell (%d,%d), score %d"), Slot, CellX, CellY, PuzzleManager ? PuzzleManager->Score : -1);
	After(0.5f, [this, Slot]()
	{
		bDevInjecting = true; OnMousePressed(); bDevInjecting = false;                         // 1st click: pick up
		UE_LOG(LogTemp, Display, TEXT("ClickTest: 1st click -> holding=%d, piece in tray slot %d still there=%d"), IsMouseHolding(), Slot, GridManager && GridManager->TraySlotUsed.IsValidIndex(Slot) ? !GridManager->TraySlotUsed[Slot] : -1);
	});
	After(1.0f, [this]() { bDevInjecting = true; OnMouseReleased(); bDevInjecting = false; UE_LOG(LogTemp, Display, TEXT("ClickTest: button released -> holding=%d (must still be 1)"), IsMouseHolding()); });
	After(1.5f, [this, CellScreen, CellX, CellY]() { DevMousePosition = CellScreen; UE_LOG(LogTemp, Display, TEXT("ClickTest: cursor over board cell (%d,%d)"), CellX, CellY); });
	if (SwitchTo >= 0)
	{
		FVector2D OtherScreen;
		if (PC->ProjectWorldLocationToScreen(GridManager->GetTrayAnchorWorldLocation(SwitchTo), OtherScreen))
		{
			After(12.0f, [this, OtherScreen, SwitchTo]()
			{
				DevMousePosition = OtherScreen;
				bDevInjecting = true; OnMousePressed(); bDevInjecting = false;
				UE_LOG(LogTemp, Display, TEXT("ClickTest: clicked tray slot %d -> holding=%d, held slot=%d"), SwitchTo, IsMouseHolding(), DraggedSlot);
			});
			After(13.0f, [this, CellScreen]() { DevMousePosition = CellScreen; });
		}
	}
	After(24.0f, [this, Slot]()
	{
		const int32 ScoreBefore = PuzzleManager ? PuzzleManager->Score : -1;
		bDevInjecting = true; OnMousePressed(); bDevInjecting = false;                         // 2nd click: put it down
		UE_LOG(LogTemp, Display, TEXT("ClickTest: 2nd click -> holding=%d, piece left the tray=%d, score %d -> %d"), IsMouseHolding(), GridManager && GridManager->TraySlotUsed.IsValidIndex(Slot) ? GridManager->TraySlotUsed[Slot] : -1, ScoreBefore, PuzzleManager ? PuzzleManager->Score : -1);
	});
	After(25.0f, [this]() { bDevInjecting = true; OnMouseReleased(); bDevInjecting = false; bDevMouse = false; UE_LOG(LogTemp, Display, TEXT("ClickTest: finished")); });
}
#endif

void APuzzleInputHandler::OnMouseReleased()
{
#if !UE_BUILD_SHIPPING
	if (bDevMouse && !bDevInjecting)
	{
		return;
	}
#endif
	// PC mouse: a piece is picked up and put down by clicks, so letting go of the button does nothing.
	if (bMouseHolding)
	{
		return;
	}
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		float MouseX = 0.f, MouseY = 0.f;
		PC->GetMousePosition(MouseX, MouseY);
		HandleReleased(FVector2D(MouseX, MouseY));
	}
}

void APuzzleInputHandler::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	bTouchDrag = true;
	HandlePressed(FVector2D(Location.X, Location.Y));
}

void APuzzleInputHandler::OnTouchRepeat(ETouchIndex::Type FingerIndex, FVector Location)
{
	HandleMoved(FVector2D(Location.X, Location.Y));
}

void APuzzleInputHandler::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	HandleReleased(FVector2D(Location.X, Location.Y));
}

// --- Drag-to-place --------------------------------------------------------

bool APuzzleInputHandler::DeprojectToPlane(FVector2D ScreenPosition, float PlaneZ, FVector& OutPoint) const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	FVector RayOrigin, RayDirection;
	if (!PC || !PC->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, RayOrigin, RayDirection) || FMath::IsNearlyZero(RayDirection.Z))
	{
		return false;
	}

	const float T = (PlaneZ - RayOrigin.Z) / RayDirection.Z;
	if (T < 0.f)
	{
		return false;
	}
	OutPoint = RayOrigin + RayDirection * T;
	return true;
}

int32 APuzzleInputHandler::FindTraySlotUnderScreenPosition(FVector2D ScreenPosition) const
{
	FVector Point;
	if (!GridManager || !DeprojectToPlane(ScreenPosition, GridManager->GetTrayPickPlaneZ(), Point))
	{
		return -1;
	}
	return GridManager->FindTraySlotAt(Point);
}

bool APuzzleInputHandler::ScreenPositionToCell(FVector2D ScreenPosition, int32& OutX, int32& OutY) const
{
	FVector HitPoint;
	if (!GridManager || !DeprojectToPlane(ScreenPosition, GridManager->GetPickPlaneZ(), HitPoint))
	{
		return false;
	}

	// On touch the finger would cover the piece, so it rides above the fingertip (world -Y is screen-up).
	if (bTouchDrag)
	{
		HitPoint.Y -= TouchLiftCells * GridManager->TileSpacing;
	}
	return GridManager->WorldLocationToCell(HitPoint, OutX, OutY);
}

bool APuzzleInputHandler::ScreenPositionToBoardOrigin(FVector2D ScreenPosition, const FPuzzlePieceShape& Shape, int32& OutOriginX, int32& OutOriginY) const
{
	int32 CenterX = 0, CenterY = 0;
	if (!GridManager)
	{
		return false;
	}
	// Off-board positions still produce an origin: the ghost shows red rather than vanishing.
	ScreenPositionToCell(ScreenPosition, CenterX, CenterY);

	OutOriginX = CenterX - Shape.GetWidth() / 2;
	OutOriginY = CenterY - Shape.GetHeight() / 2;
	return true;
}

void APuzzleInputHandler::ShowHolyGhost(FVector2D ScreenPosition)
{
	int32 X = 0, Y = 0;
	if (ScreenPositionToCell(ScreenPosition, X, Y))
	{
		GridManager->ShowAreaTarget(X, Y);
	}
	else
	{
		GridManager->HideAreaTarget();
	}
}

void APuzzleInputHandler::HandlePressed(FVector2D ScreenPosition)
{
	// The Halloween event's end screen: a click or touch anywhere goes back to the menu.
	if (APuzzleGameMode* EventMode = GetWorld() ? GetWorld()->GetAuthGameMode<APuzzleGameMode>() : nullptr)
	{
		if (EventMode->IsEventEndWaiting())
		{
			EventMode->EventEndTapped();
			return;
		}
	}
	if (bDragging || !IsInputAllowed())
	{
		return;
	}

	if (bHolyTargeting)
	{
		bDragging = true;
		DraggedSlot = -1;
		ShowHolyGhost(ScreenPosition);
		return;
	}

	const int32 Slot = FindTraySlotUnderScreenPosition(ScreenPosition);
	if (Slot < 0)
	{
		return;
	}

	bDragging = true;
	DraggedSlot = Slot;
	HandleMoved(ScreenPosition);
}

void APuzzleInputHandler::HandleMoved(FVector2D ScreenPosition)
{
	if (!bDragging || !GridManager)
	{
		return;
	}

	if (bHolyTargeting)
	{
		ShowHolyGhost(ScreenPosition);
		return;
	}

	// Dropping a regular piece on the hold slot parks it there.
	FVector TrayPoint;
	bOverReserve = DraggedSlot != AGridManager::ReserveSlot
		&& DeprojectToPlane(ScreenPosition, GridManager->GetTrayPickPlaneZ(), TrayPoint)
		&& GridManager->IsOverReserve(TrayPoint);
	if (bOverReserve)
	{
		GridManager->HideGhostPreview();
		return;
	}

	const FPuzzlePieceShape& Shape = GridManager->Tray[DraggedSlot];
	int32 OriginX = 0, OriginY = 0;
	if (ScreenPositionToBoardOrigin(ScreenPosition, Shape, OriginX, OriginY))
	{
		GridManager->ShowGhostPreview(Shape, OriginX, OriginY, GridManager->CanPlacePieceAt(Shape, OriginX, OriginY));
	}
}

void APuzzleInputHandler::HandleReleased(FVector2D ScreenPosition)
{
	if (!bDragging || !IsInputAllowed())
	{
		bDragging = false;
		DraggedSlot = -1;
		return;
	}

	if (bHolyTargeting)
	{
		int32 X = 0, Y = 0;
		const bool bOnBoard = ScreenPositionToCell(ScreenPosition, X, Y);
		CancelInteraction();
		if (bOnBoard)
		{
			PuzzleManager->UseHolyLight(X, Y);
		}
		return;
	}

	const int32 Slot = DraggedSlot;
	const bool bPark = bOverReserve;
	const FPuzzlePieceShape Shape = GridManager->Tray[Slot];
	CancelInteraction();

	if (bPark)
	{
		PuzzleManager->ParkPiece(Slot);
		return;
	}

	int32 OriginX = 0, OriginY = 0;
	if (ScreenPositionToBoardOrigin(ScreenPosition, Shape, OriginX, OriginY))
	{
		PuzzleManager->TryPlacePiece(Slot, OriginX, OriginY);
	}
}

// --- Gamepad (SDL3) alternative scheme -------------------------------------

void APuzzleInputHandler::RefreshGamepadGhost()
{
	if (!GridManager || GridManager->Tray.Num() == 0)
	{
		return;
	}

	for (int32 Attempt = 0; Attempt < GridManager->Tray.Num() && GridManager->TraySlotUsed[ActiveTraySlot]; ++Attempt)
	{
		ActiveTraySlot = (ActiveTraySlot + 1) % GridManager->Tray.Num();
	}

	const FPuzzlePieceShape& Shape = GridManager->Tray[ActiveTraySlot];
	GridManager->ShowGhostPreview(Shape, CursorX, CursorY, GridManager->CanPlacePieceAt(Shape, CursorX, CursorY));
}

void APuzzleInputHandler::TickGamepadNavigation()
{
	if (!GamepadSubsystem || !GamepadSubsystem->IsGamepadConnected() || !GridManager || !PuzzleManager || bDragging)
	{
		return;
	}

	bool bCursorMoved = false;

	if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::DPadRight))
	{
		CursorX = FMath::Clamp(CursorX + 1, 0, GridManager->GridWidth - 1);
		bCursorMoved = true;
	}
	else if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::DPadLeft))
	{
		CursorX = FMath::Clamp(CursorX - 1, 0, GridManager->GridWidth - 1);
		bCursorMoved = true;
	}

	if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::DPadUp))
	{
		CursorY = FMath::Clamp(CursorY + 1, 0, GridManager->GridHeight - 1);
		bCursorMoved = true;
	}
	else if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::DPadDown))
	{
		CursorY = FMath::Clamp(CursorY - 1, 0, GridManager->GridHeight - 1);
		bCursorMoved = true;
	}

	// Cycles through the three pieces and the hold slot.
	bool bSlotChanged = false;
	if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::CyclePiece))
	{
		for (int32 Attempt = 0; Attempt < GridManager->Tray.Num(); ++Attempt)
		{
			ActiveTraySlot = (ActiveTraySlot + 1) % GridManager->Tray.Num();
			if (!GridManager->TraySlotUsed[ActiveTraySlot])
			{
				break;
			}
		}
		bSlotChanged = true;
	}

	if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::Confirm))
	{
		if (bHolyTargeting)
		{
			CancelInteraction();
			PuzzleManager->UseHolyLight(CursorX, CursorY);
		}
		else
		{
			PuzzleManager->TryPlacePiece(ActiveTraySlot, CursorX, CursorY);
		}
		bCursorMoved = true; // force ghost refresh against the new board/tray state
	}

	if (GamepadSubsystem->WasButtonJustPressed(EPuzzleGamepadButton::Cancel))
	{
		CancelInteraction();
		return;
	}

	if (bHolyTargeting)
	{
		if (bCursorMoved)
		{
			GridManager->ShowAreaTarget(CursorX, CursorY);
		}
	}
	else if (bCursorMoved || bSlotChanged)
	{
		RefreshGamepadGhost();
	}
}

void APuzzleInputHandler::OnEscapePressed()
{
	// Escape first lets go of a piece that is being dragged; with nothing in the hand it opens or closes the menu.
	if (bDragging || bHolyTargeting)
	{
		CancelInteraction();
		return;
	}
	if (APuzzleGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APuzzleGameMode>() : nullptr)
	{
		GameMode->TogglePauseMenu();
	}
}

void APuzzleInputHandler::OnToggleAutoPlayPressed()
{
	if (APuzzleGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APuzzleGameMode>() : nullptr)
	{
		GameMode->TogglePauseMenu();
	}
}

void APuzzleInputHandler::OnRestartPressed()
{
	CancelInteraction();
	APuzzleGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APuzzleGameMode>() : nullptr;
	if (GameMode && GameMode->Flow != EPuzzleFlow::Menu)
	{
		GameMode->Retry();
	}
}
