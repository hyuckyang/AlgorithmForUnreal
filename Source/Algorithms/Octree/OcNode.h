// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OcNode.generated.h"

class FOctree;
class UBoxComponent;


UENUM()
enum class OcNodeIndex : uint8
{	
	TOPUPPERLEFT = 0, // Top
	TOPUPPERRIGHT,
	TOPLOWERRIGHT,
	TOPLOWERLEFT,
	BOTTOMUPPERLEFT, // Bottom
	BOTTOMUPPERRIGHT,
	BOTTOMLOWERRIGHT,
	BOTTOMLOWERLEFT,
	STRADDLING,  // 경계선에 걸친 경우
	OUTOFAREA // 영역 밖을 벗어난 경우. ( 입력 에러 )
};

USTRUCT()
struct FOcBounds
{
	GENERATED_BODY()
	
	FVector Center;
	FVector Extents;
	
	FOcBounds();
	FOcBounds(const FVector& InCenter, const FVector& InExtents);
	FOcBounds(const UBoxComponent* BoxComponent);
	
	FVector GetSize() const { return Extents * 2.f; }
	FVector GetMin() const { return Center - Extents; }
	FVector GetMax() const { return Center + Extents; }
	
	bool operator==(const FOcBounds& Outer) const;
};

/**
 * 
 */
class ALGORITHMS_API FOcNode
{
public:
	TArray<TWeakObjectPtr<UBoxComponent>> Items;
	TArray<FOcNode*> Children;
	int32 Depth = 0;
	FOcBounds Bounds;
	
private:
	FOctree* Tree;
	FOcNode* Parent;
	
public:
	
	FOcNode(FOctree* InTree, FOcNode* InParent, FOcBounds InBound, int32 InDepth);
	~FOcNode();
	
	void Insert(UBoxComponent* Item);
	void Query(UBoxComponent* Item, TArray<FOcNode*>& PossibleNodes);
	void Clear();
	
	void DrawBounds(const UWorld* World);
	
private:
	TArray<OcNodeIndex> GetOctants(FOcBounds InBounds);
	OcNodeIndex TestRegion(FOcBounds InBounds);
	bool Split();
	//
	FORCEINLINE bool IsSplitted() const { return Children.Num() > 0; } 
};
