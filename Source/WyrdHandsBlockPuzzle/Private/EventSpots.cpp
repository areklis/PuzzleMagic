#include "EventSpots.h"
#include "GridManager.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Dev: puzzle.PumpkinYaw turns the carved pumpkin about its up axis, to find which way the model's face looks.
static TAutoConsoleVariable<float> CVarPumpkinYaw(TEXT("puzzle.PumpkinYaw"), 0.f, TEXT("Yaw in degrees of the event pumpkin's carved face."));
// The camera looks down at the floor from the front: the pumpkin leans back this many degrees so its face looks straight into the lens.
static TAutoConsoleVariable<float> CVarPumpkinTilt(TEXT("puzzle.PumpkinTilt"), -50.f, TEXT("Degrees the event pumpkin leans back toward the camera."));

namespace
{
	// Where the spots stand on the screen: two columns by two rows on each side of the play area, clear of the buttons.
	constexpr float ColumnX[2] = { 0.095f, 0.195f };
	constexpr float RowY[2] = { 0.47f, 0.65f };
	constexpr float PopTime = 0.45f;
	// A cauldron is about 230 units across at scale 1; the spots are this much of the space between two columns.
	constexpr float CauldronWidth = 230.f;
	constexpr float FillOfSpacing = 0.7f;

	float EaseOutBack(float T)
	{
		const float C1 = 1.70158f;
		const float C3 = C1 + 1.f;
		const float U = T - 1.f;
		return 1.f + C3 * U * U * U + C1 * U * U;
	}
}

AEventSpots::AEventSpots()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MarbleFinder(TEXT("/Game/Materials/M_Marble.M_Marble"));
	SphereMesh = SphereFinder.Object;
	CylinderMesh = CylinderFinder.Object;
	CubeMesh = CubeFinder.Object;
	StoneMaterial = MarbleFinder.Object;

	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		USceneComponent* Anchor = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("Spot%d"), Index));
		Anchor->SetupAttachment(RootComponent);
		Spots[Index].Anchor = Anchor;
	}
}

void AEventSpots::Refit()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	FVector2D Viewport(0.f, 0.f);
	if (!Controller || !GEngine || !GEngine->GameViewport)
	{
		return;
	}
	GEngine->GameViewport->GetViewportSize(Viewport);
	if (Viewport.X <= 0.f || Viewport.Y <= 0.f)
	{
		return;
	}
	LastViewport = Viewport;

	// Where each spot stands on the screen (fractions of the viewport). Landscape: two columns by two rows on each side of the play
	// area. Portrait: the board fills the width, so each side has one narrow column of four, between the board's far edge and the
	// trays, found by projecting them onto the screen.
	const bool bPortrait = Viewport.Y > Viewport.X * 1.05f;
	FVector2D Wanted[SpotCount];
	if (!bPortrait)
	{
		for (int32 Side = 0; Side < 2; ++Side)
		{
			for (int32 Row = 0; Row < 2; ++Row)
			{
				for (int32 Column = 0; Column < 2; ++Column)
				{
					// The right side mirrors the left one.
					const float X = Side == 0 ? ColumnX[Column] : 1.f - ColumnX[1 - Column];
					Wanted[Side * 4 + Row * 2 + Column] = FVector2D(X, RowY[Row]);
				}
			}
		}
	}
	else
	{
		// A slim screen has no room beside the board: four spots stand in a row above the board's candles and four in a row below the
		// trays, which uses the empty floor there. Their heights come from where the board's far edge and the trays are on the screen.
		float TopRow = 0.22f;
		float BottomRow = 0.74f;
		TActorIterator<AGridManager> It(GetWorld());
		if (It)
		{
			const FBox2D Bounds = It->GetContentBounds();
			FVector2D Far;
			if (Controller->ProjectWorldLocationToScreen(FVector(0.5 * (Bounds.Min.X + Bounds.Max.X), Bounds.Min.Y - 0.2 * (Bounds.Max.X - Bounds.Min.X), 0.f), Far))
			{
				TopRow = static_cast<float>(Far.Y / Viewport.Y) - 0.075f; // above the candles
			}
			FVector2D Tray;
			if (Controller->ProjectWorldLocationToScreen(It->GetTrayAnchorWorldLocation(0), Tray))
			{
				BottomRow = static_cast<float>(Tray.Y / Viewport.Y) + 0.11f; // below the trays and HOLD
			}
		}
		TopRow = FMath::Clamp(TopRow, 0.2f, 0.45f);
		BottomRow = FMath::Clamp(BottomRow, TopRow + 0.2f, 0.84f);
		for (int32 Side = 0; Side < 2; ++Side)
		{
			for (int32 Step = 0; Step < 4; ++Step)
			{
				Wanted[Side * 4 + Step] = FVector2D(0.2f + 0.2f * Step, Side == 0 ? TopRow : BottomRow);
			}
		}
	}

	FVector Hits[SpotCount];
	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		FVector Origin, Direction;
		if (!Controller->DeprojectScreenPositionToWorld(Wanted[Index].X * Viewport.X, Wanted[Index].Y * Viewport.Y, Origin, Direction) || FMath::Abs(Direction.Z) < 0.01f)
		{
			return;
		}
		Hits[Index] = Origin + Direction * (-Origin.Z / Direction.Z);
		Spots[Index].Where = FVector(Hits[Index].X, Hits[Index].Y, 0.f);
	}

	float Spacing = FVector::Dist(Hits[0], Hits[1]); // landscape: between the two columns; portrait: between neighbours down a column
	BaseScale = FMath::Clamp(Spacing * FillOfSpacing / CauldronWidth, 0.1f, 1.3f);
	// Portrait: each row has its own size (the row below the trays is much nearer the camera than the row above the board).
	const float RowScale[2] = {
		FMath::Clamp(FVector::Dist(Hits[0], Hits[1]) * FillOfSpacing / CauldronWidth, 0.1f, 1.3f),
		FMath::Clamp(FVector::Dist(Hits[4], Hits[5]) * FillOfSpacing / CauldronWidth, 0.1f, 1.3f) };
	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		FSpot& Spot = Spots[Index];
		Spot.Fit = bPortrait ? RowScale[Index / 4] : BaseScale;
		if (Spot.Anchor)
		{
			Spot.Anchor->SetWorldLocation(Spot.Where);
			ApplyScale(Spot);
		}
	}
}

