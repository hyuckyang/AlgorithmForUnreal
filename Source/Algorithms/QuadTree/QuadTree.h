// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FQTBounds;
class FQTNode;
class UBoxComponent;

/**
 * 
 */
class FQuadTree
{
private:
	TUniquePtr<FQTNode> RootNode;
	int32 MaxDepth = 5; // 최대 깊이

public:
	
	FQuadTree(FVector Position, FVector Size);
	~FQuadTree();
	
	void Insert(UBoxComponent* ItemBox);
	void Query(UBoxComponent* QueryItem,TArray<TWeakObjectPtr<UBoxComponent>>& Intersect, TArray<FQTNode*>& Possible);
	void DrawBounds(const UWorld* World);
	
	FORCEINLINE int32 GetMaxDepth() const { return MaxDepth; }
};
