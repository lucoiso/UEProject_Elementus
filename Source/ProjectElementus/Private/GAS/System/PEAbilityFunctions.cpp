// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilityFunctions.h"
#include <AbilitySystemComponent.h>

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromSingleHitResult(const FHitResult HitResult)
{
	FGameplayAbilityTargetDataHandle TargetData;

	FGameplayAbilityTargetData_SingleTargetHit* const NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	TargetData.Add(NewData);

	return TargetData;
}

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromHitResultArray(const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* const NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors)
{
	if (!TargetActors.IsEmpty())
	{
		FGameplayAbilityTargetData_ActorArray* const NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);

		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

bool UPEAbilityFunctions::HasAbilityWithClass(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!IsValid(TargetABSC))
	{
		return false;
	}

	return TargetABSC->FindAbilitySpecFromClass(AbilityClass) != nullptr;
}

void UPEAbilityFunctions::GiveAbilityWithoutBinding(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability, const bool bTryRemoveExistingAbilityWithClass)
{
	if (!TargetABSC->IsOwnerActorAuthoritative() || !IsValid(Ability))
	{
		return;
	}
	
	if (ensureAlwaysMsgf(IsValid(TargetABSC), TEXT("%s have a invalid Ability System Component"), *TargetABSC->GetAvatarActor()->GetName()))
	{
		if (bTryRemoveExistingAbilityWithClass)
		{
			if (const FGameplayAbilitySpec* const AbilitySpec = TargetABSC->FindAbilitySpecFromClass(Ability))
			{
				RemoveAbility(TargetABSC, AbilitySpec->Ability->GetClass());
			}
		}

		const FGameplayAbilitySpec Spec = FGameplayAbilitySpec(*Ability, 1, INDEX_NONE, TargetABSC->GetAvatarActor());
		TargetABSC->GiveAbility(Spec);
	}
}

void UPEAbilityFunctions::GiveAbility(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability, const FName InputId, const UEnum* EnumerationClass, const bool bTryRemoveExistingAbilityWithInput = true, const bool bTryRemoveExistingAbilityWithClass = true)
{
	if (!TargetABSC->IsOwnerActorAuthoritative() || !IsValid(Ability))
	{
		return;
	}
	
	if (ensureAlwaysMsgf(IsValid(TargetABSC), TEXT("%s have a invalid Ability System Component"), *TargetABSC->GetAvatarActor()->GetName()))
	{
		// If can't find the input value, will cancel the ability addition
		const uint32 InputID = EnumerationClass->GetValueByName(InputId, EGetByNameFlags::CheckAuthoredName);
		if (InputID == INDEX_NONE)
		{
			return;
		}

		const auto RemoveAbility_Lambda = [&](const FGameplayAbilitySpec* const& AbilitySpec) -> void
		{
			if (AbilitySpec != nullptr)
			{
				RemoveAbility(TargetABSC, AbilitySpec->Ability->GetClass());
			}
		};

		if (bTryRemoveExistingAbilityWithClass)
		{
			const FGameplayAbilitySpec* const AbilitySpec = TargetABSC->FindAbilitySpecFromClass(Ability);
			RemoveAbility_Lambda(AbilitySpec);
		}

		if (bTryRemoveExistingAbilityWithInput)
		{
			const FGameplayAbilitySpec* const AbilitySpec = TargetABSC->FindAbilitySpecFromInputID(InputID);
			RemoveAbility_Lambda(AbilitySpec);
		}

		const FGameplayAbilitySpec Spec = FGameplayAbilitySpec(*Ability, 1, InputID, TargetABSC->GetAvatarActor());

		TargetABSC->GiveAbility(Spec);
	}
}

void UPEAbilityFunctions::RemoveAbility(UAbilitySystemComponent* TargetABSC, const TSubclassOf<UGameplayAbility> Ability)
{
	if (!TargetABSC->IsOwnerActorAuthoritative() || !IsValid(Ability))
	{
		return;
	}
	
	if (ensureAlwaysMsgf(IsValid(TargetABSC), TEXT("%s have a invalid Ability System Component"), *TargetABSC->GetAvatarActor()->GetName()))
	{
		const FGameplayAbilitySpec* const AbilitySpec = TargetABSC->FindAbilitySpecFromClass(Ability);

		if (AbilitySpec == nullptr)
		{
			return;
		}

		TargetABSC->SetRemoveAbilityOnEnd(AbilitySpec->Handle);
		TargetABSC->ClearAbility(AbilitySpec->Handle);
	}
}
