// The arcade side of APuzzleGameMode: a campaign of sequential challenges (a UArcadeCampaign data asset). A challenge
// sets the board size, the relics and the bonus tiles, shows an intro popup, runs the clock, and ends with an outro
// popup when the goal score is reached, or a fail popup when time runs out or no piece fits.
#include "PuzzleGameMode.h"
#include "ArcadeCampaign.h"
#include "GridManager.h"
#include "PuzzleManager.h"
#include "PuzzleHUDWidget.h"
#include "PuzzleInputHandler.h"
#include "PuzzleSaveGame.h"

void APuzzleGameMode::LoadCampaign()
{
	if (!Campaign)
	{
		Campaign = Cast<UArcadeCampaign>(StaticLoadObject(UArcadeCampaign::StaticClass(), nullptr, TEXT("/Game/Arcade/DA_ArcadeCampaign.DA_ArcadeCampaign"), nullptr, LOAD_NoWarn | LOAD_Quiet));
	}
}

bool APuzzleGameMode::HasArcade() const
{
	return Campaign && Campaign->Challenges.Num() > 0;
}

int32 APuzzleGameMode::GetArcadeCount() const
{
	return Campaign ? Campaign->Challenges.Num() : 0;
}

const FArcadeChallenge* APuzzleGameMode::GetArcadeChallenge() const
{
	return (Campaign && Campaign->Challenges.IsValidIndex(ArcadeIndex)) ? &Campaign->Challenges[ArcadeIndex] : nullptr;
}

int32 APuzzleGameMode::GetArcadeProgress() const
{
	return SaveGame ? FMath::Clamp(SaveGame->ArcadeProgress, 0, GetArcadeCount()) : 0;
}

void APuzzleGameMode::OpenArcade()
{
	if (!HasArcade())
	{
		return;
	}
	// With nothing beaten yet it goes straight to the first challenge; otherwise it offers to continue.
	if (GetArcadeProgress() == 0)
	{
		StartArcade(0);
	}
	else if (UPuzzleHUDWidget* UI = GetUI())
	{
		UI->ShowCard(EPuzzleCard::ArcadeMenu);
	}
}

void APuzzleGameMode::StartArcade(int32 FromIndex)
{
	if (!HasArcade() || !PuzzleManager || !GridManager)
	{
		return;
	}
	SetAutoPlay(false);
	bArcade = true;
	ArcadeIndex = FMath::Clamp(FromIndex, 0, GetArcadeCount() - 1);
	PrepareArcadeChallenge(true);
}

void APuzzleGameMode::PrepareArcadeChallenge(bool bShowIntro)
{
	const FArcadeChallenge* Challenge = GetArcadeChallenge();
	if (!Challenge || !PuzzleManager || !GridManager)
	{
		return;
	}
	PuzzleManager->ConfigureItems(Challenge->bHolyLight, Challenge->bReroll, Challenge->bPumpkin, Challenge->bOutgoingBottle, Challenge->bIncomingBottle);
	GridManager->SetGridSize(Challenge->GridWidth, Challenge->GridHeight);
	ReframeCamera();
	StartRound();

	ArcadeTimeLeft = static_cast<float>(Challenge->TimeLimitSeconds);
	ArcadeFailReason.Reset();
	if (bShowIntro)
	{
		// The empty board waits behind the popup; the clock starts with the START button.
		ArcadeState = EArcadeState::Intro;
		Flow = EPuzzleFlow::Menu;
		if (UPuzzleHUDWidget* UI = GetUI())
		{
			UI->ShowCard(EPuzzleCard::ArcadeIntro);
		}
	}
	else
	{
		BeginArcadeChallenge();
	}
}

void APuzzleGameMode::BeginArcadeChallenge()
{
	const FArcadeChallenge* Challenge = GetArcadeChallenge();
	if (!bArcade || !Challenge)
	{
		return;
	}
	ArcadeState = EArcadeState::Playing;
	ArcadeTimeLeft = static_cast<float>(Challenge->TimeLimitSeconds);
	bGameOver = false;
	Flow = EPuzzleFlow::Playing;
	if (UPuzzleHUDWidget* UI = GetUI())
	{
		UI->ShowCard(EPuzzleCard::None);
	}
}

void APuzzleGameMode::ArcadeRetry()
{
	if (bArcade)
	{
		PrepareArcadeChallenge(false);
	}
}

