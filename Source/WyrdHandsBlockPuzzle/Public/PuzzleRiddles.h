#pragma once

#include "CoreMinimal.h"

// One question of the Trick-or-Treat packet's RIDDLE: dark folklore, Halloween and New Orleans Mardi Gras.
// Answers[0] is always the right one; the card shuffles them.
struct FPuzzleRiddle
{
	const TCHAR* Topic;
	const TCHAR* Question;
	const TCHAR* Answers[4];
	// One line told after the answer, right or wrong.
	const TCHAR* Lore;
};

const TArray<FPuzzleRiddle>& PuzzleRiddles();

// The Halloween event asks every topic except DAY OF THE DEAD and the MARDI GRAS / NEW ORLEANS ones (the designer's choice).
bool IsHalloweenEventRiddle(const FPuzzleRiddle& Riddle);
// Indices into PuzzleRiddles() of the riddles the Halloween event may ask.
const TArray<int32>& HalloweenEventRiddleIndices();
