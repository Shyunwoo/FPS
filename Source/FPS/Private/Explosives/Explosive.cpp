// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives/Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>("ExplosiveMesh");
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, HitResult.Location, FRotator(0.f), true);
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(Actor, Damage, ShooterController, Shooter, UDamageType::StaticClass());
	}

	Destroy();
}

