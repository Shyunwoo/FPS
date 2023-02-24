// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/GruxAnimInstance.h"
#include "Enemies/Emeny.h"

void UGruxAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEmeny>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
