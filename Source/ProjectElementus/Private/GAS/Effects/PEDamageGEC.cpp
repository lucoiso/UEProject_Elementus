// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "GAS/Effects/PEDamageGEC.h"
#include "GAS/Attributes/PEBasicStatusAS.h"
#include "GAS/Attributes/PECustomStatusAS.h"

#include "AbilitySystemComponent.h"

struct FDamageAttributesStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefenseRate);

	FDamageAttributesStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPEBasicStatusAS, Damage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPECustomStatusAS, AttackRate, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPECustomStatusAS, DefenseRate, Target, false);
	}
};

static const FDamageAttributesStatics& GetAttributesStatics()
{
	static FDamageAttributesStatics Attributes;
	return Attributes;
}

UPEDamageGEC::UPEDamageGEC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RelevantAttributesToCapture.Add(GetAttributesStatics().DamageDef);
	RelevantAttributesToCapture.Add(GetAttributesStatics().AttackRateDef);
	RelevantAttributesToCapture.Add(GetAttributesStatics().DefenseRateDef);
}

void UPEDamageGEC::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BaseDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributesStatics().DamageDef, EvaluationParameters,
		BaseDamage);
	BaseDamage += FMath::Max<float>(
		Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	float AttackRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributesStatics().AttackRateDef,
		EvaluationParameters, AttackRate);
	AttackRate = FMath::Max<float>(AttackRate, 0.0f);

	float DefenseRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributesStatics().AttackRateDef,
		EvaluationParameters, DefenseRate);
	DefenseRate = FMath::Max<float>(DefenseRate, 0.0f);

	const auto CalculateDamage = [BaseDamage, AttackRate, DefenseRate]() -> float
	{
		float DamageDone = BaseDamage + (AttackRate / DefenseRate * FMath::FRandRange(1.f, BaseDamage));

		if (DamageDone < 0.f)
		{
			DamageDone *= -1.f;
		}

		return DamageDone;
	};

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(GetAttributesStatics().DamageProperty,
			EGameplayModOp::Additive,
			CalculateDamage()));
}