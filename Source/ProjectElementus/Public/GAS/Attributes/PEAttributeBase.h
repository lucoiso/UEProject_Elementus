// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <AbilitySystemComponent.h>
#include <AttributeSet.h>
#include "PEAttributeBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
			GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS(Abstract, NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEAttributeBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	explicit UPEAttributeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/* A helper function to clamp attribute values */
	virtual void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	virtual void InitFromMetaDataTable(const UDataTable* DataTable);

	template<typename ComponentTy>
	ComponentTy* GetCastedAbilitySystemComponent()
	{
		return Cast<ComponentTy>(GetOwningAbilitySystemComponent());
	}
};