void AEventSpots::ApplyScale(FSpot& Spot) const
{
	const float Pop = Spot.PopAge >= 1.f ? 1.f : FMath::Max(EaseOutBack(Spot.PopAge), 0.001f);
	Spot.Anchor->SetWorldScale3D(FVector(Spot.Fit * Pop));
}

void AEventSpots::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (FSpot& Spot : Spots)
	{
		if (Spot.Kind != EEventSpotKind::Empty && Spot.PopAge < 1.f)
		{
			Spot.PopAge = FMath::Min(Spot.PopAge + DeltaTime / PopTime, 1.f);
			ApplyScale(Spot);
		}
	}
	// The framing changes with the board size and the window: look again now and then.
	RefitTimer -= DeltaTime;
	if (RefitTimer <= 0.f)
	{
		RefitTimer = 0.5f;
		Refit();
	}
}

void AEventSpots::Clear()
{
	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		SetKind(Index, EEventSpotKind::Empty);
	}
}

int32 AEventSpots::CountOf(EEventSpotKind Kind) const
{
	int32 Count = 0;
	for (const FSpot& Spot : Spots)
	{
		Count += Spot.Kind == Kind ? 1 : 0;
	}
	return Count;
}

bool AEventSpots::AddCauldron()
{
	TArray<int32> Free;
	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		if (Spots[Index].Kind == EEventSpotKind::Empty)
		{
			Free.Add(Index);
		}
	}
	if (Free.Num() == 0)
	{
		return false;
	}
	SetKind(Free[FMath::RandRange(0, Free.Num() - 1)], EEventSpotKind::Cauldron);
	return true;
}

bool AEventSpots::AddPumpkin()
{
	TArray<int32> Free;
	TArray<int32> Cauldrons;
	for (int32 Index = 0; Index < SpotCount; ++Index)
	{
		if (Spots[Index].Kind == EEventSpotKind::Empty)
		{
			Free.Add(Index);
		}
		else if (Spots[Index].Kind == EEventSpotKind::Cauldron)
		{
			Cauldrons.Add(Index);
		}
	}
	const TArray<int32>& Choices = Free.Num() > 0 ? Free : Cauldrons;
	if (Choices.Num() == 0)
	{
		return false;
	}
	SetKind(Choices[FMath::RandRange(0, Choices.Num() - 1)], EEventSpotKind::Pumpkin);
	return true;
}

void AEventSpots::SetKind(int32 Index, EEventSpotKind Kind)
{
	FSpot& Spot = Spots[Index];
	for (UStaticMeshComponent* Part : Spot.Parts)
	{
		if (Part)
		{
			Part->DestroyComponent();
		}
	}
	Spot.Parts.Reset();
	for (USceneComponent* Frame : Spot.Frames)
	{
		if (Frame)
		{
			Frame->DestroyComponent();
		}
	}
	Spot.Frames.Reset();
	Spot.Kind = Kind;
	if (Kind == EEventSpotKind::Cauldron)
	{
		BuildCauldron(Spot);
	}
	else if (Kind == EEventSpotKind::Pumpkin)
	{
		BuildPumpkin(Spot);
	}
	Spot.PopAge = Kind == EEventSpotKind::Empty ? 1.f : 0.f;
	if (Spot.Anchor)
	{
		ApplyScale(Spot);
	}
}

