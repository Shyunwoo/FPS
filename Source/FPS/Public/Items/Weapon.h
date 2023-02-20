// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Characters/AmmoType.h"
#include "Characters/WeaponType.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazingCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotDamage;
};

/**
 * 
 */
UCLASS()
class FPS_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	void ThrowWeapon();
	void DecrementAmmo();
	void ReloadAmmo(int32 Amount);
	bool ClipIsFull();
	void StartSlideTimer();

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void FinishMovingSlide();
	void UpdateSlideDisplacement();

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess));
	bool bMovingClip = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	int32 MagazineCapacity = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	EWeaponType WeaponType = EWeaponType::EWT_SubmachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	FName ReloadMontageSection = FName(TEXT("ReloadSMG"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	FName ClipBoneName = FName(TEXT("smg_clip"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UTexture2D* CrosshairsTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UTexture2D* CrosshairsBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UTexture2D* CrosshairsLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UTexture2D* CrosshairsRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	float AutoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	USoundCue* FireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	FName BoneToHide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	float SlideDisplacement = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	float RecoilRotation = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	UCurveFloat* SlideDisplacementCurve;

	FTimerHandle SlideTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	float SlideDisplacementTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	float MaxSlideDisplacement = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	float MaxRecoilRotation = 20.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess))
	bool bMovingSlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess))
	float HeadShotDamage = 50.f;

public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection;}
	FORCEINLINE int32 GetMagazineCapacity() const {return MagazineCapacity;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;}
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
	FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }
	FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }
	FORCEINLINE float GetAutoFireRate() const {return AutoFireRate;}
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }
	FORCEINLINE bool GetAutomatic() const { return bAutomatic; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
};
