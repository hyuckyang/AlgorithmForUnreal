// Fill out your copyright notice in the Description page of Project Settings.


#include "OcTreeActor.h"
#include "Components/BoxComponent.h"

// Sets default values
AOctreeActor::AOctreeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOctreeActor::BeginPlay()
{
	Super::BeginPlay();
	
	Tree = MakeUnique<FOctree>(GetActorLocation(), TotalArea);
	
	Insert();
}

// Called every frame
void AOctreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Query();
	DrawBounds();
}

void AOctreeActor::Insert()
{
	if (ObstacleActors.Num() == 0)
		return;
	
	for (TObjectPtr Obstacle : ObstacleActors)
	{
		if (Obstacle == nullptr)
			continue;
		
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(Obstacle->GetComponentByClass(UBoxComponent::StaticClass())))
		{
			ObstacleBoxes.Add(BoxComp);
			Tree->Insert(BoxComp);
		}
	}
}

void AOctreeActor::Query()
{
	if (MovableActor == nullptr)
		return;
	
	if (!MovableBox.IsValid())
		MovableBox = Cast<UBoxComponent>(MovableActor->GetComponentByClass(UBoxComponent::StaticClass()));
	
	IntersectBounds.Empty();
	PossibleNodes.Empty();
	
	Tree->Query(MovableBox.Get(), IntersectBounds, PossibleNodes);
}

void AOctreeActor::DrawBounds()
{
	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	FVector Extents (TotalArea.X * 0.5f, TotalArea.Y * 0.5f, TotalArea.Z * 0.5f);
	
	DrawDebugBox(World, Location, Extents, FColor::White);
	
	if (Tree.IsValid())
		Tree->DrawBounds(World);
	
	if (MovableBox.IsValid())
	{
		FVector MoveableLocate = MovableActor->GetActorLocation();
		//MoveableLocate.Z = 11.f;
		DrawDebugBox(World, MoveableLocate, MovableBox->Bounds.BoxExtent, FColor::Orange);
	}
	
	if (PossibleNodes.Num() > 0)
	{
		// 검사한 노드 모두 Draw
		for (FOcNode* PossibleNode : PossibleNodes)
		{	
			DrawDebugBox(World, PossibleNode->Bounds.Center, PossibleNode->Bounds.Extents, FColor::Yellow);
		}
	}
	
	for (UBoxComponent* BoxComponent : ObstacleBoxes)
	{
		FVector ObstacleLocate = BoxComponent->GetOwner()->GetActorLocation();
		//ObstacleLocate.Z = 13.f;
		
		bool bIntersect = false;
		for (TWeakObjectPtr Bound : IntersectBounds)
		{
			if (Bound.Get() == BoxComponent)
			{
				bIntersect = true;
				break;
			}
		}
		
		DrawDebugBox(World, ObstacleLocate, BoxComponent->Bounds.BoxExtent, bIntersect ? FColor::Red : FColor::Green);
	}
}

