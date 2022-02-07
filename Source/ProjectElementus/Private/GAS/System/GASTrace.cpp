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
