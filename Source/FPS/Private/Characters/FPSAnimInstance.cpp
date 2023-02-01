// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FPSAnimInstance.h"
#include "Characters/FPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFPSAnimInstance::NativeInitializeAnimation()
{
	FPSCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
	
}

void UFPSAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (FPSCharacter == nullptr)
	{
		FPSCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
	}
	if (FPSCharacter)
	{
		FVector Velocity{ FPSCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		bIsInAir = FPSCharacter->GetCharacterMovement()->IsFalling();

		if (FPSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = FPSCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(FPSCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (FPSCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = FPSCharacter->GetAiming();
	}
}
