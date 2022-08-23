// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Targeting/PEGroundTargeting.h"

APEGroundTargeting::APEGroundTargeting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TraceProfile = FCollisionProfileName(TEXT("Target"));
}
