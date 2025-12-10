// Fill out your copyright notice in the Description page of Project Settings.


#include "QTNode.h"
#include "QuadTree.h"
#include "Components/BoxComponent.h"

FQTBounds::FQTBounds()
{
	Center = FVector::ZeroVector;
	Extents = FVector::ZeroVector;
}

FQTBounds::FQTBounds(const FVector& InCenter, const FVector& InExtents)
{
	Center = InCenter;
	Extents = InExtents * 0.5f;
}

FQTBounds::FQTBounds(const UBoxComponent* BoxComponent)
{
	Center = BoxComponent ?  BoxComponent->Bounds.Origin : FVector::ZeroVector;
	Extents = BoxComponent ?  BoxComponent->Bounds.BoxExtent : FVector::ZeroVector;
}

bool FQTBounds::operator==(const FQTBounds& Outer) const
{
	const FVector MinA = GetMin();
	const FVector MaxA = GetMax();
	const FVector MinB = Outer.GetMin();
	const FVector MaxB = Outer.GetMax();
	
	return	(MinA.X <= MaxB.X && MaxA.X >= MinB.X) && 
			(MinA.Y <= MaxB.Y && MaxA.Y >= MinB.Y) && 
			(MinA.Z <= MaxB.Z && MaxA.Z >= MinB.Z);
}

FQTNode::FQTNode(FQuadTree* InTree, FQTNode* InParent, FQTBounds InBound, int32 InDepth)
{
	Tree = InTree;
	Parent = InParent;
	Bounds = InBound;
	Depth = InDepth;
}

FQTNode::~FQTNode()
{
	Clear();
}

void FQTNode::Insert(UBoxComponent* Item)
{
	QTNodeIndex Result = TestRegion(FQTBounds(Item));
	if (Result == QTNodeIndex::STRADDLING) // 경계선이 겹친다면, 현재 노드에 추가 
	{
		Items.Add(Item);
	}
	else if (Result != QTNodeIndex::OUTOFAREA) // 현재 노드의 범위 밖이 아니라면 (즉 에러가 아니면)
	{
		if (Split())
		{
			// 명확하게 영역안에 들어간다면
			const int32 Idx = static_cast<int32>(Result); 
			Children[Idx]->Insert(Item);
		}
		else
		{
			Items.Add(Item); // 더이상 쪼갤 수 없다면, 그냥 현재 노드에 
		}
	}
}

void FQTNode::Query(UBoxComponent* Item, TArray<FQTNode*>& PossibleNodes)
{
	PossibleNodes.Add(this);
	
	if (IsSplitted())
	{
		TArray<QTNodeIndex> Quad = GetQuads(FQTBounds(Item));
		for (QTNodeIndex Index : Quad)
		{
			Children[static_cast<int32>(Index)]->Query(Item, PossibleNodes);
		}
	}
}

void FQTNode::Clear()
{
	for (int i = 0; i < Children.Num(); ++i)
	{
		Children[i]->Clear();
		delete Children[i];
	}
	
	Items.Empty();
	Children.Empty();
}

void FQTNode::DrawBounds(const UWorld* World)
{
	FVector Extents = Bounds.Extents;
	Extents.Z = 2.f;
	DrawDebugBox(World, Bounds.Center, Extents, FColor::Black);
	if (IsSplitted())
	{
		for (int i = 0; i < Children.Num(); ++i)
		{
			Children[i]->DrawBounds(World);
		}
	}
	else
	{
		if (Items.Num() > 0)
		{
			FVector XExtent = Bounds.Extents;
			FVector YExtent = Bounds.Extents;
			FVector Center = Bounds.Center;
			
			Center.Z = 3.f;
			
			XExtent.Y = 0.f;
			XExtent.Z = 3.f;
			
			YExtent.X = 0.f;
			YExtent.Z = 3.f;

			DrawDebugLine(World, Center - XExtent, Center + XExtent, FColor::Black);
			DrawDebugLine(World, Center - YExtent, Center + YExtent, FColor::Black);
		}
	}
}

TArray<QTNodeIndex> FQTNode::GetQuads(FQTBounds InBounds)
{
	TArray<QTNodeIndex> Quad;
	
	bool NegX = InBounds.GetMin().X <= Bounds.Center.X;
	bool NegY = InBounds.GetMin().Y <= Bounds.Center.Y;
	bool PosX = InBounds.GetMax().X >= Bounds.Center.X;
	bool PosY = InBounds.GetMax().Y >= Bounds.Center.Y;
	
	if (NegX && PosY)
	{
		Quad.Add(QTNodeIndex::UPPERLEFT);
	}
	
	if (PosX && PosY)
	{
		Quad.Add(QTNodeIndex::UPPERRIGHT);
	}
	
	if (PosX && NegY)
	{
		Quad.Add(QTNodeIndex::LOWERRIGHT);
	}
	
	if (NegX && NegY)
	{
		Quad.Add(QTNodeIndex::LOWERLEFT);
	}
	
	return Quad;
}

QTNodeIndex FQTNode::TestRegion(FQTBounds InBounds)
{
	TArray<QTNodeIndex> Quad = GetQuads(InBounds);
	
	if (Quad.Num() == 0)
	{
		return QTNodeIndex::OUTOFAREA; // 범위 바깥, 에러
	}
	else if (Quad.Num() == 1) 
	{
		return Quad[0]; // 명확하게 ~분면 안에 들어가 있다면
	}
	else
	{
		return QTNodeIndex::STRADDLING; // 경계선	
	}
}

bool FQTNode::Split()
{
	if (Depth == Tree->GetMaxDepth())
	{
		return false; // 더이상 쪼갤 수 없다면
	}
	
	if (!IsSplitted())
	{
		FVector NewBoundSize = Bounds.GetSize() * 0.5f;
		
		//Children.SetNum(4); // 쿼드 (4) 개 공간 분할
		FVector CenterOffset = NewBoundSize * 0.5f; 
		
		// [-x, +y] , Upper Left;
		CenterOffset.X *= -1.0f;
		Children.Add(new FQTNode(Tree, this, FQTBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		// [+x, +y] , Upper Right;
		CenterOffset.X *= -1.0f;
		Children.Add(new FQTNode(Tree, this, FQTBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		// [-x, -y] , Lower Right;
		CenterOffset.Y *= -1.0f;
		Children.Add(new FQTNode(Tree, this, FQTBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		// [+x, +y] , Lower Left;
		CenterOffset.X *= -1.0f;
		Children.Add(new FQTNode(Tree, this, FQTBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
	}
	return true;
}



