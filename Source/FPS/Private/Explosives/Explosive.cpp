// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives/Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, HitResult.Location, FRotator(0.f), true);
	}



	Destroy();
}

