// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Targeting/PELineTargeting.h"

APELineTargeting::APELineTargeting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TraceProfile = FCollisionProfileName("Target");
}
