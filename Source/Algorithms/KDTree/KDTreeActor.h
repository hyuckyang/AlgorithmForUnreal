// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KDNode.h"
#include "KDTree.h"
#include "KDTreeActor.generated.h"

class UBoxComponent;

UCLASS()
class ALGORITHMS_API AKDTreeActor : public AActor
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(EditAnywhere, Category="KDTree", meta=(AllowPrivateAccess=true))
	int32 Dimension = 2; // 차원
	
public:
	// 테스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="KDTree")
	FVector TotalArea;
	
private:
	UPROPERTY(EditAnywhere, Category="KDTree", meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<AActor>> ObstacleActors;
	
	UPROPERTY(EditAnywhere, Category="KDTree", meta=(AllowPrivateAccess=true))
	TObjectPtr<AActor> MovableActor; // 질의 (범위) 액터
	
	TWeakObjectPtr<UBoxComponent> MovableBox;
	TArray<FKDNode*> SearchKDNodes; // 위험
	FKDTransform NearestTransform;
	bool bIsNearest;
	TUniquePtr<FKDTree> Tree;
	
	FKDBounds Bounds;
public:	
	AKDTreeActor();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	
private:
	void Insert();
	void Query();
	void Draw() const;
};
