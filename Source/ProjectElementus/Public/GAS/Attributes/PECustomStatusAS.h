// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/System/PEAttributeData.h"
#include "PECustomStatusAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Custom GAS | Attributes")
class PROJECTELEMENTUS_API UPECustomStatusAS : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPECustomStatusAS(const FObjectInitializer& ObjectInitializer);

private:
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_AttackRate)
		FGameplayAttributeData AttackRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, AttackRate)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_DefenseRate)
		FGameplayAttributeData DefenseRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, DefenseRate)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_SpeedRate)
		FGameplayAttributeData SpeedRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, SpeedRate)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_JumpRate)
		FGameplayAttributeData JumpRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, JumpRate)

		UPROPERTY(BlueprintReadOnly, Category = "Custom GAS | Attributes", ReplicatedUsing = OnRep_Gold)
		FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, Gold)

protected:
	UFUNCTION()
		void OnRep_AttackRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_DefenseRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_SpeedRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_JumpRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
		void OnRep_Gold(const FGameplayAttributeData& OldValue) const;
};
