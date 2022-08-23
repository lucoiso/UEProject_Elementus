// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "GameplayTagContainer.h"

#pragma region Equipment
static const FGameplayTag& GlobalTag_GenericEquipped = FGameplayTag::RequestGameplayTag("EquipSlot.Generic.Equipped");
static const FGameplayTag& GlobalTag_WeaponEquipped = FGameplayTag::RequestGameplayTag("State.Weapon.Equipped");
#pragma endregion Equipment

#pragma region Effect
static const FGameplayTag& GlobalTag_Damage = FGameplayTag::RequestGameplayTag("Data.Damage");
#pragma endregion Effect

#pragma region State
static const FGameplayTag& GlobalTag_CanInteract = FGameplayTag::RequestGameplayTag("State.CanInteract");
static const FGameplayTag& GlobalTag_CannotInteract = FGameplayTag::RequestGameplayTag("State.CannotInteract");
static const FGameplayTag& GlobalTag_DeadState = FGameplayTag::RequestGameplayTag("State.Dead");
static const FGameplayTag& GlobalTag_StunState = FGameplayTag::RequestGameplayTag("State.Stunned");
static const FGameplayTag& GlobalTag_AimingState = FGameplayTag::RequestGameplayTag("State.Aiming");
static const FGameplayTag& GlobalTag_WaitingConfirmationState = FGameplayTag::RequestGameplayTag("State.WaitingConfirm");
static const FGameplayTag& GlobalTag_WaitingCancelationState = FGameplayTag::RequestGameplayTag("State.WaitingCancel");
#pragma endregion State

#pragma region Attributes
static const FGameplayTag& GlobalTag_RegenBlock_Health = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Health");
static const FGameplayTag& GlobalTag_RegenBlock_Mana = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana");
static const FGameplayTag& GlobalTag_RegenBlock_Stamina = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina");

static const FGameplayTag& GlobalTag_CostWhileActive_Stamina = FGameplayTag::RequestGameplayTag("GameplayAbility.State.CostWhileActive.Stamina");
static const FGameplayTag& GlobalTag_CostWhileActive_Mana = FGameplayTag::RequestGameplayTag("GameplayAbility.State.CostWhileActive.Mana");
#pragma endregion Attributes
