// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

bool UPEGameInstance::CreateDefaultLocalSession(const bool bIsLAN, const bool bIsDedicated,
                                                const int32 NumOfPublicConnections)
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = bIsLAN;
	SessionSettings.bIsDedicated = bIsDedicated;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.NumPublicConnections = NumOfPublicConnections;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinViaPresence = true;

	SessionSettings.Set(SEARCH_KEYWORDS, FString("ProjectElementus"), EOnlineDataAdvertisementType::ViaOnlineService);

	return EOS_CreateSession(0, SessionSettings);
}

bool UPEGameInstance::DefaultFindSessions(const int32 LocalUserNum)
{
	return EOS_FindSessions(LocalUserNum);
}

bool UPEGameInstance::DefaultJoinSession(const int32 LocalUserNum, const FSessionDataHandle SessionData)
{
	return EOS_JoinSession(LocalUserNum, SessionData.Result);
}

bool UPEGameInstance::DefaultDestroySession()
{
	return EOS_DestroySession();
}

FString UPEGameInstance::GetSessionOwningUserNameFromHandle(const FSessionDataHandle DataHandle)
{
	return DataHandle.Result.Session.OwningUserName;
}

FString UPEGameInstance::GetSessionIdFromHandle(const FSessionDataHandle DataHandle)
{
	return DataHandle.Result.Session.GetSessionIdStr();
}

int32 UPEGameInstance::GetSessionPingFromHandle(const FSessionDataHandle DataHandle)
{
	return DataHandle.Result.PingInMs;
}

FName UPEGameInstance::GetGameSessionName()
{
	return NAME_GameSession;
}

TArray<FSessionDataHandle> UPEGameInstance::GetSessionsDataHandles() const
{
	TArray<FSessionDataHandle> SessionDataHandle_Arr;
	for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
	{
		SessionDataHandle_Arr.Add(FSessionDataHandle{SearchResult});
	}

	return SessionDataHandle_Arr;
}

bool UPEGameInstance::EOS_CreateSession(const int32 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionCreated);

			return SessionInterface->CreateSession(HostingPlayerNum, NAME_GameSession, NewSessionSettings);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_FindSessions(const int32 SearchingPlayerNum)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			EOSSearchSettings = MakeShareable(new FOnlineSessionSearch());
			EOSSearchSettings->MaxSearchResults = 100;
			EOSSearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("ProjectElementus"),
			                                     EOnlineComparisonOp::Equals);
			EOSSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionsFound);
			return SessionInterface->FindSessions(SearchingPlayerNum, EOSSearchSettings.ToSharedRef());
		}
	}

	return false;
}

bool UPEGameInstance::EOS_JoinSession(const int32 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionJoined);
			return SessionInterface->JoinSession(LocalUserNum, NAME_GameSession, DesiredSession);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_DestroySession()
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionDestroyed);
			return SessionInterface->DestroySession(NAME_GameSession, FOnDestroySessionCompleteDelegate());
		}
	}

	return false;
}

void UPEGameInstance::OnSessionCreated(const FName SessionName, const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(),
	       bResult);

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
			SessionInterface->StartSession(SessionName);
			CreateSessionDelegate.Broadcast();
		}
	}
}

void UPEGameInstance::OnSessionsFound(const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d - Sessions found: %d"), *FString(__func__), bResult,
	       EOSSearchSettings->SearchResults.Num());

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
		}
	}

	TArray<FSessionDataHandle> SessionDataHandle_Arr;

	for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
	{
		const FSessionDataHandle SessionDataHandle{SearchResult};

		UE_LOG(LogTemp, Warning, TEXT("Session Found: %s - %s"), *SearchResult.GetSessionIdStr(),
		       *SearchResult.Session.OwningUserName);

		SessionDataHandle_Arr.Add(SessionDataHandle);
	}

	FindSessionsDelegate.Broadcast(SessionDataHandle_Arr);
}

void UPEGameInstance::OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), Result);

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnJoinSessionCompleteDelegates(this);

			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				JoinSessionDelegate.Broadcast();

				FString ConnectionInfo;
				SessionInterface->GetResolvedConnectString(SessionName, ConnectionInfo);

				if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
					IsValid(PlayerController) && !ConnectionInfo.IsEmpty())
				{
					PlayerController->ClientTravel(ConnectionInfo, TRAVEL_Absolute);
				}
			}
		}
	}
}

void UPEGameInstance::OnSessionDestroyed(const FName SessionName, const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(),
	       bResult);

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegates(this);

			DestroySessionDelegate.Broadcast();
		}
	}
}

bool UPEGameInstance::DefaultLogin(const int32 LocalUserNum, const FString Token, const bool bUsePortal)
{
	return EOS_Login(LocalUserNum, bUsePortal
		                               ? FOnlineAccountCredentials("AccountPortal", "", "")
		                               : FOnlineAccountCredentials("Developer", "localhost:8303", Token));
}

bool UPEGameInstance::DefaultLogout(const int32 LocalUserNum)
{
	return EOS_Logout(LocalUserNum);
}

bool UPEGameInstance::EOS_Login(const int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineIdentityPtr OnlineIdentity = OnlineSubsystem->GetIdentityInterface())
		{
			OnlineIdentity->OnLoginCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLoginComplete);

			return OnlineIdentity->Login(LocalUserNum, AccountCredentials);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_Logout(const int32 LocalUserNum)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineIdentityPtr OnlineIdentity = OnlineSubsystem->GetIdentityInterface())
		{
			OnlineIdentity->OnLogoutCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLogoutComplete);

			return OnlineIdentity->Logout(LocalUserNum);
		}
	}

	return false;
}

void UPEGameInstance::OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
                                      const FString& Error)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineIdentityPtr OnlineIdentity = OnlineSubsystem->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);
			OnlineIdentity->ClearOnLoginCompleteDelegates(LocalUserNum, this);

			UserLoginDelegate.Broadcast();
		}
	}
}

void UPEGameInstance::OnLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful)
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (const IOnlineIdentityPtr OnlineIdentity = OnlineSubsystem->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);
			OnlineIdentity->ClearOnLogoutCompleteDelegates(LocalUserNum, this);

			UserLogoutDelegate.Broadcast();
		}
	}
}