UMaterialInstanceDynamic* AEventSpots::MakeMaterial(const FLinearColor& Base, const FLinearColor& Vein, float VeinStrength, float Roughness)
{
	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(StoneMaterial, this);
	Material->SetVectorParameterValue(TEXT("BaseColor"), Base);
	Material->SetVectorParameterValue(TEXT("VeinColor"), Vein);
	Material->SetScalarParameterValue(TEXT("VeinStrength"), VeinStrength);
	Material->SetScalarParameterValue(TEXT("Roughness"), Roughness);
	return Material;
}

UStaticMeshComponent* AEventSpots::AddPart(FSpot& Spot, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, UMaterialInterface* Material, USceneComponent* Parent, float RollDegrees)
{
	UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this);
	Component->SetStaticMesh(Mesh);
	Component->SetupAttachment(Parent ? Parent : Spot.Anchor.Get());
	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetCastShadow(true);
	Component->RegisterComponent();
	Component->SetRelativeLocation(Location);
	Component->SetRelativeScale3D(Scale);
	if (RollDegrees != 0.f)
	{
		Component->SetRelativeRotation(FRotator(0.f, RollDegrees, 0.f)); // turned about the part's own up axis
	}
	if (Material)
	{
		Component->SetMaterial(0, Material);
	}
	Spot.Parts.Add(Component);
	return Component;
}

void AEventSpots::BuildFace(FSpot& Spot)
{
	// The carved face is a flat frame turned to look at the camera, sitting on the front of the centre lobe. It floats a little in front
	// of the curved skin, which is fine from the player's view. Each feature is a dark carved edge with a glowing cut in front of it:
	// two diamond eyes, a small diamond nose and a jagged mouth.
	USceneComponent* Face = NewObject<USceneComponent>(this);
	Face->SetupAttachment(Spot.Anchor);
	Face->RegisterComponent();
	Face->SetRelativeLocation(FVector(0.f, 54.f, 112.f));
	Face->SetRelativeRotation(FRotationMatrix::MakeFromZX(FVector(0.f, 0.57f, 0.82f).GetSafeNormal(), FVector(1.f, 0.f, 0.f)).Rotator());
	Spot.Frames.Add(Face);

	UMaterialInstanceDynamic* Dark = MakeMaterial(FLinearColor(0.05f, 0.012f, 0.f), FLinearColor(0.1f, 0.03f, 0.f), 0.1f, 0.8f);
	UMaterialInstanceDynamic* Glow = MakeMaterial(FLinearColor(1.f, 0.72f, 0.1f), FLinearColor(1.f, 0.9f, 0.3f), 0.2f, 0.3f);
	// Face coordinates: X to the right, Y up (the frame's own Y points down the face, so it is flipped here); Depth is toward the viewer.
	auto Feature = [&](float X, float Y, float W, float H, float Roll)
	{
		AddPart(Spot, CubeMesh, FVector(X, -Y, 0.f), FVector(W * 1.35f / 100.f, H * 1.35f / 100.f, 0.05f), Dark, Face, Roll);
		AddPart(Spot, CubeMesh, FVector(X, -Y, 1.5f), FVector(W / 100.f, H / 100.f, 0.05f), Glow, Face, Roll);
	};
	Feature(-30.f, 16.f, 20.f, 20.f, 45.f);   // left eye
	Feature(30.f, 16.f, 20.f, 20.f, 45.f);    // right eye
	Feature(0.f, 0.f, 11.f, 11.f, 45.f);      // nose
	for (int32 Tooth = 0; Tooth < 5; ++Tooth)
	{
		Feature(-40.f + 20.f * Tooth, -22.f + (Tooth % 2 == 0 ? 0.f : -9.f), 17.f, 13.f, 0.f); // jagged mouth
	}
}

