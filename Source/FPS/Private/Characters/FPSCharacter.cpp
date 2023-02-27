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
#include "Items/Item.h"
#include "Components/WidgetComponent.h"
#include "Items/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Ammo.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FPS/FPS.h"
#include "Interfaces/BulletHitInterface.h"
#include "Enemies/Emeny.h"

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 240.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 35.f, 80.f);

	FollowCamera = CreateDefaultSubobject <UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	WeaponInterpCom = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpCom"));
	WeaponInterpCom->SetupAttachment(GetFollowCamera());
	InterpCom1 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom1"));
	InterpCom1->SetupAttachment(GetFollowCamera());
	InterpCom2 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom2"));
	InterpCom2->SetupAttachment(GetFollowCamera());
	InterpCom3 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom3"));
	InterpCom3->SetupAttachment(GetFollowCamera());
	InterpCom4 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom4"));
	InterpCom4->SetupAttachment(GetFollowCamera());
	InterpCom5 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom5"));
	InterpCom5->SetupAttachment(GetFollowCamera());
	InterpCom6 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom6"));
	InterpCom6->SetupAttachment(GetFollowCamera());
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	InitializeInterpLocations();
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);
	TraceForItems();
	InterpCapsuleHalfHeight(DeltaTime);
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

		PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &AFPSCharacter::Jump);
		PlayerInputComponent->BindAction(FName("FireButton"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FireButtonPressed);
		PlayerInputComponent->BindAction(FName("FireButton"), EInputEvent::IE_Released, this, &AFPSCharacter::FireButtonReleased);
		PlayerInputComponent->BindAction(FName("AimingButton"), EInputEvent::IE_Pressed, this, &AFPSCharacter::AimingButtonPressed);
		PlayerInputComponent->BindAction(FName("AimingButton"), EInputEvent::IE_Released, this, &AFPSCharacter::AimingButtonReleased);
		PlayerInputComponent->BindAction(FName("Select"), EInputEvent::IE_Pressed, this, &AFPSCharacter::SelectButtonPressed);
		PlayerInputComponent->BindAction(FName("Select"), EInputEvent::IE_Released, this, &AFPSCharacter::SelectButtonReleased);
		PlayerInputComponent->BindAction(FName("ReloadButton"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ReloadButtonPressed);
		PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::CrouchButtonPressed);
		PlayerInputComponent->BindAction(FName("FKey"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FKeyPressed);
		PlayerInputComponent->BindAction(FName("1Key"), EInputEvent::IE_Pressed, this, &AFPSCharacter::OneKeyPressed);
		PlayerInputComponent->BindAction(FName("2Key"), EInputEvent::IE_Pressed, this, &AFPSCharacter::TwoKeyPressed);
		PlayerInputComponent->BindAction(FName("3Key"), EInputEvent::IE_Pressed, this, &AFPSCharacter::ThreeKeyPressed);
		PlayerInputComponent->BindAction(FName("4Key"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FourKeyPressed);
		PlayerInputComponent->BindAction(FName("5Key"), EInputEvent::IE_Pressed, this, &AFPSCharacter::FiveKeyPressed);
	}
}

void AFPSCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItmes = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItmes = true;
	}
}

void AFPSCharacter::GetPickUpItem(AItem* Item)
{
	Item->PlayEquipSound();

	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < Inventory_Capacity)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else
		{
			SwapWeapon(Weapon);
		}
	}

	AAmmo* Ammo = Cast<AAmmo>(Item);
	if(Ammo)
	{
		PickUpAmmo(Ammo);
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

void AFPSCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AFPSCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		StartCrosshairBulletFire();
		EquippedWeapon->DecrementAmmo();
		StartFireTimer();

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}
	}
}

void AFPSCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void AFPSCharacter::SelectButtonReleased()
{

}

void AFPSCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AFPSCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AFPSCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AFPSCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AFPSCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AFPSCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AFPSCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AFPSCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;

	ExChangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

bool AFPSCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	FHitResult CrosshaurHitResult;
	
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshaurHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshaurHitResult.Location;
	}

	
	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation;
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd * 1.25f;

	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (!OutHitResult.bBlockingHit)
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}

	return true;
}

void AFPSCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;

	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Equipping && CombatState != ECombatState::ECS_Stunned)
	{
		Aim();
	}
}

void AFPSCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
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
	FireWeapon();
}

void AFPSCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AFPSCharacter::PlayFireSound()
{
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AFPSCharacter::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName(FName("BarrelSocket"));
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		UWorld* World = GetWorld();
		if (EquippedWeapon->GetMuzzleFlash() && World)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);
		if (bBeamEnd)
		{
			if (BeamHitResult.Actor.IsValid())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult);
				}

				AEmeny* HitEnemy = Cast<AEmeny>(BeamHitResult.Actor.Get());
				if (HitEnemy)
				{
					int32 Damage{};
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, true);
					}
					else
					{
						Damage = EquippedWeapon->GetDamage();
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());

						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, false);
					}
				}
			}
			else
			{
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, BeamHitResult.Location);
				}
			}

			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
				}
			}
		}
	}
}

