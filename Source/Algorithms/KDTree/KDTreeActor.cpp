// Fill out your copyright notice in the Description page of Project Settings.


#include "KDTreeActor.h"
#include "Components/BoxComponent.h"

AKDTreeActor::AKDTreeActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKDTreeActor::BeginPlay()
{
	Super::BeginPlay();
	
	Tree = MakeUnique<FKDTree>(Dimension);
	Bounds = FKDBounds(GetActorLocation(), TotalArea);
	
	Insert();
}

void AKDTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Query();
	Draw();
}

void AKDTreeActor::Insert()
{
	if (ObstacleActors.Num() == 0)
		return;
	
	for (TObjectPtr Obstacle : ObstacleActors)
	{
		if (Obstacle == nullptr)
			continue;
		
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Obstacle->GetComponentByClass(UBoxComponent::StaticClass())))
		{
			Tree->Insert(FKDTransform(Obstacle->GetActorLocation(), BoxComp->Bounds.BoxExtent));
		}
	}
}

void AKDTreeActor::Query()
{	
	if (MovableActor == nullptr)
		return;
	
	if (!MovableBox.IsValid())
		MovableBox = Cast<UBoxComponent>(MovableActor->GetComponentByClass(UBoxComponent::StaticClass()));
	
	SearchKDNodes.Empty();
	bIsNearest = Tree->Query(FKDTransform(MovableActor->GetActorLocation(), MovableBox->Bounds.BoxExtent), NearestTransform, SearchKDNodes);
}

void AKDTreeActor::Draw() const
{
	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	// FVector Extents (TotalArea.X * 0.5f, TotalArea.Y * 0.5f, TotalArea.Z * 0.5f);
	DrawDebugBox(World, Location, TotalArea * 0.5f, FColor(0.5f, 0.5f, 0.5f, 1.0f));
	
	if (Tree.IsValid())
		Tree->Draw(World, Bounds);
	
	if (MovableBox == nullptr)
		return;
	
	DrawDebugBox(World, MovableActor->GetActorLocation(), MovableBox->Bounds.BoxExtent, FColor::Red);
	
	if (bIsNearest)
	{
		DrawDebugSolidBox(World, NearestTransform.Bounds.Center, NearestTransform.Bounds.Extents * 1.1f, FColor::Magenta);
		DrawDebugLine(World, NearestTransform.Location, MovableActor->GetActorLocation(), FColor::Magenta);
	}
}