void AEventSpots::BuildCauldron(FSpot& Spot)
{
	// The same iron cauldron as the Halloween scenery's (AHalloweenProps), without the steam, bubbles and light: three legs,
	// the round body, a collar round the mouth, two handles and the lime liquid.
	UMaterialInstanceDynamic* Iron = MakeMaterial(FLinearColor(0.012f, 0.012f, 0.016f), FLinearColor(0.08f, 0.08f, 0.1f), 0.25f, 0.4f);
	UMaterialInstanceDynamic* Liquid = MakeMaterial(FLinearColor(0.25f, 0.95f, 0.06f), FLinearColor(0.5f, 1.f, 0.2f), 0.6f, 0.2f);
	for (int32 Leg = 0; Leg < 3; ++Leg)
	{
		const float Angle = FMath::DegreesToRadians(90.f + Leg * 120.f);
		AddPart(Spot, CylinderMesh, FVector(FMath::Cos(Angle) * 62.f, FMath::Sin(Angle) * 62.f, 21.f), FVector(0.2f, 0.2f, 0.42f), Iron);
	}
	AddPart(Spot, SphereMesh, FVector(0.f, 0.f, 100.f), FVector(2.3f, 2.3f, 1.8f), Iron);
	AddPart(Spot, CylinderMesh, FVector(0.f, 0.f, 172.f), FVector(2.2f, 2.2f, 0.22f), Iron);
	AddPart(Spot, SphereMesh, FVector(-115.f, 0.f, 150.f), FVector(0.32f), Iron);
	AddPart(Spot, SphereMesh, FVector(115.f, 0.f, 150.f), FVector(0.32f), Iron);
	AddPart(Spot, CylinderMesh, FVector(0.f, 0.f, 184.f), FVector(1.95f, 1.95f, 0.03f), Liquid);
}

void AEventSpots::BuildPumpkin(FSpot& Spot)
{
	// The carved pumpkin of Kenney's Graveyard Kit (CC0, see RawMeshes/Kenney and Tools/import_kenney_pumpkin.py), turned so its
	// face looks at the player. Without that asset a pumpkin is built from basic shapes.
	if (UStaticMesh* Carved = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Event/SM_PumpkinCarved.SM_PumpkinCarved")))
	{
		const FBox Box = Carved->GetBoundingBox();
		const FVector Size = Box.GetSize();
		const float Width = FMath::Max(static_cast<float>(FMath::Max(Size.X, Size.Y)), 1.f);
		const float Fit = 190.f / Width; // about as wide as a cauldron is across
		// puzzle.PumpkinYaw below 0 is a test: each spot turns its pumpkin a quarter more than the one before.
		const float Yaw = CVarPumpkinYaw.GetValueOnGameThread();
		const FRotator Turn(0.f, Yaw < 0.f ? 90.f * static_cast<float>(&Spot - Spots) : Yaw, CVarPumpkinTilt.GetValueOnGameThread());
		// The pumpkin turns about its own middle, which stays at half its height over the floor.
		const FVector Middle(0.f, 0.f, (Box.Min.Z + Box.Max.Z) * 0.5f * Fit);
		const FVector Wanted(0.f, 0.f, Size.Z * Fit * 0.5f + 4.f);
		UStaticMeshComponent* Part = AddPart(Spot, Carved, Wanted - Turn.RotateVector(Middle), FVector(Fit), nullptr);
		Part->SetRelativeRotation(Turn);
		return;
	}
	// A fat pumpkin: a centre lobe and four round it, a green stem on top.
	UMaterialInstanceDynamic* Orange = MakeMaterial(FLinearColor(0.95f, 0.3f, 0.02f), FLinearColor(1.f, 0.5f, 0.06f), 0.2f, 0.5f);
	UMaterialInstanceDynamic* DeepOrange = MakeMaterial(FLinearColor(0.75f, 0.2f, 0.01f), FLinearColor(0.95f, 0.38f, 0.04f), 0.2f, 0.5f);
	UMaterialInstanceDynamic* Green = MakeMaterial(FLinearColor(0.07f, 0.2f, 0.03f), FLinearColor(0.12f, 0.3f, 0.05f), 0.1f, 0.6f);
	AddPart(Spot, SphereMesh, FVector(0.f, 0.f, 66.f), FVector(1.7f, 1.7f, 1.3f), Orange);
	AddPart(Spot, SphereMesh, FVector(46.f, 0.f, 62.f), FVector(1.25f, 1.45f, 1.2f), DeepOrange);
	AddPart(Spot, SphereMesh, FVector(-46.f, 0.f, 62.f), FVector(1.25f, 1.45f, 1.2f), DeepOrange);
	AddPart(Spot, SphereMesh, FVector(0.f, 40.f, 62.f), FVector(1.45f, 1.2f, 1.2f), DeepOrange);
	AddPart(Spot, SphereMesh, FVector(0.f, -40.f, 62.f), FVector(1.45f, 1.2f, 1.2f), DeepOrange);
	AddPart(Spot, CylinderMesh, FVector(0.f, 0.f, 132.f), FVector(0.28f, 0.28f, 0.4f), Green);
	BuildFace(Spot);
}
