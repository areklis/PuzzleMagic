#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArcadeCampaign.generated.h"

// Where an arcade challenge stands.
UENUM(BlueprintType)
enum class EArcadeState : uint8
{
	None,
	Intro,    // the "before" popup is up
	Playing,  // the clock is running
	Won,      // the goal score was reached; the "after" popup follows
	Failed,   // time ran out, or no piece fits
	Done      // the last challenge is beaten
};

// One challenge of an arcade campaign: a score to reach on a board with certain relics and bonus tiles in play,
// optionally against the clock, with a popup of your own text before and after.
USTRUCT(BlueprintType)
struct PUZZLEGAME5X5_API FArcadeChallenge
{
	GENERATED_BODY()

	// Shown in the popups and on the screen while playing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	FString Title = TEXT("New challenge");

	// The challenge is won as soon as the score reaches this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge", meta = (ClampMin = "1", UIMin = "1"))
	int32 TargetScore = 200;

	// Seconds to reach the score in. 0 means no time limit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge", meta = (ClampMin = "0", UIMin = "0", Units = "s"))
	int32 TimeLimitSeconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Board", meta = (ClampMin = "4", ClampMax = "8", UIMin = "4", UIMax = "8"))
	int32 GridWidth = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Board", meta = (ClampMin = "4", ClampMax = "8", UIMin = "4", UIMax = "8"))
	int32 GridHeight = 8;

	// Relics are earned by building a combo. Switching either one on also switches combos and luck on.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Relics")
	bool bHolyLight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Relics")
	bool bReroll = false;

	// Bonus tiles that drop onto the board as the score grows.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Bonus tiles")
	bool bPumpkin = false;

	// The full potion bottle (outgoing).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Bonus tiles")
	bool bOutgoingBottle = false;

	// The empty potion bottle (incoming).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Bonus tiles")
	bool bIncomingBottle = false;

	// The popup shown before the challenge starts. Leave empty to show only the rules.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Popups", meta = (MultiLine = "true"))
	FString IntroText;

	// The popup shown after the challenge is won. Leave empty to show only the result.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge|Popups", meta = (MultiLine = "true"))
	FString OutroText;
};

// A campaign of arcade challenges played one after another. The game loads /Game/Arcade/DA_ArcadeCampaign: open it in the
// Content Browser, and add, remove and reorder the challenges in the Details panel.
UCLASS(BlueprintType)
class PUZZLEGAME5X5_API UArcadeCampaign : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Campaign")
	FString CampaignName = TEXT("Arcade");

	// Played top to bottom. Beating one opens the next.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Campaign", meta = (TitleProperty = "Title"))
	TArray<FArcadeChallenge> Challenges;

	// The popup shown after the last challenge.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Campaign", meta = (MultiLine = "true"))
	FString FinaleText = TEXT("You beat every challenge!");
};
