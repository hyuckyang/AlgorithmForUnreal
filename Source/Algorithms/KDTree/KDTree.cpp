// Fill out your copyright notice in the Description page of Project Settings.


#include "KDTree.h"
#include "KDNode.h"

FKDTree::FKDTree(int32 InDimension)
{
	Dimension = InDimension;
}

void FKDTree::Insert(FKDTransform NewTransform)
{
	int32 Depth = 0;
	
	if (RootNode == nullptr)
	{
		RootNode = MakeUnique<FKDNode>(Dimension, nullptr, NewTransform, Depth);
		return;
	}
	
	FKDNode* Target = RootNode.Get();
	while (Target != nullptr)
	{
		if (NewTransform == Target->GetTransform())
		{
			// 같은 포지션(값)을 가진 노드는 추가(Insert) 할 수 없습니다.
			break;
		}
		
		// Parent 은 일단 null -> 사용하는 곳이 없음
		// 새로운 포지션의 차원(X,Y,Z) 중 현재 깊이에 따른 값과 타겟의 (해당 노드의 기준 차원 기반)값을 비교
		if (NewTransform.GetKDValue(Dimension, Depth) < Target->GetKDValue())
		{
			// 값이 작고, (좌/우 혹은 상/하, 개념적으로)왼쪽이 비워져있다면
			if (!Target->GetIsLeft())
			{
				Target->SetLeft(new FKDNode(Dimension, nullptr, NewTransform, Depth + 1));
				break;
			}
			else
			{
				Target = Target->GetLeft();
			}
		}
		else
		{
			// 크다면 오른쪽 ( 혹은 상-하 등 )
			if (!Target->GetIsRight())
			{
				Target->SetRight(new FKDNode(Dimension, nullptr, NewTransform, Depth + 1));
				break;
			}
			else
			{
				Target = Target->GetRight();
			}
		}
		Depth++;
	}
}

bool FKDTree::Query(const FKDTransform& QueryTransform, FKDTransform& ResultTransform, TArray<FKDNode*>& SearchNodes) const
{
	FKDNode* Result = RootNode->CheckNearestNode(QueryTransform, RootNode.Get(), SearchNodes);
	ResultTransform = Result  ? Result->GetTransform() : FKDTransform();
	
	return Result != nullptr;;
}

void FKDTree::Draw(const UWorld* World, const FKDBounds& Bounds) const
{
	RootNode->Draw(World, Bounds);
}
