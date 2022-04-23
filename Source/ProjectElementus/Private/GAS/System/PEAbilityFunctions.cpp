// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/PEAbilityFunctions.h"

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromSingleHitResult(
	const FHitResult HitResult)
{
	FGameplayAbilityTargetDataHandle TargetData;

	FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	TargetData.Add(NewData);

	return TargetData;
}

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromHitResultArray(
	const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FGameplayAbilityTargetDataHandle UPEAbilityFunctions::MakeTargetDataHandleFromActorArray(
	const TArray<AActor*> TargetActors)
{
	if (!TargetActors.IsEmpty())
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);

		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}