void AFPSCharacter::PlayGunFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AFPSCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AFPSCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AFPSCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{WeaponInterpCom, 0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{InterpCom1, 0};
	InterpLocations.Add(InterpLoc1);
	FInterpLocation InterpLoc2{ InterpCom2, 0 };
	InterpLocations.Add(InterpLoc2);
	FInterpLocation InterpLoc3{ InterpCom3, 0 };
	InterpLocations.Add(InterpLoc3);
	FInterpLocation InterpLoc4{ InterpCom4, 0 };
	InterpLocations.Add(InterpLoc4);
	FInterpLocation InterpLoc5{ InterpCom5, 0 };
	InterpLocations.Add(InterpLoc5);
	FInterpLocation InterpLoc6{ InterpCom6, 0 };
	InterpLocations.Add(InterpLoc6);
}

void AFPSCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AFPSCharacter::AutoFireReset, EquippedWeapon->GetAutoFireRate());
}

void AFPSCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
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

bool AFPSCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
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
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.f;
		OutHitLocation = End;

		World->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
			);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AFPSCharacter::TraceForItems()
{
	if (bShouldTraceForItmes)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation = {};
		if (TraceUnderCrosshairs(ItemTraceResult, HitLocation))
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			const AWeapon* TraceHitWeapon = Cast<AWeapon>(TraceHitItem);

			if (TraceHitItem)
			{
				if (HightlightedSlot == -1)
				{
					HightlightInventorySlot();
				}
			}
			else
			{
				if (HightlightedSlot != -1)
				{
					UnHightlightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= Inventory_Capacity)
				{
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			if (TraceHitItemLastFrame && TraceHitItemLastFrame->GetPickupWidget())
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
					
			}		
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame && TraceHitItemLastFrame->GetPickupWidget())
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

AWeapon* AFPSCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);		
	}

	return nullptr;
}

void AFPSCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if(!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AFPSCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AFPSCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AFPSCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AFPSCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AFPSCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	if (EquippedWeapon == nullptr) return;

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AFPSCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;
	
	EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AFPSCharacter::PickUpAmmo(AAmmo* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		int32 AmmoCount = AmmoMap[Ammo->GetAmmoType()];
		AmmoCount += Ammo->GetItemCount();
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

FInterpLocation AFPSCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}

	return FInterpLocation();
}

int32 AFPSCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;
	
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AFPSCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AFPSCharacter::StartPickUpSoundTimer()
{
	bShouldPlayPickUpSound = false;
	GetWorldTimerManager().SetTimer(PickUpSoundTimer, this, &AFPSCharacter::ResetPickUpSoundTimer, PickUpSoundResetTime);
}

void AFPSCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AFPSCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AFPSCharacter::FinishReloading()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;

	const EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() -
			EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AFPSCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AFPSCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	int32 ClipBoneIndex{ EquippedWeapon ->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName())};
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName("hand_l"));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetMovingClip(true);
}

void AFPSCharacter::ReleaseClip()
{
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->SetMovingClip(false);
}

void AFPSCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f)};

	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };
	GetMesh() ->AddLocalOffset(MeshOffset);
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AFPSCharacter::ExChangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	const bool bCanExchangeItems = (CurrentItemIndex != NewItemIndex) &&
		(NewItemIndex < Inventory.Num()) &&
		(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping);

	if (bCanExchangeItems)
	{
		if (bAiming)
		{
			StopAiming();
		}

		AWeapon* OldEquippedWeapon = EquippedWeapon;
		AWeapon* NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}

		NewWeapon->PlayEquipSound(true);
	}
}

int32 AFPSCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}

	if (Inventory.Num() < Inventory_Capacity)
	{
		return Inventory.Num();
	}

	return -1;
}

void AFPSCharacter::HightlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HightlightIconDelegate.Broadcast(EmptySlot, true);
	HightlightedSlot = EmptySlot;
}

EPhysicalSurface AFPSCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void AFPSCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AFPSCharacter::UnHightlightInventorySlot()
{
	HightlightIconDelegate.Broadcast(HightlightedSlot, false);
	HightlightedSlot = -1;
}

float AFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AFPSCharacter::Stun()
{
	CombatState = ECombatState::ECS_Stunned;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void AFPSCharacter::ResetPickUpSoundTimer()
{
	bShouldPlayPickUpSound = true;
}

void AFPSCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AFPSCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}



