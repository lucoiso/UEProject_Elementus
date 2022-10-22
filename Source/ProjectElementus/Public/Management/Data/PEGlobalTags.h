// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "GameplayTagContainer.h"

#pragma region Generic
static const FGameplayTag GlobalTag_GenericCooldown = FGameplayTag::RequestGameplayTag("GameplayEffect.Cooldown");
#pragma endregion Generic

#pragma region SetByCaller
static const FGameplayTag GlobalTag_SetByCallerDuration = FGameplayTag::RequestGameplayTag("SetByCaller.Duration");
static const FGameplayTag GlobalTag_SetByCallerHealth = FGameplayTag::RequestGameplayTag("SetByCaller.Health");
static const FGameplayTag GlobalTag_SetByCallerStamina = FGameplayTag::RequestGameplayTag("SetByCaller.Stamina");
static const FGameplayTag GlobalTag_SetByCallerMana = FGameplayTag::RequestGameplayTag("SetByCaller.Mana");
static const FGameplayTag GlobalTag_SetByCallerFloat1 = FGameplayTag::RequestGameplayTag("SetByCaller.Float1");
static const FGameplayTag GlobalTag_SetByCallerFloat2 = FGameplayTag::RequestGameplayTag("SetByCaller.Float2");
static const FGameplayTag GlobalTag_SetByCallerFloat3 = FGameplayTag::RequestGameplayTag("SetByCaller.Float3");
static const FGameplayTag GlobalTag_SetByCallerFloat4 = FGameplayTag::RequestGameplayTag("SetByCaller.Float4");
static const FGameplayTag GlobalTag_SetByCallerFloat5 = FGameplayTag::RequestGameplayTag("SetByCaller.Float5");
#pragma endregion SetByCaller

#pragma region Equipment
static const FGameplayTag GlobalTag_EquipSlot_Base = FGameplayTag::RequestGameplayTag("EquipSlot");

static const FGameplayTag GlobalTag_WeaponSlot_Base = FGameplayTag::RequestGameplayTag("EquipSlot.Weapon");
static const FGameplayTag GlobalTag_WeaponSlot_Both = FGameplayTag::RequestGameplayTag("EquipSlot.Weapon.Both");
static const FGameplayTag GlobalTag_WeaponSlot_Left = FGameplayTag::RequestGameplayTag("EquipSlot.Weapon.Left");
static const FGameplayTag GlobalTag_WeaponSlot_Right = FGameplayTag::RequestGameplayTag("EquipSlot.Weapon.Right");
#pragma endregion Equipment

#pragma region Effect
static const FGameplayTag GlobalTag_Damage = FGameplayTag::RequestGameplayTag("Data.Damage");
#pragma endregion Effect

#pragma region State
static const FGameplayTag GlobalTag_CanInteract = FGameplayTag::RequestGameplayTag("State.CanInteract");
static const FGameplayTag GlobalTag_CannotInteract = FGameplayTag::RequestGameplayTag("State.CannotInteract");
static const FGameplayTag GlobalTag_DeadState = FGameplayTag::RequestGameplayTag("State.Dead");
static const FGameplayTag GlobalTag_StunState = FGameplayTag::RequestGameplayTag("State.Stunned");
static const FGameplayTag GlobalTag_AimingState = FGameplayTag::RequestGameplayTag("State.Aiming");
static const FGameplayTag GlobalTag_AimingBlockedState = FGameplayTag::RequestGameplayTag("State.Aiming.Blocked");
static const FGameplayTag GlobalTag_WaitingConfirmationState = FGameplayTag::RequestGameplayTag("State.WaitingConfirm");
static const FGameplayTag GlobalTag_WaitingCancelationState = FGameplayTag::RequestGameplayTag("State.WaitingCancel");
#pragma endregion State

#pragma region Attributes
static const FGameplayTag GlobalTag_RegenBlock_Health = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Health");
static const FGameplayTag GlobalTag_RegenBlock_Mana = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Mana");
static const FGameplayTag GlobalTag_RegenBlock_Stamina = FGameplayTag::RequestGameplayTag("GameplayEffect.Debuff.Regeneration.Block.Stamina");

static const FGameplayTag GlobalTag_CostWhileActive_Stamina = FGameplayTag::RequestGameplayTag("GameplayAbility.State.CostWhileActive.Stamina");
static const FGameplayTag GlobalTag_CostWhileActive_Mana = FGameplayTag::RequestGameplayTag("GameplayAbility.State.CostWhileActive.Mana");
#pragma endregion Attributes
