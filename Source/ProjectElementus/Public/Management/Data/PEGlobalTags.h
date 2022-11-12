// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#pragma region Generic
constexpr auto GlobalTag_GenericCooldown = "GameplayEffect.Cooldown";
#pragma endregion Generic

#pragma region SetByCaller
constexpr auto GlobalTag_SetByCallerDuration = "SetByCaller.Duration";
constexpr auto GlobalTag_SetByCallerHealth = "SetByCaller.Health";
constexpr auto GlobalTag_SetByCallerStamina = "SetByCaller.Stamina";
constexpr auto GlobalTag_SetByCallerMana = "SetByCaller.Mana";
constexpr auto GlobalTag_SetByCallerFloat1 = "SetByCaller.Float1";
constexpr auto GlobalTag_SetByCallerFloat2 = "SetByCaller.Float2";
constexpr auto GlobalTag_SetByCallerFloat3 = "SetByCaller.Float3";
constexpr auto GlobalTag_SetByCallerFloat4 = "SetByCaller.Float4";
constexpr auto GlobalTag_SetByCallerFloat5 = "SetByCaller.Float5";
#pragma endregion SetByCaller

#pragma region Equipment
constexpr auto GlobalTag_EquipSlot_Base = "EquipSlot";

constexpr auto GlobalTag_WeaponSlot_Base = "EquipSlot.Weapon";
constexpr auto GlobalTag_WeaponSlot_Both = "EquipSlot.Weapon.Both";
constexpr auto GlobalTag_WeaponSlot_Left = "EquipSlot.Weapon.Left";
constexpr auto GlobalTag_WeaponSlot_Right = "EquipSlot.Weapon.Right";
#pragma endregion Equipment

#pragma region Effect
constexpr auto GlobalTag_Damage = "Data.Damage";
#pragma endregion Effect

#pragma region State
constexpr auto GlobalTag_CanInteract = "State.CanInteract";
constexpr auto GlobalTag_CannotInteract = "State.CannotInteract";
constexpr auto GlobalTag_DeadState = "State.Dead";
constexpr auto GlobalTag_StunState = "State.Stunned";
constexpr auto GlobalTag_AimingState = "State.Aiming";
constexpr auto GlobalTag_AimingBlockedState = "State.Aiming.Blocked";
constexpr auto GlobalTag_WaitingConfirmationState = "State.WaitingConfirm";
constexpr auto GlobalTag_WaitingCancelationState = "State.WaitingCancel";
#pragma endregion State

#pragma region Attributes
constexpr auto GlobalTag_RegenBlock_Health = "GameplayEffect.Debuff.Regeneration.Block.Health";
constexpr auto GlobalTag_RegenBlock_Mana = "GameplayEffect.Debuff.Regeneration.Block.Mana";
constexpr auto GlobalTag_RegenBlock_Stamina = "GameplayEffect.Debuff.Regeneration.Block.Stamina";

constexpr auto GlobalTag_CostWhileActive_Stamina = "GameplayAbility.State.CostWhileActive.Stamina";
constexpr auto GlobalTag_CostWhileActive_Mana = "GameplayAbility.State.CostWhileActive.Mana";
#pragma endregion Attributes
