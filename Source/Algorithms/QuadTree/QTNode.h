// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QTNode.generated.h"

class UBoxComponent;
class FQuadTree;

UENUM()
enum class QTNodeIndex : uint8
{
	UPPERLEFT = 0,
	UPPERRIGHT,
	LOWERRIGHT,
	LOWERLEFT,
	STRADDLING, // 경계선에 걸친 경우
	OUTOFAREA // 영역 밖을 벗어난 경우. ( 입력 에러 )
};

USTRUCT()
struct FQTBounds 
{
	GENERATED_BODY()
	
	FVector Center;
	FVector Extents;
	
	FQTBounds();
	FQTBounds(const FVector& InCenter, const FVector& InExtents);
	FQTBounds(const UBoxComponent* BoxComponent);
	
	FVector GetSize() const { return Extents * 2.f; }
	FVector GetMin() const { return Center - Extents; }
	FVector GetMax() const { return Center + Extents; }
	
	bool operator==(const FQTBounds& Outer) const;
	
};

/**
 * 
 */
class ALGORITHMS_API FQTNode
{
public:
	
	TArray<TWeakObjectPtr<UBoxComponent>> Items;
	TArray<FQTNode*> Children;
	int32 Depth = 0;
	FQTBounds Bounds;
	
private:
	FQuadTree* Tree;
	FQTNode* Parent;
	
public:
	
	FQTNode(FQuadTree* InTree, FQTNode* InParent, FQTBounds InBound, int32 InDepth);
	~FQTNode();
	
	void Insert(UBoxComponent* Item);
	void Query(UBoxComponent* Item, TArray<FQTNode*>& PossibleNodes);
	void Clear();
	// void CollectAllNode(TMap<int32, TArray<const FQTNode*>> AllNodes);
	void DrawBounds(const UWorld* World);
	
	
private:
	TArray<QTNodeIndex> GetQuads(FQTBounds InBounds);
	QTNodeIndex TestRegion(FQTBounds InBounds);
	bool Split();
	//
	FORCEINLINE bool IsSplitted() const { return Children.Num() > 0; } 
	
	
	
};
