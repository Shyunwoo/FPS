// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject <UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AFPSCharacter::MoveFroward);
		PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AFPSCharacter::MoveRight);
		PlayerInputComponent->BindAxis(FName("Turn"), this, &AFPSCharacter::Turn);
		PlayerInputComponent->BindAxis(FName("LookUp"), this, &AFPSCharacter::LookUp);
		PlayerInputComponent->BindAxis(FName("TurnRate"), this, &AFPSCharacter::TurnAtRate);
		PlayerInputComponent->BindAxis(FName("LookUpRate"), this, &AFPSCharacter::LookUpAtRate);

		PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction(FName("FireButton"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
		PlayerInputComponent->BindAction(FName("FireButton"), EInputEvent::IE_Released, this, &AFPSCharacter::FireButtonReleased);
		PlayerInputComponent->BindAction(FName("AimingButton"), EInputEvent::IE_Pressed, this, &AFPSCharacter::AimingButtonPressed);
		PlayerInputComponent->BindAction(FName("AimingButton"), EInputEvent::IE_Released, this, &AFPSCharacter::AimingButtonReleased);
	}
}

void AFPSCharacter::MoveFroward(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{0.f, Rotation.Yaw, 0.f};
		const FVector Direction{ FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction, Value);
	}
}

void AFPSCharacter::MoveRight(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AFPSCharacter::Turn(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		float Scale = 0.f;
		if (bAiming)
		{
			Scale = MouseAimingTurnRate;
		}
		else
		{
			Scale = MouseHipTurnRate;
		}
		AddControllerYawInput(Value * Scale);
	}
}

void AFPSCharacter::LookUp(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		float Scale = 0.f;
		if (bAiming)
		{
			Scale = MouseAimingLookUpRate;
		}
		else
		{
			Scale = MouseHipLookUpRate;
		}
		AddControllerPitchInput(Value * Scale);
	}
}

void AFPSCharacter::TurnAtRate(float Rate)
{
	if ((Controller) && (Rate != 0.f))
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AFPSCharacter::LookUpAtRate(float Rate)
{
	if ((Controller) && (Rate != 0.f))
	{
		AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AFPSCharacter::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(FName("BarrelSocket"));
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		UWorld* World = GetWorld();
		if (MuzzleFlash && World)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, BeamEnd);
			}

			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}
		}
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

	StartCrosshairBulletFire();
}

bool AFPSCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	UWorld* World = GetWorld();
	if (bScreenToWorld && World)
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.f;

		OutBeamLocation = End;
		World->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (ScreenTraceHit.bBlockingHit)
		{
			OutBeamLocation = ScreenTraceHit.Location;
		}

		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart = MuzzleSocketLocation;
		const FVector WeaponTraceEnd = OutBeamLocation;

		World->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}

	return false;
}

void AFPSCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AFPSCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AFPSCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AFPSCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AFPSCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AFPSCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AFPSCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AFPSCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AFPSCharacter::AutoFireReset, AutomaticFireRate);
	}
}

void AFPSCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

void AFPSCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AFPSCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AFPSCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

float AFPSCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}



