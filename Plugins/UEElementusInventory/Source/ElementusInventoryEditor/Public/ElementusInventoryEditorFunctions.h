// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEElementusInventory

#pragma once

namespace ElementusEdHelper
{
	static FString EnumToString(const EElementusItemType InEnumName)
	{
		switch (InEnumName)
		{
			case EElementusItemType::None :
			return "None";

			case EElementusItemType::Consumable :
			return "Consumable";

			case EElementusItemType::Armor :
			return "Armor";

			case EElementusItemType::Weapon :
			return "Weapon";
			
			case EElementusItemType::Accessory :
			return "Accessory";

			case EElementusItemType::Crafting :
			return "Crafting";

			case EElementusItemType::Material :
			return "Material";

			case EElementusItemType::Information :
			return "Information";

			case EElementusItemType::Special :
			return "Special";

			case EElementusItemType::Event :
			return "Event";

			case EElementusItemType::Quest :
			return "Quest";

			case EElementusItemType::Junk :
			return "Junk";

			case EElementusItemType::Other :
			return "Other";

			default:
			return FString();
		}
	}

	static TArray<TSharedPtr<FString>> GetEnumValuesAsStringArray()
	{
		TArray<TSharedPtr<FString>> EnumValues;
		for (int32 i = 0; i < static_cast<int32>(EElementusItemType::MAX); i++)
		{
			EnumValues.Add(MakeShareable<FString>(new FString(EnumToString(static_cast<EElementusItemType>(i)))));
		}
		return EnumValues;
	}
}
