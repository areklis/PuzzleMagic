#include "HalloweenEvent.h"
#include "PuzzleManager.h"
#include "PuzzleRiddles.h"
#include "PuzzleSaveGame.h"
#include "Misc/DateTime.h"

int32 UHalloweenEvent::TodayNumber()
{
	return static_cast<int32>(FDateTime::Now().GetDate().GetTicks() / ETimespan::TicksPerDay);
}

bool UHalloweenEvent::IsActiveToday()
{
	const int64 First = FDateTime(2026, 10, 1).GetTicks() / ETimespan::TicksPerDay;
	const int64 Last = FDateTime(2026, 11, 30).GetTicks() / ETimespan::TicksPerDay;
	const int64 Today = TodayNumber();
	return Today >= First && Today <= Last;
}

EPuzzleTheme UHalloweenEvent::ThemeFor(const UPuzzleSaveGame* Save)
{
	const uint8 Mode = Save ? Save->ThemeMode : 0;
	if (Mode == 2 && Save && Save->bHalloweenUnlocked)
	{
		return EPuzzleTheme::Halloween;
	}
	if (Mode == 0 && IsActiveToday())
	{
		return EPuzzleTheme::Halloween;
	}
	return EPuzzleTheme::Desk;
}

void UHalloweenEvent::Init(UPuzzleManager* InRules)
{
	Rules = InRules;
}

void UHalloweenEvent::StartRun()
{
	NextRiddleScore = RiddleEvery;
	Treats = 0;
	Tricks = 0;
	Riddle = -1;
	Chosen = -1;
}

bool UHalloweenEvent::CheckRiddleDue()
{
	if (!Rules || Riddle >= 0 || Rules->Score < NextRiddleScore)
	{
		return false;
	}
	NextRiddleScore = (Rules->Score / RiddleEvery + 1) * RiddleEvery;
	return true;
}

int32 UHalloweenEvent::StartRiddle(TArray<int32>& Seen)
{
	const TArray<int32>& Pool = HalloweenEventRiddleIndices();
	TArray<int32> Fresh;
	for (int32 Index : Pool)
	{
		if (!Seen.Contains(Index))
		{
			Fresh.Add(Index);
		}
	}
	if (Fresh.Num() == 0)
	{
		// Every riddle of the pool has been asked: start over (the list keeps whatever else it holds).
		Seen.RemoveAll([&Pool](int32 Index) { return Pool.Contains(Index); });
		Fresh = Pool;
	}
	Riddle = Fresh.Num() > 0 ? Fresh[FMath::RandRange(0, Fresh.Num() - 1)] : -1;
	if (ForcedRiddle >= 0)
	{
		Riddle = ForcedRiddle;
		ForcedRiddle = -1;
	}
	Chosen = -1;
	if (Riddle >= 0)
	{
		Seen.AddUnique(Riddle);
	}
	for (int32 I = 0; I < 4; ++I)
	{
		Order[I] = I;
	}
	for (int32 I = 3; I > 0; --I)
	{
		Swap(Order[I], Order[FMath::RandRange(0, I)]);
	}
	return Riddle;
}

void UHalloweenEvent::ForceLongestRiddle()
{
	const TArray<FPuzzleRiddle>& All = PuzzleRiddles();
	int32 Longest = -1;
	int32 LongestSize = 0;
	for (int32 Index : HalloweenEventRiddleIndices())
	{
		const FPuzzleRiddle& Entry = All[Index];
		int32 Size = FCString::Strlen(Entry.Question) + FCString::Strlen(Entry.Lore);
		for (const TCHAR* Answer : Entry.Answers)
		{
			Size = FMath::Max(Size, FCString::Strlen(Entry.Question) + FCString::Strlen(Entry.Lore) + FCString::Strlen(Answer) * 4);
		}
		if (Size > LongestSize)
		{
			LongestSize = Size;
			Longest = Index;
		}
	}
	ForcedRiddle = Longest;
}

int32 UHalloweenEvent::RightPosition() const
{
	for (int32 Position = 0; Position < 4; ++Position)
	{
		if (Order[Position] == 0)
		{
			return Position;
		}
	}
	return 0;
}

bool UHalloweenEvent::AnswerRiddle(int32 Position)
{
	if (Riddle < 0 || Chosen >= 0 || Position < 0 || Position > 3)
	{
		return false;
	}
	Chosen = Position;
	const bool bRight = Position == RightPosition();
	bLastRight = bRight;
	++(bRight ? Treats : Tricks);
	UE_LOG(LogTemp, Display, TEXT("HalloweenEvent: %s (treats %d, tricks %d)"), bRight ? TEXT("Treat!") : TEXT("Trick!"), Treats, Tricks);
	return bRight;
}

void UHalloweenEvent::FinishRiddle()
{
	Riddle = -1;
	Chosen = -1;
}
