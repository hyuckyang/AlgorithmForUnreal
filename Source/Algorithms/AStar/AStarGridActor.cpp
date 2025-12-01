// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGridActor.h"


AStarGrid::AStarGrid(AActor* InGridActor, const float InNodeSize, const int32 InNumOfGridX, const int32 InNumOfGridY) : GridActor(InGridActor), NumOfGridsX(InNumOfGridX), NumOfGridsY(InNumOfGridY), NodeSize(InNodeSize) 
{
	GridLenX = FMath::RoundToInt32(NumOfGridsX / NodeSize);
	GridLenY = FMath::RoundToInt32(NumOfGridsY / NodeSize);
}

void AStarGrid::CreateGrid()
{
	GridNode.SetNum(GridLenX * GridLenY);
	
	const FVector RightVector = FVector::RightVector;
	const FVector ForwardVector = FVector::ForwardVector;
	
	const FVector WorldPosition = GridActor->GetActorLocation();
	const FVector BottomLeft = WorldPosition - ForwardVector * NumOfGridsX * 0.5f - RightVector * NumOfGridsY * 0.5f;

	const float NodeHalfSize = NodeSize * 0.5f;
	for (int32 x = 0; x < GridLenX; x++)
	{
		for (int y = 0; y < GridLenY; ++y)
		{
			FVector NodePos = BottomLeft + ForwardVector * (x * NodeSize + NodeHalfSize) + RightVector * (y * NodeSize + NodeHalfSize);
			bool bBlock = GridActor->GetWorld()->OverlapAnyTestByObjectType(NodePos, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(NodeHalfSize));
			GridNode[GetGridIndex(x, y)] = FAStarNode(!bBlock, NodePos, x, y);
		}
	}
}

void AStarGrid::DrawGrid(const TArray<const FAStarNode*>& Path)
{
	const UWorld* World = GridActor->GetWorld();
	const FVector WorldPosition = GridActor->GetActorLocation();
	const FVector BoxExtent (NumOfGridsX * 0.5f, NumOfGridsX * 0.5f, 1.F);
	DrawDebugBox(World, WorldPosition, BoxExtent, FColor::White);
	
	const FVector NodeBoxExtent (NodeSize * 0.5f, NodeSize * 0.5f, 1.F); 
	
	for (int32 i = 0; i < GridNode.Num(); ++i)
	{
		const FAStarNode* Node = &GridNode[i];
		if (Node->IsWalk)
		{
			if (Path.Num() > 0 && Path.Contains(Node))
			{
				
				DrawDebugCapsule(World, Node->Position, 0.5f, NodeSize * 0.4f, FQuat::Identity,  FColor::Black);
			}
		}
		else
		{
			DrawDebugCapsule(World, Node->Position, 0.5f, NodeSize * 0.4f, FQuat::Identity,  FColor::Red);
		}
		
		DrawDebugBox(World, Node->Position, NodeBoxExtent, FColor::White);
	}
}

FAStarNode* AStarGrid::GetNode(const FVector& Position)
{
	float PercentX = (Position.X + NumOfGridsX * 0.5f) / NumOfGridsX;
	float PercentY = (Position.Y + NumOfGridsY * 0.5f) / NumOfGridsY;
	PercentX = FMath::Clamp(PercentX, 0.F, 1.f);
	PercentY = FMath::Clamp(PercentY, 0.F, 1.f);
	
	const int32 X = FMath::RoundToInt((GridLenX -1) * PercentX);
	const int32 Y = FMath::RoundToInt((GridLenY -1) * PercentY);
	
	return &GridNode[GetGridIndex(X, Y)];
}

FAStarNode* AStarGrid::GetNode(const int32 GridX, const int32 GridY)
{
	return &GridNode[GetGridIndex(GridX, GridY)];
}


TArray<FAStarNode*> AStarGrid::GetNeighbor(const FAStarNode* Node)
{
	TArray<FAStarNode*> Neighbor;

	for (int X = -1; X <= 1; ++X)
	{
		for (int Y = -1; Y <= 1; ++Y)
		{
			// 자기 자신
			if (X == 0 && Y == 0)
				continue;
			
			int32 CheckX = Node->GridX + X;
			int32 CheckY = Node->GridY + Y;
			
			if (CheckX >= 0 && CheckX < GridLenX && CheckY >= 0 && CheckY < GridLenY)
			{
				Neighbor.Add(&GridNode[GetGridIndex(CheckX, CheckY)]);
			}
		}
	}
	
	return Neighbor;
}

int32 AStarGrid::GetGridIndex(int32 X, int32 Y) const
{
	return Y * GridLenX + X;
}

AAStarGridActor::AAStarGridActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAStarGridActor::BeginPlay()
{
	Super::BeginPlay();
	
	Grid = MakeUnique<AStarGrid>(this, 100, 3000, 3000);
	Grid->CreateGrid();
	
}

void AAStarGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Grid)
		Grid->DrawGrid(PathNodes);
}

FAStarNode* AAStarGridActor::GetNode(const FVector& Position) const
{
	return Grid != nullptr ? Grid->GetNode(Position) : nullptr;
}

FAStarNode* AAStarGridActor::GetNode(const int32 GridX, const int32 GridY) const
{
	return Grid != nullptr ? Grid->GetNode(GridX, GridY) : nullptr;
}

TArray<FAStarNode*> AAStarGridActor::GetNeighbor(const FAStarNode* Node) const
{
	return Grid != nullptr ? Grid->GetNeighbor(Node) : TArray<FAStarNode*>();
}

void AAStarGridActor::SetPathNodes(const TArray<const FAStarNode*>& Paths)
{
	PathNodes = Paths;
}

