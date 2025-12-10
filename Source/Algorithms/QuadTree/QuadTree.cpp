// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree.h"
#include "QTNode.h"

FQuadTree::FQuadTree(FVector Position, FVector Size)
{
	RootNode = MakeUnique<FQTNode>(this, nullptr, FQTBounds(Position, Size), 0);
}

FQuadTree::~FQuadTree()
{
	RootNode->Clear();
}

void FQuadTree::Insert(UBoxComponent* ItemBox)
{
	RootNode->Insert(ItemBox);
}

void FQuadTree::Query(UBoxComponent* QueryItem, TArray<TWeakObjectPtr<UBoxComponent>>& Intersect, TArray<FQTNode*>& Possible)
{
	RootNode->Query(QueryItem, Possible);
	
	FQTBounds Query(QueryItem);
	for (auto Node : Possible)
	{
		for (TWeakObjectPtr<UBoxComponent> BoxComp : Node->Items)
		{
			FQTBounds BoxBounds(BoxComp.Get());
			if (BoxBounds == Query)
			{
				// 실제 충돌(교차)이 검출된 객체
				Intersect.Add(BoxComp);
			}	
		}
	}
}

void FQuadTree::DrawBounds(const UWorld* World)
{
	RootNode->DrawBounds(World);
}

