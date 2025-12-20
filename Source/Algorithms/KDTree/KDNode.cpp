// Fill out your copyright notice in the Description page of Project Settings.


#include "KDNode.h"

FKDBounds::FKDBounds()
{
	Center = FVector::ZeroVector;
	Extents = FVector::ZeroVector;
}

FKDBounds::FKDBounds(const FVector& InCenter, const FVector& InExtents)
{
	Center = InCenter;
	Extents = InExtents * 0.5f;
}

void FKDBounds::SetMinMax(const FVector& Min, const FVector& Max)
{
	Center = (Min + Max) * 0.5f;
	Extents = (Max - Min) * 0.5f;
}

void FKDBounds::SetMin(const FVector& Min)
{
	FVector CurrentMax = GetMax();
	SetMinMax(Min, CurrentMax);
}

void FKDBounds::SetMax(const FVector& Max)
{
	FVector CurrentMin = GetMin();
	SetMinMax(CurrentMin, Max);
}

void FKDBounds::Clear()
{
	Center = FVector::ZeroVector;
	Extents = FVector::ZeroVector;
}

FKDTransform::FKDTransform() : Location(FVector::ZeroVector), Bounds(FVector::ZeroVector, FVector::OneVector)
{
	//
}

FKDTransform::FKDTransform(const FVector& InPosition) : Location(InPosition), Bounds(InPosition, FVector::OneVector)
{
	//
}

FKDTransform::FKDTransform(const FVector& InPosition, const FVector& InScale) : Location(InPosition), Bounds(InPosition, InScale)
{
	//
}

float FKDTransform::GetKDValue(int32 Dimension, int32 Depth)
{
	return Location[Depth % Dimension]; // x, y, z  등 차원에 따라 값을 전달
}

float FKDTransform::GetDistanceTo(const FVector& TargetLocation) const
{
	return (TargetLocation-Location).Size();
}

float FKDTransform::GetSquaredDistanceTo(const FVector& TargetLocation) const
{
	return (TargetLocation-Location).SizeSquared();
}

bool FKDTransform::operator==(const FKDTransform& Outer) const
{
	return Location == Outer.Location;
}

FKDNode::FKDNode(int32 InDimension, FKDNode* InParent, const FKDTransform& InTransform, int32 InDepth)
{
	Dimension = InDimension;
	Parent = InParent;
	Transform = InTransform;
	Depth = InDepth;
}

FKDNode::~FKDNode()
{
	delete Right;
	Right = nullptr;
	
	delete Left;
	Left = nullptr;
}

void FKDNode::Draw(const UWorld* World, const FKDBounds& Bounds)
{
	FVector PlaneCenter  = Bounds.Center;
	PlaneCenter[Depth % Dimension] = GetKDValue();
	FVector PlaneSize = Bounds.GetSize();
	PlaneSize[Depth % Dimension] = 0.f;
	
	// 왼쪽 AABB
	FVector LeftMin = Bounds.GetMin();
	FVector LeftMax = Bounds.GetMax();
	LeftMax[Depth % Dimension] = GetKDValue();
	DrawBounds[0].Clear();
	DrawBounds[0].SetMin(LeftMin);
	DrawBounds[0].SetMax(LeftMax);
	
	// 오른쪽 AABB
	FVector RightMin = Bounds.GetMin();
	RightMin[Depth % Dimension] = GetKDValue();
	FVector RightMax = Bounds.GetMax();
	DrawBounds[1].Clear();
	DrawBounds[1].SetMin(RightMin);
	DrawBounds[1].SetMax(RightMax);
	
	DrawDebugBox(World, PlaneCenter, PlaneSize * 0.5f, FColor(0.5f, 0.5f, 0.5f, 1.0f));
	DrawDebugSolidBox(World, Transform.Bounds.Center, Transform.Bounds.Extents, FColor::White);
	if (GetIsLeft())
	{
		Left->Draw(World, DrawBounds[0]);
	}
	
	if (GetIsRight())
	{
		Right->Draw(World, DrawBounds[1]);
	}
}

FKDNode* FKDNode::CheckNearestNode(FKDTransform TestTransform, FKDNode* NearestNode, TArray<FKDNode*>& SearchedNodes)
{
	SearchedNodes.Add(this); // 
	
	FKDNode* NearestNodeSoFar = NearestNode; // 현재 가장 가까운 노드
	float NearestSqrDist = NearestNodeSoFar->GetSquaredDistanceTo(TestTransform.Location); // 가장 가까운 노드 - 타겟 노드 거리
	float SqrDistToNode = GetSquaredDistanceTo(TestTransform.Location); // 현재 노드 - 타겟 노드 거리 
	
	// 현재 노드가 거리가 더 짧다면
	if (SqrDistToNode < NearestSqrDist)
	{
		NearestSqrDist = SqrDistToNode;
		
		// 자식이 없는 더이상 자식 탐색 할 수 없는 꼬리 노드라면 현재노드를 반환
		if (GetIsLeaf())
		{
			return this;
		}
		else
		{
			NearestNodeSoFar = this; // 일단 타겟과 가장 가까운 노드는 자기 자신
		}	
	}
	
	
	// (현재 깊이) 현재 노드 와 타겟 노드의 차원 (X,Y,Z) 의 거리 계산
	float DistToSplitAxis = GetKDValue() - TestTransform.GetKDValue(Dimension, Depth);  
	float sqrDistToSplitAxis = DistToSplitAxis * DistToSplitAxis;
	
	// 자식 탐색 시작
	// 현재 깊이의 차원이 타겟이 작다면 왼쪽부터 우선 탐색
	if (TestTransform.GetKDValue(Dimension, Depth) < GetKDValue())
	{
		if (GetIsLeft())
		{
			// [우선 탐색] 자식 노드 중, 타겟과 가장 가까운 노드 갱신
			NearestNodeSoFar = Left->CheckNearestNode(TestTransform, NearestNodeSoFar, SearchedNodes);
			// NearestNodeSoFar 가 변경되었을 수도 있으므로, NearestSqrDist(가장 가까운 노드 거리) 갱신
			NearestSqrDist = NearestNodeSoFar->GetSquaredDistanceTo(TestTransform.Location);
		}
		  
		// 오른쪽 노드가 존재 한다면  그리고 타겟과 현재 차원의 거리보다, NearestSqrDist 가 여전히 크다면
		// 오른쪽 노드로  검사 합니다
		if (GetIsRight() && (sqrDistToSplitAxis < NearestSqrDist))
		{
			NearestNodeSoFar = Right->CheckNearestNode(TestTransform, NearestNodeSoFar, SearchedNodes);
		}
	}
	else
	{
		// 위의 내용을 반대로 적용됨
		if (GetIsRight())
		{
			NearestNodeSoFar = Right->CheckNearestNode(TestTransform, NearestNodeSoFar, SearchedNodes);
			NearestSqrDist = NearestNodeSoFar->GetSquaredDistanceTo(TestTransform.Location);
		}
		
		if (GetIsLeft() && (sqrDistToSplitAxis < NearestSqrDist))
		{
			NearestNodeSoFar = Left->CheckNearestNode(TestTransform, NearestNodeSoFar, SearchedNodes);
		}
	}
	
	return NearestNodeSoFar;
}

float FKDNode::GetSquaredDistanceTo(const FVector& TargetLocation) const 
{
	return Transform.GetSquaredDistanceTo(TargetLocation);
}

float FKDNode::GetDistanceTo(const FVector& TargetLocation) const
{
	return Transform.GetDistanceTo(TargetLocation);
}

