#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventSpots.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

enum class EEventSpotKind : uint8
{
	Empty,
	Cauldron,
	Pumpkin
};

// The floor of the Halloween event run (Specs/TrickOrTreatEvent.md): eight spots beside the play area, four on the left and
// four on the right. A Treat! puts a cauldron on a random empty spot, a Trick! a pumpkin. When all eight are taken a Treat!
// adds nothing and a Trick! turns a random cauldron into a pumpkin. The spots stand where the camera puts them (a screen
// position projected onto the floor), so they follow the framing of every board size and screen shape.
UCLASS()
class WYRDHANDSBLOCKPUZZLE_API AEventSpots : public AActor
{
	GENERATED_BODY()

public:
	static constexpr int32 SpotCount = 8;

	AEventSpots();

	virtual void Tick(float DeltaTime) override;

	// Puts the spots where the camera's framing wants them (called when the framing changes, and now and then).
	void Refit();
	// Every spot empty again (a new run).
	void Clear();
	// A cauldron on a random empty spot. False when all eight are taken.
	bool AddCauldron();
	// A pumpkin on a random empty spot, or when all are taken in place of a random cauldron. False when there is nothing to take.
	bool AddPumpkin();

	int32 CountOf(EEventSpotKind Kind) const;

private:
	struct FSpot
	{
		TObjectPtr<USceneComponent> Anchor = nullptr;
		TArray<TObjectPtr<UStaticMeshComponent>> Parts;
		TArray<TObjectPtr<USceneComponent>> Frames; // the turned frame a pumpkin's face hangs on
		EEventSpotKind Kind = EEventSpotKind::Empty;
		float Fit = 1.f;      // the size this spot gets from where it stands on the screen
		float PopAge = 1.f;   // 0 when the thing has just appeared, 1 once it has settled
		FVector Where = FVector::ZeroVector;
	};

	void SetKind(int32 Index, EEventSpotKind Kind);
	void BuildCauldron(FSpot& Spot);
	void BuildPumpkin(FSpot& Spot);
	UStaticMeshComponent* AddPart(FSpot& Spot, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, UMaterialInterface* Material, USceneComponent* Parent = nullptr, float RollDegrees = 0.f);
	void BuildFace(FSpot& Spot);
	UMaterialInstanceDynamic* MakeMaterial(const FLinearColor& Base, const FLinearColor& Vein, float VeinStrength, float Roughness);
	void ApplyScale(FSpot& Spot) const;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> StoneMaterial;

	FSpot Spots[SpotCount];
	float BaseScale = 0.7f;
	FVector2D LastViewport = FVector2D::ZeroVector;
	float RefitTimer = 0.f;
};
