// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "Engine/DataTable.h"

UPEAbilitySystemGlobals::UPEAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static const ConstructorHelpers::FClassFinder<UGameplayEffect> DeathGameplayEffect_ClassRef(
		TEXT("/Game/Main/GAS/Effects/States/GE_Death"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&DeathGameplayEffect_ClassRef.Class != nullptr)
#else
	if (&DeathGameplayEffect_ClassRef.Class != nullptr)
#endif
	{
		GlobalDeathEffect = DeathGameplayEffect_ClassRef.Class;
	}

	static const ConstructorHelpers::FClassFinder<UGameplayEffect> StunGameplayEffect_ClassRef(
		TEXT("/Game/Main/GAS/Effects/States/GE_Stun"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&StunGameplayEffect_ClassRef.Class != nullptr)
#else
	if (&StunGameplayEffect_ClassRef.Class != nullptr)
#endif
	{
		GlobalStunEffect = StunGameplayEffect_ClassRef.Class;
	}

	static const ConstructorHelpers::FObjectFinder<UDataTable> MainAttributesMetaData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_BasicStatusAS"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&MainAttributesMetaData_ObjRef.Object != nullptr)
#else
	if (&MainAttributesMetaData_ObjRef.Object != nullptr)
#endif
	{
		MainStatusAttributeData = MainAttributesMetaData_ObjRef.Object;
	}

	static const ConstructorHelpers::FObjectFinder<UDataTable> CustomAttributesMetaData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_CustomStatusAS"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&CustomAttributesMetaData_ObjRef.Object != nullptr)
#else
	if (&CustomAttributesMetaData_ObjRef.Object != nullptr)
#endif
	{
		CustomStatusAttributeData = CustomAttributesMetaData_ObjRef.Object;
	}
	
	static const ConstructorHelpers::FObjectFinder<UDataTable> LevelingAttributesMetaData_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_LevelingAS"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&LevelingAttributesMetaData_ObjRef.Object != nullptr)
#else
	if (&LevelingAttributesMetaData_ObjRef.Object != nullptr)
#endif
	{
		LevelingAttributeData = LevelingAttributesMetaData_ObjRef.Object;
	}

	static const ConstructorHelpers::FObjectFinder<UDataTable> LevelingBonus_ObjRef(
		TEXT("/Game/Main/GAS/Data/DT_LevelingBonus"));
#if __cplusplus > 201402L // Check if C++ > C++14
	if constexpr (&LevelingBonus_ObjRef.Object != nullptr)
#else
	if (&LevelingBonus_ObjRef.Object != nullptr)
#endif
	{
		LevelingBonusData = LevelingBonus_ObjRef.Object;
	}
}

UGameplayEffect* UPEAbilitySystemGlobals::GetGlobalDeathEffect()
{
	return GlobalDeathEffect.LoadSynchronous()->GetDefaultObject<UGameplayEffect>();
}

UGameplayEffect* UPEAbilitySystemGlobals::GetGlobalStunEffect()
{
	return GlobalStunEffect.LoadSynchronous()->GetDefaultObject<UGameplayEffect>();
}

UDataTable* UPEAbilitySystemGlobals::GetMainStatusAttributeMetaData()
{
	return MainStatusAttributeData.LoadSynchronous();
}

UDataTable* UPEAbilitySystemGlobals::GetCustomStatusAttributeMetaData()
{
	return CustomStatusAttributeData.LoadSynchronous();
}

UDataTable* UPEAbilitySystemGlobals::GetLevelingAttributeMetaData()
{
	return LevelingAttributeData.LoadSynchronous();
}

UDataTable* UPEAbilitySystemGlobals::GetLevelingBonusData()
{
	return LevelingBonusData.LoadSynchronous();
}
