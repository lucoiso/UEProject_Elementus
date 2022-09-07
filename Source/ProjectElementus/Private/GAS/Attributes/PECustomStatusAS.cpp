// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Attributes/PECustomStatusAS.h"
#include "Actors/Character/PECharacter.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

UPECustomStatusAS::UPECustomStatusAS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , AttackRate(1.f)
	  , DefenseRate(1.f)
	  , SpeedRate(1.f)
	  , JumpRate(1.f)
	  , Gold(0.f)
{
	static const ConstructorHelpers::FObjectFinder<UDataTable>
		CustomAttributesMetaData_ObjRef(TEXT("/Game/Main/Data/GAS/AttributeMetaDatas/DT_CustomStatusAS"));
	if constexpr (&CustomAttributesMetaData_ObjRef.Object != nullptr)
	{
		UAttributeSet::InitFromMetaDataTable(CustomAttributesMetaData_ObjRef.Object);
	}
}

void UPECustomStatusAS::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue,
                                            const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetSpeedRateAttribute() || Attribute == GetJumpRateAttribute())
	{
		if (const APlayerState* const State = Cast<APlayerState>(GetOwningActor()))
		{
			if (const APECharacter* const Character = State->GetPawn<APECharacter>())
			{
				if (UCharacterMovementComponent* const MovComp = Character->GetCharacterMovement();
					ensureAlwaysMsgf(IsValid(MovComp), TEXT("%s have a invalid Movement Component"), *GetName()))
				{
					//Check if the attribute is equal to speed or jump rate and multiply the value by the rate
					if (Attribute == GetSpeedRateAttribute())
					{
						MovComp->MaxWalkSpeed = NewValue * Character->GetDefaultWalkSpeed();
						MovComp->MaxWalkSpeedCrouched = NewValue * Character->GetDefaultCrouchSpeed();
					}
					else if (Attribute == GetJumpRateAttribute())
					{
						MovComp->JumpZVelocity = NewValue * Character->GetDefaultJumpVelocity();
					}
				}
			}
		}
	}
}

#pragma region Attribute Replication
void UPECustomStatusAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, AttackRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, DefenseRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, SpeedRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, JumpRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, Gold, COND_None, REPNOTIFY_Always);
}

void UPECustomStatusAS::OnRep_AttackRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, AttackRate, OldValue);
}

void UPECustomStatusAS::OnRep_DefenseRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, DefenseRate, OldValue);
}

void UPECustomStatusAS::OnRep_SpeedRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, SpeedRate, OldValue);
}

void UPECustomStatusAS::OnRep_JumpRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, JumpRate, OldValue);
}

void UPECustomStatusAS::OnRep_Gold(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, Gold, OldValue);
}
#pragma endregion Attribute Replication
