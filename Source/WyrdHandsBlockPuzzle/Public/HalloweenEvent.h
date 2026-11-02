#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PuzzleStyle.h"
#include "HalloweenEvent.generated.h"

class UPuzzleManager;
class UPuzzleSaveGame;

// The rules of the Halloween seasonal event run (Specs/TrickOrTreatEvent.md): every RiddleEvery points a Halloween riddle appears.
// A right answer is a Treat!, a wrong one a Trick!. TricksToLose tricks end the run (game over); TreatsToWin treats are a success
// (the first time of the day). The scene (the spots on the floor, the candles) and the end screens are in APuzzleGameMode and
// UPuzzleHUDWidget; this class keeps the counts and the riddle.
UCLASS()
class WYRDHANDSBLOCKPUZZLE_API UHalloweenEvent : public UObject
{
	GENERATED_BODY()

public:
	static constexpr int32 RiddleEvery = 300;
	static constexpr int32 TreatsToWin = 5;
	static constexpr int32 TricksToLose = 5;
	static constexpr int32 BoardSize = 6;

	void Init(UPuzzleManager* InRules);
	// A new run: the counts and the riddle are cleared, the first riddle comes at RiddleEvery points.
	void StartRun();

	// After every scoring move: true when the score has just reached the next riddle (one at a time).
	bool CheckRiddleDue();

	// Picks a Halloween-event riddle not yet in Seen (the save game's list; once the whole pool has been asked it starts over),
	// adds it there and shuffles its answers. Returns the riddle's index in PuzzleRiddles().
	int32 StartRiddle(TArray<int32>& Seen);
	// Dev (layout tests): the next riddle asked is the one with the most text.
	void ForceLongestRiddle();
	int32 GetRiddle() const { return Riddle; }
	// The answer shown at Position (0-3), as an index into FPuzzleRiddle::Answers (0 is the right one).
	int32 AnswerAt(int32 Position) const { return Order[FMath::Clamp(Position, 0, 3)]; }
	// Where the right answer is shown (0-3).
	int32 RightPosition() const;
	// -1 until the riddle is answered.
	int32 GetChosenPosition() const { return Chosen; }
	// Answers with the choice at Position. True for a right answer (a Treat!), false for a wrong one (a Trick!); counted.
	bool AnswerRiddle(int32 Position);
	// The riddle is done with: the next one is RiddleEvery points after the last threshold passed.
	void FinishRiddle();

	// True when the last answer was right (a Treat!), false when it was wrong (a Trick!).
	bool WasLastRight() const { return bLastRight; }

	// The event window, by the local date: on from 1 October through 30 November 2026 (the designer will fine-tune it).
	static bool IsActiveToday();
	// Days since 1 January 0001 of the local date: the save's list of success days (UPuzzleSaveGame::EventSuccessDays) uses it.
	static int32 TodayNumber();
	// The theme the player's Theme option asks for: Auto = the event's theme during an event and Desk otherwise, Desk = Desk,
	// Halloween (once five successes have unlocked it) = Halloween.
	static EPuzzleTheme ThemeFor(const UPuzzleSaveGame* Save);

	int32 GetTreats() const { return Treats; }
	int32 GetTricks() const { return Tricks; }
	bool IsLost() const { return Tricks >= TricksToLose; }
	bool IsWon() const { return Treats >= TreatsToWin; }

private:
	UPROPERTY()
	TObjectPtr<UPuzzleManager> Rules;

	int32 NextRiddleScore = RiddleEvery;
	int32 Treats = 0;
	int32 Tricks = 0;
	int32 Riddle = -1;
	int32 Chosen = -1;
	bool bLastRight = false;
	int32 Order[4] = { 0, 1, 2, 3 };
	int32 ForcedRiddle = -1;
};
