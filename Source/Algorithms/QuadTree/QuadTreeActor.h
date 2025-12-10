// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QTNode.h"
#include "QuadTree.h"
#include "QuadTreeActor.generated.h"

// class FQTNode;
// class FQuadTree;
class UBoxComponent;

// UENUM()
// enum class QuadDrawType : uint8
// {
// 	Insert, // 삽입
// 	Query // 질의 (충돌 여부)
// };

UCLASS()
class ALGORITHMS_API AQuadTreeActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	// UPROPERTY(EditAnywhere)
	// QuadDrawType DrawType;
	
	UPROPERTY(EditAnywhere)
	FVector2D TotalArea;
	
	UPROPERTY(EditInstanceOnly, Category="Quad Tree")
	TArray<TObjectPtr<AActor>> ObstacleActors;
	
	// UPROPERTY(EditInstanceOnly, Category="Quad Tree")
	// TObjectPtr<AActor> InsertActor; // 노드에 넣을 Actor
	
	UPROPERTY(EditInstanceOnly, Category="Quad Tree")
	TObjectPtr<AActor> MovableActor; // 질의 (범위) 액터
	
private:
	
	TUniquePtr<FQuadTree> Tree;
	TArray<FQTNode*> PossibleNodes; // 질의(충돌) 을 시도한 모든 노드 가져옴
	TArray<TWeakObjectPtr<UBoxComponent>> IntersectBounds;
	
	TArray<TObjectPtr<UBoxComponent>> ObstacleBoxes;
	//TWeakObjectPtr<UBoxComponent> InsertBox;
	TWeakObjectPtr<UBoxComponent> MovableBox;
	
public:	
	AQuadTreeActor();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	
	void Insert();
	void Query();
	
protected:
	void DrawBounds();

};
