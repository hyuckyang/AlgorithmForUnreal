// Fill out your copyright notice in the Description page of Project Settings.


#include "OcNode.h"

#include "Octree.h"
#include "Components/BoxComponent.h"

FOcBounds::FOcBounds()
{
	Center = FVector::ZeroVector;
	Extents = FVector::ZeroVector;
}

FOcBounds::FOcBounds(const FVector& InCenter, const FVector& InExtents)
{
	Center = InCenter;
	Extents = InExtents * 0.5f;
}

FOcBounds::FOcBounds(const UBoxComponent* BoxComponent)
{
	Center = BoxComponent ?  BoxComponent->Bounds.Origin : FVector::ZeroVector;
	Extents = BoxComponent ?  BoxComponent->Bounds.BoxExtent : FVector::ZeroVector;
}

bool FOcBounds::operator==(const FOcBounds& Outer) const
{
	const FVector MinA = GetMin();
	const FVector MaxA = GetMax();
	const FVector MinB = Outer.GetMin();
	const FVector MaxB = Outer.GetMax();
	
	return	(MinA.X <= MaxB.X && MaxA.X >= MinB.X) && 
			(MinA.Y <= MaxB.Y && MaxA.Y >= MinB.Y) && 
			(MinA.Z <= MaxB.Z && MaxA.Z >= MinB.Z);
}

FOcNode::FOcNode(FOctree* InTree, FOcNode* InParent, FOcBounds InBound, int32 InDepth)
{
	Tree = InTree;
	Parent = InParent;
	Bounds = InBound;
	Depth = InDepth;
}

FOcNode::~FOcNode()
{
	Clear();
}

void FOcNode::Insert(UBoxComponent* Item)
{
	OcNodeIndex Result = TestRegion(FOcBounds(Item));
	if (Result == OcNodeIndex::STRADDLING) // 경계선이 겹친다면, 현재 노드에 추가 
	{
		Items.Add(Item);
	}
	else if (Result != OcNodeIndex::OUTOFAREA) // 현재 노드의 범위 밖이 아니라면 (즉 에러가 아니면)
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

void FOcNode::Query(UBoxComponent* Item, TArray<FOcNode*>& PossibleNodes)
{
	PossibleNodes.Add(this);
	
	if (IsSplitted())
	{
		TArray<OcNodeIndex> Quad = GetOctants(FOcBounds(Item));
		for (OcNodeIndex Index : Quad)
		{
			Children[static_cast<int32>(Index)]->Query(Item, PossibleNodes);
		}
	}
}

void FOcNode::Clear()
{
	for (int i = 0; i < Children.Num(); ++i)
	{
		Children[i]->Clear();
		delete Children[i];
	}
	
	Items.Empty();
	Children.Empty();
}

void FOcNode::DrawBounds(const UWorld* World)
{
	FVector Extents = Bounds.Extents;
	//Extents.Z = 2.f;
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
			FVector ZExtent = Bounds.Extents;
			FVector Center = Bounds.Center;
			XExtent.Y = 0;
			XExtent.Z = 0;
			YExtent.X = 0;
			YExtent.Z = 0;
			ZExtent.X = 0;
			ZExtent.Y = 0;
			DrawDebugLine(World, Center - XExtent, Center + XExtent, FColor::Black);
			DrawDebugLine(World, Center - YExtent, Center + YExtent, FColor::Black);
			DrawDebugLine(World, Center - ZExtent, Center + ZExtent, FColor::Black);
		}
	}
}

TArray<OcNodeIndex> FOcNode::GetOctants(FOcBounds InBounds)
{
	TArray<OcNodeIndex> Octants;
	
	bool NegX = InBounds.GetMin().X <= Bounds.Center.X;
	bool NegY = InBounds.GetMin().Y <= Bounds.Center.Y;
	bool NegZ = InBounds.GetMin().Z <= Bounds.Center.Z;
	
	bool PosX = InBounds.GetMax().X >= Bounds.Center.X;
	bool PosY = InBounds.GetMax().Y >= Bounds.Center.Y;
	bool PosZ = InBounds.GetMax().Z >= Bounds.Center.Z;
	
	if (PosZ && NegX && PosY)
	{
		Octants.Add(OcNodeIndex::TOPUPPERLEFT);
	}

	if (PosZ && PosX && PosY)
	{
		Octants.Add(OcNodeIndex::TOPUPPERRIGHT);
	}

	if (PosZ && PosX && NegY)
	{
		Octants.Add(OcNodeIndex::TOPLOWERRIGHT);
	}

	if (PosZ && NegX && NegY)
	{
		Octants.Add(OcNodeIndex::TOPLOWERLEFT);
	}

	if (NegZ && NegX && PosY)
	{
		Octants.Add(OcNodeIndex::BOTTOMUPPERLEFT);
	}

	if (NegZ && PosX && PosY)
	{
		Octants.Add(OcNodeIndex::BOTTOMUPPERRIGHT);
	}

	if (NegZ && PosX && NegY)
	{
		Octants.Add(OcNodeIndex::BOTTOMLOWERRIGHT);
	}

	if (NegZ && NegX && NegY)
	{
		Octants.Add(OcNodeIndex::BOTTOMLOWERLEFT);
	}

	return Octants;
}

OcNodeIndex FOcNode::TestRegion(FOcBounds InBounds)
{
	TArray<OcNodeIndex> Octants = GetOctants(InBounds);
	
	if (Octants.Num() == 0)
	{
		return OcNodeIndex::OUTOFAREA; // 범위 바깥, 에러
	}
	else if (Octants.Num() == 1) 
	{
		return Octants[0]; // 명확하게 ~분면 안에 들어가 있다면
	}
	else
	{
		return OcNodeIndex::STRADDLING; // 경계선	
	}
}

bool FOcNode::Split()
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
		
		//[-x, +y, +z] // TOPUPPERLEFT
		CenterOffset.X *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[+x, +y, +z] // TOPUPPERRIGHT
		CenterOffset.X *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[-x, -y, +z] // TOPLOWERRIGHT
		CenterOffset.Y *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[+x, -y, +z] // TOPLOWERLEFT
		CenterOffset.X *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[-x, +y, -z] // BOTTOMUPPERLEFT
		CenterOffset.Z *= -1.0f;
		CenterOffset.Y *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[+x, +y, -z] // BOTTOMUPPERRIGHT
		CenterOffset.X *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[-x, -y, -z] // BOTTOMLOWERRIGHT
		CenterOffset.Y *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
		
		//[+x, -y, -z] // BOTTOMLOWERLEFT
		CenterOffset.X *= -1.0f;
		Children.Add(new FOcNode(Tree, this, FOcBounds(Bounds.Center + CenterOffset, NewBoundSize), Depth + 1));
	}
	return true;
}
