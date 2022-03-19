// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/System/GASTrace.h"

FGASTraceDataHandle::FGASTraceDataHandle()
{
	Hit = FHitResult();
	MaxRange = 0.f;
	bDoTick = false;
	Type = EGASTraceType::Single;
	FilterHandle.Filter = TSharedPtr<FGameplayTargetDataFilter>(new FGameplayTargetDataFilter);
	CollisionProfile = "Default";
}