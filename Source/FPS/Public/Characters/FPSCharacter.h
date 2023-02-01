// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class FPS_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	
	//Input function
	void MoveFroward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void FireWeapon();

	//Weapon fire
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);
	void AimingButtonPressed();
	void AimingButtonReleased();
	void CameraInterpZoom(float DeltaTime);
	void SetLookRates();
	void CalculateCrosshairSpread(float DeltaTime);
	void FireButtonPressed();
	void FireButtonReleased();

	//Timer
	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();
	void StartCrosshairBulletFire();
	
	UFUNCTION()
	void FinishCrosshairBulletFire();

private:	
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	class USoundCue* FireSound;

	//Particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	UParticleSystem* BeamParticles;

	//Camera turn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float HipTurnRate = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float HipLookUpRate = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float AimingTurnRate = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	float AimingLookUpRate = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate = 0.2f;

	//Anim montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	class UAnimMontage* HipFireMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess))
	bool bAiming = false;

	//Camera FOV
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess))
	float ZoomInterpSpeed = 20.f;

	float CameraDefaultFOV = 0.f;
	float CameraZoomedFOV = 35.f;
	float CameraCurrentFOV = 0.f;

	//Crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess))
	float CrosshairSpreadMultiplier = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess))
	float CrosshairVelocityFactor = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess))
	float CrosshairInAirFactor = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess))
	float CrosshairAimFactor = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess))
	float CrosshairShootingFactor = 0.f;

	float ShootTimeDuration = 0.05f;
	bool bFiringBullet = false;
	FTimerHandle CrosshairShootTimer;

	//Auto firing
	bool bFireButtonPressed = false;
	bool bShouldFire = true;
	float AutomaticFireRate = 0.1f;
	FTimerHandle AutoFireTimer;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
};
