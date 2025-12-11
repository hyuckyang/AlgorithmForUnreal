// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OcNode.h"
#include "Octree.h"
#include "OcTreeActor.generated.h"

UCLASS()
class ALGORITHMS_API AOctreeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere)
	FVector TotalArea;
	
	UPROPERTY(EditInstanceOnly, Category="Octree")
	TArray<TObjectPtr<AActor>> ObstacleActors;
	
	UPROPERTY(EditInstanceOnly, Category="Octree")
	TObjectPtr<AActor> MovableActor; // 질의 (범위) 액터
	
private:
	
	TUniquePtr<FOctree> Tree;
	TArray<FOcNode*> PossibleNodes; // 질의(충돌) 을 시도한 모든 노드 가져옴
	TArray<TWeakObjectPtr<UBoxComponent>> IntersectBounds;
	
	TArray<TObjectPtr<UBoxComponent>> ObstacleBoxes;
	TWeakObjectPtr<UBoxComponent> MovableBox;
	
public:
	AOctreeActor();
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void Insert();
	void Query();
	
protected:
	void DrawBounds();
};
