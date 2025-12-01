// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.generated.h"

USTRUCT()
struct FAStarNode
{
	GENERATED_BODY()

	uint8 IsWalk : 1;
	FVector Position;
	int32 GridX;
	int32 GridY;
	
	int32 GCost; 
	int32 HCost;
	
	int32 ParentX;
	int32 ParentY;
	
	FAStarNode()
		: IsWalk(false)
		  , Position(FVector::ZeroVector)
		  , GridX(0)
		  , GridY(0)
		  , GCost(0)
		  , HCost(0)
	      , ParentX(0)
	      , ParentY(0)
	{
	}

	FAStarNode(bool bWalkable, const FVector& InPosition, int32 InGridX, int32 InGridY)
		: IsWalk(bWalkable)
		  , Position(InPosition)
		  , GridX(InGridX)
		  , GridY(InGridY)
		  , GCost(0)
		  , HCost(0)
	      , ParentX(0)
		  , ParentY(0)
	{
	}

	int32 GetFCost() const
	{
		return GCost + HCost;
	}
}; 

