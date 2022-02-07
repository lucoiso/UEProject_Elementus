#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddAttribute.generated.h"

class UAttributeSet;
struct FComponentRequestHandle;
/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Attribute"))
class UGameFeatureAction_AddAttribute final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="Settings")
	TSoftClassPtr<UAttributeSet> Attribute;

protected:
	virtual void OnGameFeatureActivating() override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& WorldContext) override;

private:
	void HandleActorExtension(AActor* Owner, FName EventName);
	void ResetExtension();

	UFUNCTION(NetMulticast, Reliable)
	void AddAttribute(AActor* TargetActor);
	void AddAttribute_Implementation(AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveAttribute(AActor* TargetActor);
	void RemoveAttribute_Implementation(AActor* TargetActor);

	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<UAttributeSet>> ActiveExtensions;
	TArray<TSharedPtr<FComponentRequestHandle>> ActiveRequests;
};
