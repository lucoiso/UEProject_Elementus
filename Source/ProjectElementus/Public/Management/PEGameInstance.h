// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PEGameInstance.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateSessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSessionsDelegate, const TArray<FSessionDataHandle>&, Sessions);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJoinSessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDestroySessionDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserLoginDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserLogoutDelegate);

UCLASS(Category = "Project Elementus | Classes | Management")
class UPEGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool CreateDefaultLocalSession(const bool bIsLAN = false, const bool bIsDedicated = false,
	                               const int32 NumOfPublicConnections = 4);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FCreateSessionDelegate CreateSessionDelegate;

	/* Function created for testing only - Will call FindSessionsDelegate on complete */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultFindSessions(const int32 LocalUserNum);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FFindSessionsDelegate FindSessionsDelegate;

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

	UFUNCTION(BlueprintPure, Category = "Project Elementus | Functions")
	TArray<FSessionDataHandle> GetSessionsDataHandles() const;

	bool EOS_CreateSession(const int32 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings);
	bool EOS_FindSessions(const int32 SearchingPlayerNum);
	bool EOS_JoinSession(const int32 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession);
	bool EOS_DestroySession();

protected:
	void OnSessionCreated(const FName SessionName, const bool bResult);
	void OnSessionsFound(const bool bResult);
	void OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
	void OnSessionDestroyed(const FName SessionName, const bool bResult);

private:
	TSharedPtr<FOnlineSessionSearch> EOSSearchSettings;

public:
	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultLogin(const int32 LocalUserNum, const FString Token, const bool bUsePortal = false);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FUserLoginDelegate UserLoginDelegate;

	/* Function created for testing only */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	bool DefaultLogout(const int32 LocalUserNum);

	UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
	FUserLogoutDelegate UserLogoutDelegate;

	bool EOS_Login(const int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials);
	bool EOS_Logout(const int32 LocalUserNum);

	void OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
	                     const FString& Error);
	void OnLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful);
};
