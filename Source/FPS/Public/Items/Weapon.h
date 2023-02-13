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

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

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

public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection;}
	FORCEINLINE int32 GetMagazineCapacity() const {return MagazineCapacity;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;}
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
};
