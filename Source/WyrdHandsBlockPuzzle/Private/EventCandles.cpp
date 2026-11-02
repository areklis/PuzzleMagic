#include "EventCandles.h"
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

namespace
{
	// Five clusters in a row behind the board, left to right; their size and spacing are fractions of the board's width.
	constexpr float ClusterOfBoard = 0.085f;
	constexpr float SpacingOfBoard = 0.13f;
	constexpr float BehindBoard = 0.07f;
	constexpr float CandlePopTime = 0.5f;
	// A cluster of three candles is about this wide at scale 1.
	constexpr float ClusterWidth = 110.f;

	float CandleEaseOutBack(float T)
	{
		const float C1 = 1.70158f;
		const float C3 = C1 + 1.f;
		const float U = T - 1.f;
		return 1.f + C3 * U * U * U + C1 * U * U;
	}
}

AEventCandles::AEventCandles()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MarbleFinder(TEXT("/Game/Materials/M_Marble.M_Marble"));
	SphereMesh = SphereFinder.Object;
	CylinderMesh = CylinderFinder.Object;
	StoneMaterial = MarbleFinder.Object;

	for (int32 Index = 0; Index < Fifths; ++Index)
	{
		USceneComponent* Anchor = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("Group%d"), Index));
		Anchor->SetupAttachment(RootComponent);
		Groups[Index].Anchor = Anchor;
	}
}

void AEventCandles::Refit()
{
	// The candles stand on the floor just behind the board, placed and sized by the board (not by the camera), so they follow it
	// whatever the screen shape and never leave the picture.
	TActorIterator<AGridManager> It(GetWorld());
	if (It)
	{
		const FBox2D Bounds = It->GetContentBounds();
		const float Width = static_cast<float>(Bounds.Max.X - Bounds.Min.X);
		const float CenterX = static_cast<float>(Bounds.Min.X + Bounds.Max.X) * 0.5f;
		const float BackY = static_cast<float>(Bounds.Min.Y); // the far edge of the board (the camera stands on the + side)
		BaseScale = FMath::Clamp(Width * ClusterOfBoard / ClusterWidth, 0.05f, 3.f);
		for (int32 Index = 0; Index < Fifths; ++Index)
		{
			Groups[Index].Where = FVector(CenterX + (Index - 2) * Width * SpacingOfBoard, BackY - Width * BehindBoard, 0.f);
			if (Groups[Index].Anchor)
			{
				Groups[Index].Anchor->SetWorldLocation(Groups[Index].Where);
				ApplyScale(Groups[Index]);
			}
		}
	}
}

void AEventCandles::ApplyScale(FGroup& Group) const
{
	const float Pop = Group.PopAge >= 1.f ? 1.f : FMath::Max(CandleEaseOutBack(Group.PopAge), 0.001f);
	Group.Anchor->SetWorldScale3D(FVector(BaseScale * Pop));
}

void AEventCandles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (FGroup& Group : Groups)
	{
		if (Group.bLit && Group.PopAge < 1.f)
		{
			Group.PopAge = FMath::Min(Group.PopAge + DeltaTime / CandlePopTime, 1.f);
			ApplyScale(Group);
		}
	}
	RefitTimer -= DeltaTime;
	if (RefitTimer <= 0.f)
	{
		RefitTimer = 0.5f;
		Refit();
	}
}

void AEventCandles::SetFifths(int32 Count, bool bAnimate)
{
	Count = FMath::Clamp(Count, 0, Fifths);
	for (int32 Index = 0; Index < Fifths; ++Index)
	{
		const bool bWant = Index < Count;
		if (bWant != Groups[Index].bLit)
		{
			// Only the newest one animates; the older ones are simply there.
			SetLit(Index, bWant, bAnimate && Index == Count - 1);
		}
	}
	Refit();
}

void AEventCandles::SetLit(int32 Index, bool bLit, bool bAnimate)
{
	FGroup& Group = Groups[Index];
	for (UStaticMeshComponent* Part : Group.Parts)
	{
		if (Part)
		{
			Part->DestroyComponent();
		}
	}
	Group.Parts.Reset();
	Group.bLit = bLit;
	Group.PopAge = (bLit && bAnimate) ? 0.f : 1.f;
	if (bLit)
	{
		// Three candles of different heights, each with a small flame.
		UMaterialInstanceDynamic* Wax = MakeMaterial(FLinearColor(1.f, 0.93f, 0.75f), FLinearColor(1.f, 0.97f, 0.85f), 0.1f, 0.5f);
		UMaterialInstanceDynamic* Flame = MakeMaterial(FLinearColor(1.f, 0.62f, 0.12f), FLinearColor(1.f, 0.85f, 0.3f), 0.5f, 0.3f);
		static const float Heights[3] = { 90.f, 130.f, 70.f };
		static const float Xs[3] = { -38.f, 0.f, 38.f };
		static const float Ys[3] = { 8.f, -6.f, 10.f };
		for (int32 Candle = 0; Candle < 3; ++Candle)
		{
			const float H = Heights[Candle];
			AddPart(Group, CylinderMesh, FVector(Xs[Candle], Ys[Candle], H * 0.5f), FVector(0.28f, 0.28f, H / 100.f), Wax);
			AddPart(Group, SphereMesh, FVector(Xs[Candle], Ys[Candle], H + 14.f), FVector(0.2f, 0.2f, 0.38f), Flame);
		}
	}
	if (Group.Anchor)
	{
		ApplyScale(Group);
	}
}

UMaterialInstanceDynamic* AEventCandles::MakeMaterial(const FLinearColor& Base, const FLinearColor& Vein, float VeinStrength, float Roughness)
{
	UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(StoneMaterial, this);
	Material->SetVectorParameterValue(TEXT("BaseColor"), Base);
	Material->SetVectorParameterValue(TEXT("VeinColor"), Vein);
	Material->SetScalarParameterValue(TEXT("VeinStrength"), VeinStrength);
	Material->SetScalarParameterValue(TEXT("Roughness"), Roughness);
	return Material;
}

UStaticMeshComponent* AEventCandles::AddPart(FGroup& Group, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, UMaterialInterface* Material)
{
	UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this);
	Component->SetStaticMesh(Mesh);
	Component->SetupAttachment(Group.Anchor);
	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetCastShadow(true);
	Component->RegisterComponent();
	Component->SetRelativeLocation(Location);
	Component->SetRelativeScale3D(Scale);
	if (Material)
	{
		Component->SetMaterial(0, Material);
	}
	Group.Parts.Add(Component);
	return Component;
}
