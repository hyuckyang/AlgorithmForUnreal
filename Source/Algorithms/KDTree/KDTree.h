// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FKDBounds;
struct FKDTransform;
class FKDNode;

/**
 * 
 */
class ALGORITHMS_API FKDTree
{
	TUniquePtr<FKDNode> RootNode;
	int32 Dimension;
	
public:
	FKDTree(int32 InDimension);
	void Insert(FKDTransform NewTransform);
	
	// bool 과 null 이 필요 없지만
	bool Query(const FKDTransform& QueryTransform, FKDTransform& ResultTransform, TArray<FKDNode*>& SearchNodes) const;
	
	void Draw(const UWorld* World, const FKDBounds& Bounds) const;
};
