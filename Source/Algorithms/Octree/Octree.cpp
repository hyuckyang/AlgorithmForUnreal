// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree.h"
#include "OcNode.h"

FOctree::FOctree(FVector Position, FVector Size)
{
	RootNode = MakeUnique<FOcNode>(this, nullptr, FOcBounds(Position, Size), 0);
}

FOctree::~FOctree()
{
	RootNode->Clear();
}

void FOctree::Insert(UBoxComponent* ItemBox)
{
	RootNode->Insert(ItemBox);
}

void FOctree::Query(UBoxComponent* QueryItem, TArray<TWeakObjectPtr<UBoxComponent>>& Intersect, TArray<FOcNode*>& Possible)
{
	RootNode->Query(QueryItem, Possible);
	
	FOcBounds Query(QueryItem);
	for (auto Node : Possible)
	{
		for (TWeakObjectPtr<UBoxComponent> BoxComp : Node->Items)
		{
			FOcBounds BoxBounds(BoxComp.Get());
			if (BoxBounds == Query)
			{
				// 실제 충돌(교차)이 검출된 객체
				Intersect.Add(BoxComp);
			}	
		}
	}
}

void FOctree::DrawBounds(const UWorld* World)
{
	RootNode->DrawBounds(World);
}
