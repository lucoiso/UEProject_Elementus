// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EOSVoiceChatUser.h"
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

UENUM(BlueprintType, Category = "Project Elementus | Enumerations")
enum class EAchievementMod : uint8
{
	Set,
	Add,
	Subtract
};

UCLASS(Category = "Project Elementus | Classes")
class PROJECTELEMENTUS_API UPEEOSLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FOnlineSubsystemEOS* GetOnlineSubsystemEOS();

	static FEOSVoiceChatUser* GetEOSVoiceChatUser(const uint8 LocalUserNum);

	/* Mute the user in session */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (DisplayName = "Mute EOS User in Session"))
	static void MuteEOSSessionVoiceChatUser(const int32 LocalUserNum, const bool bMute);

	/* Get the session owning user name from a session data handler */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Get EOS Session Owning User Name from Handle"))
	static FString GetEOSSessionOwningUserNameFromHandle(const FSessionDataHandler DataHandle);

	/* Get the session id from data handler */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Get EOS Session Id from Handle"))
	static FString GetEOSSessionIdFromHandle(const FSessionDataHandler DataHandle);

	/* Get the session ping from data handler */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Get EOS Session Ping from Handle"))
	static int32 GetEOSSessionPingFromHandle(const FSessionDataHandler DataHandle);

	/* Get the default EOS session name */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Get EOS Session Name"))
	static FName GetEOSSessionName();

	/* Check if the user is logged in EOS */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Is User Logged in EOS"))
	static bool IsUserLoggedInEOS(const int32 LocalUserNum);

	/* Check if the user is hosting a session */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Is Hosting EOS Session"))
	static bool IsHostingEOSSession();

	/* Check if the user is in a session */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Is User in a EOS Session"))
	static bool IsUserInAEOSSession();

	/* Generate a session settings to use to create a new session */
	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions", meta = (DisplayName = "Generate EOS Session Settings"))
	static FSessionSettingsHandler GenerateEOSSessionSettings(const int32 NumPublicConnections = 4, const int32 NumPrivateConnections = 4, const bool bShouldAdvertise = true, const bool bAllowJoinInProgress = true, const bool bIsLANMatch = false, const bool bIsDedicated = false, const bool bUsesStats = true, const bool bAllowInvites = true, const bool bUsesPresence = true, const bool bAllowJoinViaPresence = true, const bool bAllowJoinViaPresenceFriendsOnly = false, const bool bAntiCheatProtected = true, const bool bUseLobbiesIfAvailable = true, const bool bUseLobbiesVoiceChatIfAvailable = true);

	/* Update the user presence in EOS overlay */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (DisplayName = "Update EOS Presence"))
	static void UpdateEOSPresence(const int32 LocalUserNum, const FString& PresenceText, const bool bOnline);

	/* Modify the status of a achievement */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (DisplayName = "Write EOS Achievement"))
	static void WriteEOSAchievement(const int32 LocalUserNum, const EAchievementMod Modifier, const FName StatName, const float Percentage = 1.f);
};
