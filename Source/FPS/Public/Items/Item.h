// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Lagendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
};

UCLASS()
class FPS_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();
	virtual void Tick(float DeltaTime) override;

	void SetItemState(EItemState State);
	void StartItemCurve(class AFPSCharacter* Char, bool bForcePlaySound = false);
	void PlayEquipSound(bool bForcePlaySound = false);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	void DisableGlowMaterial();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetActiveStars();
	virtual void SetItemProperties(EItemState State);
	
	void FinishInterping();
	void ItemInterp(float DeltaTime);
	FVector GetInterpLocation();
	void PlayPickUpSound(bool bForcePlaySound = false);

	virtual void InitializeCustomDepth();
	virtual void OnConstruction(const FTransform& Transform) override;
	void EnableGlowMaterial();
	void UpdatePulse();
	void ResetPulseTimer();
	void StartPulseTimer();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	class UWidgetComponent* PickupWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	FString ItemName{FString("Default")};

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Item, meta = (AllowPrivateAccess))
	int32 ItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	EItemRarity ItemRarity = EItemRarity::EIR_Damaged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	EItemState ItemState = EItemState::EIS_PickUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	EItemType ItemType = EItemType::EIT_MAX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	TArray<bool> ActiveStars;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	class UCurveFloat* ItemZCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	UCurveFloat* ItemScaleCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	FVector ItemInterpStartLocation {FVector(0.f)};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	FVector CameraTargetLocation {FVector(0.f)};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	bool bInterping = false;

	FTimerHandle ItemInterpTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	float ZCurveTime = 0.7f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	AFPSCharacter* Character;

	float ItemInterpX = 0.f;
	float ItemInterpY = 0.f;
	float interpInitialYawOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess))
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess))
	USoundCue* EquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	int32 InterpLocIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess))
	int32 MaterialIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess))
	UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	class UCurveVector* PulseCurve;

	FTimerHandle PulseTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	float PulseCurveTime = 5.f;

	UPROPERTY(VisibleAnywhere, Category = Item, meta = (AllowPrivateAccess))
	float GlowAmount = 100.f;

	UPROPERTY(VisibleAnywhere, Category = Item, meta = (AllowPrivateAccess))
	float FresnelExponent = 3.f;

	UPROPERTY(VisibleAnywhere, Category = Item, meta = (AllowPrivateAccess))
	float FresnelReflectFraction = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	UCurveVector* InterpPulseCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess))
	UTexture2D* IconItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess))
	UTexture2D* AmmoItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	int32 SlotIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess))
	bool bCharacterInventoryFull = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess))
	class UDataTable* ItemRarityDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	FLinearColor GlowColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	FLinearColor LightColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	FLinearColor DarkColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	int32 NumberOfStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess))
	UTexture2D* IconBackGround;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickUpSound() const {return PickUpSound;}
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetCharacter(AFPSCharacter* Char) { Character = Char; }
	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }
};
