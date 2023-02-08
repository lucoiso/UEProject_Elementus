// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Targeting/PEGroundTargeting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEGroundTargeting)

APEGroundTargeting::APEGroundTargeting(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TraceProfile = FCollisionProfileName(TEXT("Target"));
}
