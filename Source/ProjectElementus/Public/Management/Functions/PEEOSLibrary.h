// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSessionSettings.h"
#include "PEEOSLibrary.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FSessionDataHandler
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSearchResult Result;
};


USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FSessionSettingsHandler
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSettings Settings;
};

UCLASS(Category = "Project Elementus | Classes | Functions")
class PROJECTELEMENTUS_API UPEEOSLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FOnlineSubsystemEOS* GetOnlineSubsystemEOS();

	static FEOSVoiceChatUser* GetEOSVoiceChatUser(const int8 LocalUserNum);

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	static void MuteSessionVoiceChatUser(const int32 LocalUserNum, const bool bMute);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FString GetSessionOwningUserNameFromHandle(const FSessionDataHandler DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FString GetSessionIdFromHandle(const FSessionDataHandler DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static int32 GetSessionPingFromHandle(const FSessionDataHandler DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FName GetGameSessionName();

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static bool IsUserLoggedIn(const int32 LocalUserNum);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FSessionSettingsHandler GenerateSessionSettings(const int32 NumPublicConnections = 4,
	                                                       const int32 NumPrivateConnections = 4,
	                                                       const bool bShouldAdvertise = true,
	                                                       const bool bAllowJoinInProgress = true,
	                                                       const bool bIsLANMatch = false,
	                                                       const bool bIsDedicated = false,
	                                                       const bool bUsesStats = true,
	                                                       const bool bAllowInvites = true,
	                                                       const bool bUsesPresence = true,
	                                                       const bool bAllowJoinViaPresence = true,
	                                                       const bool bAllowJoinViaPresenceFriendsOnly = false,
	                                                       const bool bAntiCheatProtected = true,
	                                                       const bool bUseLobbiesIfAvailable = true,
	                                                       const bool bUseLobbiesVoiceChatIfAvailable = true);
};
