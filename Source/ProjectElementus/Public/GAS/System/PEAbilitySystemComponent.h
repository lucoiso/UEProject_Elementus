// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PEAbilitySystemComponent.generated.h"

struct FGameplayEffectGroupedData;
class UPEVM_AttributeBasic;
class UPEVM_AttributeCustom;
class UPEVM_AttributeLeveling;

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAbilitySystemComponent final : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	explicit UPEAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Apply a grouped GE data to self Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ApplyEffectGroupedDataToSelf(const FGameplayEffectGroupedData GroupedData);

	/* Apply a grouped GE data to target Ability System Component */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ApplyEffectGroupedDataToTarget(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* TargetABSC);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void RemoveEffectGroupedDataFromSelf(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* InstigatorABSC, const int32 StacksToRemove = 1);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void RemoveEffectGroupedDataFromTarget(const FGameplayEffectGroupedData GroupedData, UAbilitySystemComponent* InstigatorABSC, UAbilitySystemComponent* TargetABSC, const int32 StacksToRemove = 1);
	
	template <typename T>
	const T* GetCustomAttributeSet() const
	{
		return Cast<T>(GetAttributeSet(T::StaticClass()));
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	TObjectPtr<UPEVM_AttributeBasic> BasicAttributes_VM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	TObjectPtr<UPEVM_AttributeCustom> CustomAttributes_VM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
	TObjectPtr<UPEVM_AttributeLeveling> LevelingAttributes_VM;

protected:
	virtual void InitializeComponent() override;
};
