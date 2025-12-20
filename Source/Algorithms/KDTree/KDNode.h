// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KDNode.generated.h"

USTRUCT()
struct FKDBounds
{
	GENERATED_BODY()
	
	FVector Center;
	FVector Extents;
	
	FKDBounds();
	FKDBounds(const FVector& InCenter, const FVector& InExtents);
	
	FVector GetSize() const { return Extents * 2.f; }
	FVector GetMin() const { return Center - Extents; }
	FVector GetMax() const { return Center + Extents; }
	
	void SetMinMax(const FVector& Min, const FVector& Max);
	void SetMin(const FVector& Min);
	void SetMax(const FVector& Max);
	
	void Clear();
};

USTRUCT()
struct FKDTransform
{
	GENERATED_BODY()
	
	FVector Location;
	FKDBounds Bounds;
	
	FKDTransform();
	FKDTransform(const FVector& InLocation);
	FKDTransform(const FVector& InLocation, const FVector& InScale);
	
	float GetKDValue(int32 Dimension, int32 Depth);
	float GetDistanceTo(const FVector& TargetLocation) const;
	float GetSquaredDistanceTo(const FVector& TargetLocation) const;
	
	bool operator==(const FKDTransform& Outer) const;
	
};

/**
 * 
 */
class ALGORITHMS_API FKDNode
{
private:
	
	FKDTransform Transform;
	FKDBounds DrawBounds[2];
	
	FKDNode* Parent = nullptr;
	FKDNode* Right = nullptr;
	FKDNode* Left = nullptr;
	
	int32 Dimension = -1; // 차원
	int32 Depth = -1; // 깊이
	
public:
	
	FKDNode(int32 InDimension, FKDNode* InParent, const FKDTransform& InTransform, int32 InDepth);
	~FKDNode();

	void Draw(const UWorld* World, const FKDBounds& Bounds);
	FKDNode* CheckNearestNode(FKDTransform TestTransform, FKDNode* NearestNode, TArray<FKDNode*>& SearchedNodes); // 검사
	
	void SetRight ( FKDNode* Node ) { Right = Node; }
	FKDNode* GetRight () const { return Right; }
	
	void SetLeft ( FKDNode* Node ) { Left = Node; }
	FKDNode* GetLeft () const { return Left; }
	
	bool GetIsRight() const { return Right != nullptr; }
	bool GetIsLeft() const { return Left != nullptr; }
	bool GetIsLeaf () const  { return Left == nullptr && Right == nullptr; }
	
	const FKDTransform& GetTransform() const { return Transform; }
	
	// 현재 차원의 X, Y, Z 중 하나를 가져 옵니다.
	float GetKDValue() { return Transform.GetKDValue(Dimension, Depth); };
	float GetSquaredDistanceTo(const FVector& TargetLocation) const; // 제곱근을 구하지 않는 거리 
	float GetDistanceTo(const FVector& TargetLocation) const; // 거리
	
	
};