void APuzzleGameMode::ArcadeAdvance()
{
	if (!bArcade)
	{
		return;
	}
	++ArcadeIndex;
	if (ArcadeIndex >= GetArcadeCount())
	{
		ArcadeIndex = GetArcadeCount() - 1;
		ArcadeState = EArcadeState::Done;
		Flow = EPuzzleFlow::Menu;
		if (UPuzzleHUDWidget* UI = GetUI())
		{
			UI->ShowCard(EPuzzleCard::ArcadeDone);
		}
		return;
	}
	PrepareArcadeChallenge(true);
}

void APuzzleGameMode::LeaveArcade()
{
	if (!bArcade)
	{
		return;
	}
	bArcade = false;
	ArcadeState = EArcadeState::None;
	// Back to the options and board size the player picked in the menus.
	if (SaveGame && PuzzleManager)
	{
		PuzzleManager->SetExtrasEnabled(SaveGame->bOptionRelics);
		PuzzleManager->SetBonusTilesEnabled(SaveGame->bOptionBonusTiles);
		if (GridManager)
		{
			GridManager->SetGridSize(SaveGame->CourseWidth, SaveGame->CourseHeight);
			ReframeCamera();
		}
		StartRound();
	}
}

void APuzzleGameMode::CheckArcadeWin()
{
	const FArcadeChallenge* Challenge = GetArcadeChallenge();
	if (!bArcade || ArcadeState != EArcadeState::Playing || !Challenge || !PuzzleManager || PuzzleManager->Score < Challenge->TargetScore)
	{
		return;
	}
	ArcadeState = EArcadeState::Won;
	bGameOver = true; // no more moves while the last clear plays out
	if (InputHandler)
	{
		InputHandler->CancelInteraction();
	}
	if (SaveGame && SaveGame->ArcadeProgress < ArcadeIndex + 1)
	{
		SaveGame->ArcadeProgress = ArcadeIndex + 1;
		SaveGame->Save();
	}
	Later(AGridManager::ArriveDuration + 1.6f, [this]()
	{
		if (!bArcade || ArcadeState != EArcadeState::Won)
		{
			return;
		}
		Flow = EPuzzleFlow::Menu;
		if (UPuzzleHUDWidget* UI = GetUI())
		{
			UI->ShowCard(EPuzzleCard::ArcadeOutro);
		}
	});
}

void APuzzleGameMode::FailArcade(const FString& Reason)
{
	if (!bArcade || ArcadeState != EArcadeState::Playing)
	{
		return;
	}
	ArcadeState = EArcadeState::Failed;
	ArcadeFailReason = Reason;
	bGameOver = true;
	if (InputHandler)
	{
		InputHandler->CancelInteraction();
	}
	Later(1.0f, [this]()
	{
		if (!bArcade || ArcadeState != EArcadeState::Failed)
		{
			return;
		}
		Flow = EPuzzleFlow::Menu;
		if (UPuzzleHUDWidget* UI = GetUI())
		{
			UI->ShowCard(EPuzzleCard::ArcadeFail);
		}
	});
}

void APuzzleGameMode::PuzzleArcadeTest(const FString& What)
{
	const FArcadeChallenge* Challenge = GetArcadeChallenge();
	if (!bArcade || !Challenge || !PuzzleManager)
	{
		return;
	}
	if (What == TEXT("win"))
	{
		PuzzleManager->Score = Challenge->TargetScore;
		CheckArcadeWin();
	}
	else if (What == TEXT("fail"))
	{
		FailArcade(TEXT("Test failure"));
	}
	else if (What == TEXT("time"))
	{
		ArcadeTimeLeft = 5.f;
	}
}

void APuzzleGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const FArcadeChallenge* Challenge = GetArcadeChallenge();
	if (bArcade && ArcadeState == EArcadeState::Playing && Challenge && Challenge->TimeLimitSeconds > 0 && Flow == EPuzzleFlow::Playing && !bPauseMenuOpen && !bGameOver)
	{
		// A long hitch (the app in the background) must not eat the player's time.
		ArcadeTimeLeft -= FMath::Min(DeltaSeconds, 0.25f);
		if (ArcadeTimeLeft <= 0.f)
		{
			ArcadeTimeLeft = 0.f;
			FailArcade(TEXT("Time's up!"));
		}
	}
}
