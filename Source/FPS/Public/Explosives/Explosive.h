// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/BulletHitInterface.h"
#include "Explosive.generated.h"

UCLASS()
class FPS_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	AExplosive();

	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	class UParticleSystem* ExplodeParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	class USoundCue* ImpactSound;

};
