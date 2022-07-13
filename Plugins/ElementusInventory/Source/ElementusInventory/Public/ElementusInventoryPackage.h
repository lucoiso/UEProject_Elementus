// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementusInventoryData.h"
#include "ElementusInventoryPackage.generated.h"

class UNiagaraComponent;
class UElementusInventoryComponent;

UCLASS(Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORY_API AElementusInventoryPackage final : public AActor
{
	GENERATED_BODY()

public:
	AElementusInventoryPackage();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TObjectPtr<UStaticMeshComponent> PackageMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Elementus Inventory")
	TObjectPtr<UElementusInventoryComponent> PackageInventory;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void PutItemIntoPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
	                        UElementusInventoryComponent* FromInventory) const;

	UFUNCTION(BlueprintCallable, Category = "Elementus Inventory")
	void GetItemFromPackage(TMap<FPrimaryAssetId, int32>& ItemInfo,
	                        UElementusInventoryComponent* ToInventory) const;
};
