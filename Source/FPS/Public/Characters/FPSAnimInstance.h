// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "FPSAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class FPS_API UFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

protected:
	void TurnInPlace();
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	class AFPSCharacter* FPSCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float Speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	bool bIsAccelerating = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float MovementOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess))
	bool bAiming = false;

	float TIPCharacterYaw = 0.f;
	float TIPCharacterYawLastFrame = 0.f;
	FRotator CharacterRotation = FRotator(0.f);
	FRotator CharacterRotationLastFrame = FRotator(0.f);
	float RotationCurve = 0.f;
	float RotationCurveLastFrame = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Turn, meta = (AllowPrivateAccess))
	float RootYawOffset = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess))
	float YawDelta = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Turn, meta = (AllowPrivateAccess))
	float Pitch = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Turn, meta = (AllowPrivateAccess))
	bool bReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess))
	bool bCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess))
	bool bEquipping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouch, meta = (AllowPrivateAccess))
	bool bTurningInPlace = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	float RecoilWeight = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Turn, meta = (AllowPrivateAccess))
	EOffsetState OffsetState = EOffsetState::EOS_Hip;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	EWeaponType EquippedWeaponType = EWeaponType::EWT_MAX;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	bool bShouldUseFABRIK = false;
};
