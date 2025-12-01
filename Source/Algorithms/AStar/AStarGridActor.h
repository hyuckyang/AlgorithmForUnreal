// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNode.h"
#include "AStarGridActor.generated.h"

class AStarGrid
{

private:
	
	const TObjectPtr<AActor> GridActor;
	TArray<FAStarNode> GridNode;
	int32 NumOfGridsX, NumOfGridsY;
	int32 GridLenX, GridLenY;
	float NodeSize;
	
public:
	//AStarGrid() : NumOfGridsX(0), NumOfGridsY(0), GridSizeX(0), GridSizeY(0) {}; 
	AStarGrid(AActor* InGridActor, const float InNodeSize, const int32 InNumOfGridX, const int32 InNumOfGridY);

	void CreateGrid();
	void DrawGrid(const TArray<const FAStarNode*>& Path);
	FAStarNode* GetNode(const FVector& Position);
	FAStarNode* GetNode(const int32 GridX, const int32 GridY);
	TArray<FAStarNode*> GetNeighbor(const FAStarNode* Node); // 주변 노드를 검색 넘겨 줍니다.
	FORCEINLINE int32 GetGridIndex(int32 X, int32 Y) const;
	
};


UCLASS()
class ALGORITHMS_API AAStarGridActor : public AActor
{
	GENERATED_BODY()

private:
	
	TUniquePtr<AStarGrid> Grid;
	TArray<const FAStarNode*> PathNodes;
	
public:	
	AAStarGridActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE FAStarNode* GetNode(const FVector& Position) const;
	FORCEINLINE FAStarNode* GetNode(const int32 GridX, const int32 GridY) const;
	
	FORCEINLINE TArray<FAStarNode*> GetNeighbor(const FAStarNode* Node) const;
	
	void SetPathNodes(const TArray<const FAStarNode*>& Paths);

};
