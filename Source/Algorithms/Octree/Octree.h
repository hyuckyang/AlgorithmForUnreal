// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FOcBounds;
class FOcNode;
class UBoxComponent;

/**
 * 
 */
class ALGORITHMS_API FOctree
{
	TUniquePtr<FOcNode> RootNode;
	int32 MaxDepth = 5; // 최대 깊이
	
public:
	
	FOctree(FVector Position, FVector Size);
	~FOctree();
	
	void Insert(UBoxComponent* ItemBox);
	void Query(UBoxComponent* QueryItem,TArray<TWeakObjectPtr<UBoxComponent>>& Intersect, TArray<FOcNode*>& Possible);
	void DrawBounds(const UWorld* World);
	
	FORCEINLINE int32 GetMaxDepth() const { return MaxDepth; }
};
