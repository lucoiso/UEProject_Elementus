// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include "GAS/Attributes/PEAttributeBase.h"
#include "PECustomStatusAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPECustomStatusAS final : public UPEAttributeBase
{
	GENERATED_BODY()

public:
	explicit UPECustomStatusAS(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_AttackRate)
	FGameplayAttributeData AttackRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, AttackRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_DefenseRate)
	FGameplayAttributeData DefenseRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, DefenseRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_SpeedRate)
	FGameplayAttributeData SpeedRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, SpeedRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_JumpRate)
	FGameplayAttributeData JumpRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, JumpRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_Gold)
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
