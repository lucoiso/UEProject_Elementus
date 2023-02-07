// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <EOSVoiceChatUser.h>
#include <OnlineSubsystemEOS.h>
#include <OnlineSessionSettings.h>
#include <Online/CoreOnlineFwd.h>
#include <Interfaces/OnlineStatsInterface.h>
#include <Interfaces/OnlineIdentityInterface.h>
#include <Interfaces/OnlinePresenceInterface.h>
#include <Interfaces/OnlineSessionInterface.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include "PEEOSLibrary.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FEOSSessionSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	int32 NumPublicConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	int32 NumPrivateConnections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bShouldAdvertise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bAllowJoinInProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bIsLANMatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bIsDedicated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bUsesStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bAllowInvites;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bUsesPresence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bAllowJoinViaPresence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bAllowJoinViaPresenceFriendsOnly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bAntiCheatProtected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bUseLobbiesIfAvailable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus")
	bool bUseLobbiesVoiceChatIfAvailable;

	FOnlineSessionSettings GetNativeEOSSettings() const;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FSessionDataHandler
{
	GENERATED_BODY()

	FOnlineSessionSearchResult Result;
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

	/* Update the user presence in EOS overlay */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (DisplayName = "Update EOS Presence"))
	static void UpdateEOSPresence(const int32 LocalUserNum, const FString& PresenceText, const bool bOnline);

	/* Modify all EOS stats in the given map */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (DisplayName = "Ingest EOS Stats"))
	static void IngestEOSStats(const int32 LocalUserNum, const TMap<FName, int32>& StatsMap);

	static const FUniqueNetIdPtr GetUniqueNetId(const int32 LocalUserNum, IOnlineSubsystem* OnlineSubsystem = nullptr);
	static const IOnlineIdentityPtr GetIdentityInterface(IOnlineSubsystem* OnlineSubsystem = nullptr);
	static const IOnlineSessionPtr GetSessionInterface(IOnlineSubsystem* OnlineSubsystem = nullptr);
	static const IOnlinePresencePtr GetPresenceInterface(IOnlineSubsystem* OnlineSubsystem = nullptr);
	static const IOnlineStatsPtr GetStatsInterface(IOnlineSubsystem* OnlineSubsystem = nullptr);

private:
	static bool ValidateOnlineSubsystem(IOnlineSubsystem* OnlineSubsystem);
};
