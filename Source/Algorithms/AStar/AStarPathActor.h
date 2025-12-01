// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarPathActor.generated.h"

struct FAStarNode;
class AAStarGridActor;

UCLASS()
class ALGORITHMS_API AAStarPathActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAStarPathActor();
	
protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY()
	TObjectPtr<AAStarGridActor> GridActor;
	
private:
	UPROPERTY(EditInstanceOnly, Category="AStar")
	TObjectPtr<AActor> StartActor;
	
	UPROPERTY(EditInstanceOnly, Category="AStar")
	TObjectPtr<AActor> EndActor;
	
	//FVector CacheStartPosition, CacheEndPosition;
	void FindPath(const FVector& StartPos, const FVector& EndPos) const;
	void RetracePath(const FAStarNode* StartNode, const FAStarNode* EndNode) const;
	static int32 GetDistance(int32 NodeAGridX, int32 NodeAGridY, int32 NodeBGridX, int32 NodeBGridY);
};
