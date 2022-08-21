// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "GameplayTagContainer.h"

static const FGameplayTag DeadStateTag = FGameplayTag::RequestGameplayTag("State.Dead");
static const FGameplayTag StunStateTag = FGameplayTag::RequestGameplayTag("State.Stunned");

static const FGameplayTag WeaponEquippedTag = FGameplayTag::RequestGameplayTag(FName("State.Weapon.Equipped"));
