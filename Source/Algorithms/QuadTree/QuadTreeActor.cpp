// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeActor.h"
#include "Components/BoxComponent.h"

AQuadTreeActor::AQuadTreeActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AQuadTreeActor::BeginPlay()
{
	Super::BeginPlay();
	
	Tree = MakeUnique<FQuadTree>(GetActorLocation(), FVector(TotalArea.X, TotalArea.Y, 1.f));
	
	Insert();
}

void AQuadTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Query();
	DrawBounds();
}

void AQuadTreeActor::Insert()
{
	if (ObstacleActors.Num() == 0)
		return;
	
	for (TObjectPtr<AActor> Obstacle : ObstacleActors)
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

void AQuadTreeActor::Query()
{
	if (MovableActor == nullptr)
		return;
	
	if (!MovableBox.IsValid())
		MovableBox = Cast<UBoxComponent>(MovableActor->GetComponentByClass(UBoxComponent::StaticClass()));
	
	IntersectBounds.Empty();
	PossibleNodes.Empty();
	
	Tree->Query(MovableBox.Get(), IntersectBounds, PossibleNodes);	
}

void AQuadTreeActor::DrawBounds()
{
	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	FVector Extents (TotalArea.X * 0.5f, TotalArea.Y * 0.5f, 1.f);
	//FColor  = FColor(125, 125, 125, 255);
	
	DrawDebugBox(World, Location, Extents, FColor::White);
	
	if (Tree.IsValid())
		Tree->DrawBounds(World);
	
	// for (TWeakObjectPtr<UBoxComponent> BoxComp : ObstacleBoxes)
	// {
	// 	FVector Locate = BoxComp->GetOwner()->GetActorLocation();
	// 	Locate.Z = 11.f;
	// 	DrawDebugBox(World, Locate, BoxComp->Bounds.BoxExtent, FColor::Red);
	// }
	
	if (MovableBox.IsValid())
	{
		FVector MoveableLocate = MovableActor->GetActorLocation();
		MoveableLocate.Z = 11.f;
		DrawDebugBox(World, MoveableLocate, MovableBox->Bounds.BoxExtent, FColor::Orange);
	}
	
	if (PossibleNodes.Num() > 0)
	{
		// 검사한 노드 모두 Draw
		for (FQTNode* PossibleNode : PossibleNodes)
		{	
			FVector PossibleLocate = FVector(PossibleNode->Bounds.Center.X, PossibleNode->Bounds.Center.Y, 12.f);
			DrawDebugBox(World, PossibleLocate, PossibleNode->Bounds.Extents, FColor::Yellow);
		}
	}
	
	for (UBoxComponent* BoxComponent : ObstacleBoxes)
	{
		FVector ObstacleLocate = BoxComponent->GetOwner()->GetActorLocation();
		ObstacleLocate.Z = 13.f;
		
		bool bIntersect = false;
		
		for (TWeakObjectPtr<UBoxComponent> Bound : IntersectBounds)
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

