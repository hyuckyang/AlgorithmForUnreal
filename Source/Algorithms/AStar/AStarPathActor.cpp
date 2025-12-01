// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarPathActor.h"
#include "AStarGridActor.h"
#include "AStarNode.h"

AAStarPathActor::AAStarPathActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAStarPathActor::BeginPlay()
{
	Super::BeginPlay();
	
	GridActor = GetWorld()->SpawnActor<AAStarGridActor>(
		AAStarGridActor::StaticClass(),
		GetActorLocation(),       // 월드 위치
		GetActorRotation()       // 월드 회전
	);
	
	// GridActor = NewObject<AAStarGridActor>(this);
	// GridActor->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	// GridActor->SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	
}

void AAStarPathActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GridActor && StartActor && EndActor)
	{
		FindPath(StartActor->GetActorLocation(), EndActor->GetActorLocation());
	}
}

void AAStarPathActor::FindPath(const FVector& StartPos, const FVector& EndPos) const
{
	//AAStarGridActor* GridActor = nullptr; // 일단 있다 치고
	
	//
	FAStarNode* StartNode = GridActor->GetNode(StartPos);;
	FAStarNode* TargetNode = GridActor->GetNode(EndPos);
	
	TArray<FAStarNode*> OpenSet; //
	TArray<FAStarNode*> CloseSet;
	
	OpenSet.Add(StartNode);
	while (OpenSet.Num() > 0)
	{
#pragma region 가장 낮은 값을 가진 노드 선택
		FAStarNode* CurrentNode = OpenSet[0];
		for (int i = 1; i < OpenSet.Num(); i++)
		{
			// F Cost 가 가장 낮거나, F Cost 가 같다면 H(휴리스릭) Cost 가 낮은 것을 선택 
			if (OpenSet[i]->GetFCost() < CurrentNode->GetFCost() || (OpenSet[i]->GetFCost() == CurrentNode->GetFCost() && OpenSet[i]->HCost < CurrentNode->HCost))
			{
				CurrentNode = OpenSet[i];
			}
		}
#pragma endregion
		
#pragma region 가장 낮은 값을 가진 노드가 종착노드면, 탐색 종료
		if (CurrentNode == TargetNode)
		{
			RetracePath(StartNode, TargetNode);
			return;
		}
#pragma endregion
		
#pragma region 선택된 노드를 OpenSet 에서 CloseSet 으로 이동
		OpenSet.Remove(CurrentNode);
		CloseSet.Add(CurrentNode);
#pragma endregion
		
#pragma region 이웃노드를 가져와 값을 계산 한 후 오픈 셋에 추가
		for (FAStarNode* N : GridActor->GetNeighbor(CurrentNode))
		{
			// 이동할수 없는 노드 혹은 이미 Close (노드 파악 완료) 에 들어갔다면
			if (!N->IsWalk || CloseSet.Contains(N))
				continue;
			
			int32 G = CurrentNode->GCost + GetDistance(CurrentNode->GridX, CurrentNode->GridY, N->GridX, N->GridY);
			int32 H = GetDistance(N->GridX, N->GridY, TargetNode->GridX, TargetNode->GridY);
			int32 F = G + H;
			
			// 이웃노드 중, 오픈셋에 이미 존재한다면, 값이 작은 변경합니다
			if (OpenSet.Contains(N))
			{
				if (N->GetFCost() > F)
				{
					N->GCost = G;
					N->ParentX = CurrentNode->GridX;
					N->ParentY = CurrentNode->GridY;
				}
			}
			else
			{
				N->GCost = G;
				N->HCost = H;
				N->ParentX = CurrentNode->GridX;
				N->ParentY = CurrentNode->GridY;
				
				OpenSet.Add(N);
			}
		}
#pragma endregion
	}
}

void AAStarPathActor::RetracePath(const FAStarNode* StartNode, const FAStarNode* EndNode) const
{
	//AAStarGridActor* GridActor = nullptr; // 일단 있다 치고
	TArray<const FAStarNode*> Trace;
	
	const FAStarNode* CurrentNode = EndNode;
	while (CurrentNode != StartNode)
	{
		Trace.Add(CurrentNode);
		CurrentNode = GridActor->GetNode(CurrentNode->ParentX, CurrentNode->ParentY);
	}
	
	Algo::Reverse(Trace);
	
	GridActor->SetPathNodes(Trace);
}

int32 AAStarPathActor::GetDistance(int32 NodeAGridX, int32 NodeAGridY, int32 NodeBGridX, int32 NodeBGridY)
{
	int32 DistX = FMath::Abs(NodeAGridX - NodeBGridX);
	int32 DistY = FMath::Abs(NodeAGridY - NodeBGridY);
	
	if(DistX > DistY)
	{
		return 14 * DistY + 10 * (DistX - DistY);
	}
	return 14 * DistX + 10 * (DistY - DistX);
}
