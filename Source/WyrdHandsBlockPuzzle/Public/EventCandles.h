#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventCandles.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

// The candles above the play area in the Halloween event run (Specs/TrickOrTreatEvent.md): a small clutter that fills from left
// to right, one fifth for each day's first success. Five fifths (five days) unlock the Halloween theme.
UCLASS()
class WYRDHANDSBLOCKPUZZLE_API AEventCandles : public AActor
{
	GENERATED_BODY()

public:
	static constexpr int32 Fifths = 5;

	AEventCandles();

	virtual void Tick(float DeltaTime) override;

	// How many fifths are lit (0 to 5). A new one pops in when Animate is set (a success just happened).
	void SetFifths(int32 Count, bool bAnimate);
	// Puts the clutter where the camera's framing wants it (above the play area).
	void Refit();

private:
	struct FGroup
	{
		TObjectPtr<USceneComponent> Anchor = nullptr;
		TArray<TObjectPtr<UStaticMeshComponent>> Parts;
		bool bLit = false;
		float PopAge = 1.f;
		FVector Where = FVector::ZeroVector;
	};

	void SetLit(int32 Index, bool bLit, bool bAnimate);
	UStaticMeshComponent* AddPart(FGroup& Group, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, UMaterialInterface* Material);
	UMaterialInstanceDynamic* MakeMaterial(const FLinearColor& Base, const FLinearColor& Vein, float VeinStrength, float Roughness);
	void ApplyScale(FGroup& Group) const;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> StoneMaterial;

	FGroup Groups[Fifths];
	float BaseScale = 0.7f;
	float RefitTimer = 0.f;
};
