// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VoiceChat.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemEOS.h"
#include "PEGameInstance.generated.h"

struct FVoiceChatResult;
class FOnlineAccountCredentials;
class IOnlineSubsystem;
class FOnlineSessionSettings;
/**
 *
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FSessionDataHandle
{
	GENERATED_USTRUCT_BODY()

	FOnlineSessionSearchResult Result;
};

USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FEOSVoiceChatChannelCredentials
{
	GENERATED_USTRUCT_BODY()

	FString ClientBaseUrl;
	FString ParticipantToken;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateSessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSessionsDelegate, const TArray<FSessionDataHandle>&, Sessions);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCancelFindSessionsDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJoinSessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDestroySessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserLoginDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserLogoutDelegate);

UCLASS(Category = "Project Elementus | Classes | Management")
class UPEGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

protected:
	FOnlineSubsystemEOSPtr OnlineSubsystemEOS;

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void InitializeVoiceChatFramework();

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void UninitializeVoiceChatFramework();

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ConnectVoiceChatFramework();

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void DisconnectVoiceChatFramework();

protected:
	FOnVoiceChatInitializeCompleteDelegate OnVoiceChatInitializeCompleteDelegate;
	void OnVoiceChatInitialized(const FVoiceChatResult& Result);

	FOnVoiceChatUninitializeCompleteDelegate OnVoiceChatUninitializeCompleteDelegate;
	void OnVoiceChatUninitialized(const FVoiceChatResult& Result);

	FOnVoiceChatConnectCompleteDelegate OnVoiceChatConnectCompleteDelegate;
	void OnVoiceChatConnected(const FVoiceChatResult& Result);

	FOnVoiceChatDisconnectCompleteDelegate OnVoiceChatDisconnectCompleteDelegate;
	void OnVoiceChatDisconnected(const FVoiceChatResult& Result);

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void LoginToVoiceChatFramework(const int32 LocalUserNum);

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void LogoutFromVoiceChatFramework(const int32 LocalUserNum);

protected:
	FOnVoiceChatLoginCompleteDelegate OnVoiceChatLoginCompleteDelegate;
	void OnVoiceChatLogin(const FString& PlayerName, const FVoiceChatResult& Result);

	FOnVoiceChatLogoutCompleteDelegate OnVoiceChatLogoutCompleteDelegate;
	void OnVoiceChatLogout(const FString& PlayerName, const FVoiceChatResult& Result);

private:
	TMap<const int8, TSharedPtr<IVoiceChatUser>> EOSVoiceChatUsers;

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void MuteVoiceChatUser(const int32 LocalUserNum, const bool bMute) const;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ConnectVoiceChatToSessionChannel(const int32 LocalUserNum, const FString ChannelName,
	                                      const FEOSVoiceChatChannelCredentials Credentials);

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void LeaveVoiceChatSessionChannel(const int32 LocalUserNum, const FString ChannelName);

protected:
	FOnVoiceChatChannelJoinCompleteDelegate OnVoiceChatChannelJoinCompleteDelegate;
	void OnVoiceChatChannelJoined(const FString& ChannelName, const FVoiceChatResult& Result);

	FOnVoiceChatChannelLeaveCompleteDelegate OnVoiceChatChannelLeaveCompleteDelegate;
	void OnVoiceChatChannelLeft(const FString& ChannelName, const FVoiceChatResult& Result);

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool CreateDefaultSession(const bool bIsLAN = false, const bool bIsDedicated = false,
	                          const int32 NumOfPublicConnections = 4);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FCreateSessionDelegate CreateSessionDelegate;

	/* Function created for testing only - Will call FindSessionsDelegate on complete */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultFindSessions(const int32 LocalUserNum, const bool bIsLANQuery = false, const int32 MaxResults = 100);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FFindSessionsDelegate FindSessionsDelegate;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool CancelFindSessions();

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FCancelFindSessionsDelegate CancelFindSessionsDelegate;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultJoinSession(const int32 LocalUserNum, const FSessionDataHandle SessionData);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FJoinSessionDelegate JoinSessionDelegate;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultDestroySession();

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FDestroySessionDelegate DestroySessionDelegate;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FString GetSessionOwningUserNameFromHandle(const FSessionDataHandle DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FString GetSessionIdFromHandle(const FSessionDataHandle DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static int32 GetSessionPingFromHandle(const FSessionDataHandle DataHandle);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	static FName GetGameSessionName();

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ServerTravelToLevel(const FName LevelName) const;

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void ClientTravelToSessionLevel() const;

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	TArray<FSessionDataHandle> GetSessionsDataHandles() const;

	bool EOS_CreateSession(const int8 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings);
	bool EOS_FindSessions(const int8 SearchingPlayerNum, const bool bIsLANQuery = false, const int8 MaxResults = 100);
	bool EOS_JoinSession(const int8 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession);
	bool EOS_DestroySession();

protected:
	void OnSessionCreated(const FName SessionName, const bool bResult);
	void OnSessionsFound(const bool bResult);
	void OnCancelFindSessions(const bool bResult);
	void OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
	void OnSessionDestroyed(const FName SessionName, const bool bResult);

	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;
	void OnSessionInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
	                             const FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);

private:
	TSharedPtr<FOnlineSessionSearch> EOSSearchSettings;
	FString EOSCurrentSessionInfo;

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultLogin(const int32 LocalUserNum, const FString Token, const int32 Port, const bool bUsePortal = false);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FUserLoginDelegate UserLoginDelegate;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultLogout(const int32 LocalUserNum);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FUserLogoutDelegate UserLogoutDelegate;

	bool EOS_Login(const int8 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials);
	bool EOS_Logout(const int8 LocalUserNum);

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	bool IsUserLoggedIn(const int32 LocalUserNum) const;

protected:
	void OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
	                     const FString& Error);
	void OnLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful);
};
