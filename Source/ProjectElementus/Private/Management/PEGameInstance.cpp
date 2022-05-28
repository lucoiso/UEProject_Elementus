// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEGameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "EOSVoiceChatUser.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"


void UPEGameInstance::InitializeVoiceChatFramework()
{
	if (!FEOSVoiceChat::Get()->IsInitialized())
	{
		OnVoiceChatInitializeCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatInitialized);
		FEOSVoiceChat::Get()->Initialize(OnVoiceChatInitializeCompleteDelegate);
	}
}

void UPEGameInstance::ShutdownVoiceChatFramework()
{
	if (FEOSVoiceChat::Get()->IsInitialized())
	{
		DisconnectVoiceChatFramework();

		if (FOnlineSubsystemEOS* OnlineSubsystemEOS = UPEEOSLibrary::GetOnlineSubsystemEOS())
		{
			if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
			{
				for (int Iterator = 0; Iterator < UGameplayStatics::GetNumLocalPlayerControllers(GetWorld()); ++
				     Iterator)
				{
					if (const FUniqueNetIdPtr NetId = IdentityInterface->GetUniquePlayerId(Iterator))
					{
						OnlineSubsystemEOS->ReleaseVoiceChatUserInterface(*NetId.Get());
					}
				}
			}
		}

		OnVoiceChatUninitializeCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatUninitialized);
		FEOSVoiceChat::Get()->Uninitialize(OnVoiceChatUninitializeCompleteDelegate);
	}
}

void UPEGameInstance::ConnectVoiceChatFramework()
{
	if (!FEOSVoiceChat::Get()->IsConnected() && !FEOSVoiceChat::Get()->IsConnecting())
	{
		OnVoiceChatConnectCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatConnected);
		FEOSVoiceChat::Get()->Connect(OnVoiceChatConnectCompleteDelegate);
	}
}

void UPEGameInstance::DisconnectVoiceChatFramework()
{
	if (FEOSVoiceChat::Get()->IsConnected())
	{
		OnVoiceChatDisconnectCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatDisconnected);
		FEOSVoiceChat::Get()->Disconnect(OnVoiceChatDisconnectCompleteDelegate);
	}
}

void UPEGameInstance::OnVoiceChatInitialized(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())

		ConnectVoiceChatFramework();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__),
		       Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatInitializeCompleteDelegate.Unbind();
}

void UPEGameInstance::OnVoiceChatUninitialized(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__),
		       Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatUninitializeCompleteDelegate.Unbind();
}

void UPEGameInstance::OnVoiceChatConnected(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__),
		       Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatConnectCompleteDelegate.Unbind();
}

void UPEGameInstance::OnVoiceChatDisconnected(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__),
		       Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatDisconnectCompleteDelegate.Unbind();
}

void UPEGameInstance::LoginToVoiceChatFramework(const int32 LocalUserNum)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
			IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
		{
			if (FEOSVoiceChatUser* VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
			{
				OnVoiceChatLoginCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatLogin);

				VoiceChatUserPtr->Login(
					IdentityInterface->GetPlatformUserIdFromLocalUserNum(LocalUserNum),
					IdentityInterface->GetPlayerNickname(LocalUserNum),
					IdentityInterface->GetAuthToken(LocalUserNum),
					OnVoiceChatLoginCompleteDelegate);
			}
		}
	}
}

void UPEGameInstance::LogoutFromVoiceChatFramework(const int32 LocalUserNum)
{
	if (FOnlineSubsystemEOS* OnlineSubsystemEOS = UPEEOSLibrary::GetOnlineSubsystemEOS())
	{
		if (FEOSVoiceChatUser* VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
		{
			OnVoiceChatLogoutCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatLogout);
			VoiceChatUserPtr->Logout(OnVoiceChatLogoutCompleteDelegate);

			if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
			{
				if (const FUniqueNetIdPtr NetId = IdentityInterface->GetUniquePlayerId(LocalUserNum))
				{
					OnlineSubsystemEOS->ReleaseVoiceChatUserInterface(*NetId.Get());
				}
			}
		}
	}
}

void UPEGameInstance::OnVoiceChatLogin(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Player Name: %s; Result: %d"),
		       *FString(__func__), *PlayerName, Result.IsSuccess())

		ConnectVoiceChatFramework();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Player Name: %s; Result: %d - Error Code: %s; Error Description: %s"),
		       *FString(__func__), Result.IsSuccess(), *PlayerName, *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatLoginCompleteDelegate.Unbind();
}

void UPEGameInstance::OnVoiceChatLogout(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Player Name: %s; Result: %d"),
		       *FString(__func__), *PlayerName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Player Name: %s; Result: %d - Error Code: %s; Error Description: %s"),
		       *FString(__func__), Result.IsSuccess(), *PlayerName, *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatLogoutCompleteDelegate.Unbind();
}

void UPEGameInstance::ConnectVoiceChatToSessionChannel(const int32 LocalUserNum, const FString ChannelName,
                                                       const FEOSVoiceChatChannelCredentials Credentials)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum,
	       *ChannelName);

	if (FEOSVoiceChatUser* VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
	{
		OnVoiceChatChannelJoinCompleteDelegate.
			BindUObject(this, &UPEGameInstance::OnVoiceChatChannelJoined);

		FString CredentialsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Credentials, CredentialsJson);

		VoiceChatUserPtr->JoinChannel(
			ChannelName,
			CredentialsJson,
			EVoiceChatChannelType::Echo, // Testing Only
			OnVoiceChatChannelJoinCompleteDelegate);
	}
}

void UPEGameInstance::LeaveVoiceChatSessionChannel(const int32 LocalUserNum, const FString ChannelName)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum,
	       *ChannelName);

	if (FEOSVoiceChatUser* VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
	{
		OnVoiceChatChannelLeaveCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatChannelLeft);
		VoiceChatUserPtr->LeaveChannel(ChannelName, OnVoiceChatChannelLeaveCompleteDelegate);
	}
}

void UPEGameInstance::OnVoiceChatChannelJoined(const FString& ChannelName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Channel Name: %s; Result: %d"),
		       *FString(__func__), *ChannelName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Channel Name: %s; Result: %d - Error Code: %s; Error Description: %s"),
		       *FString(__func__), *ChannelName, Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatChannelJoinCompleteDelegate.Unbind();
}

void UPEGameInstance::OnVoiceChatChannelLeft(const FString& ChannelName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Channel Name: %s; Result: %d"),
		       *FString(__func__), *ChannelName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Channel Name: %s; Result: %d - Error Code: %s; Error Description: %s"),
		       *FString(__func__), *ChannelName, Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatChannelLeaveCompleteDelegate.Unbind();
}

bool UPEGameInstance::CreateEOSSession(const FSessionSettingsHandler SessionSettings)
{
	return EOS_CreateSession(0, SessionSettings.Settings);
}

bool UPEGameInstance::FindEOSSessions(const int32 LocalUserNum, const bool bIsLANQuery, const int32 MaxResults)
{
	return EOS_FindSessions(LocalUserNum, bIsLANQuery, MaxResults);
}

bool UPEGameInstance::CancelFindEOSSessions()
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->OnCancelFindSessionsCompleteDelegates.AddUObject(
				this, &UPEGameInstance::OnCancelFindSessions);

			return SessionInterface->CancelFindSessions();
		}
	}

	return false;
}

bool UPEGameInstance::JoinEOSSession(const int32 LocalUserNum, const FSessionDataHandler SessionData)
{
	return EOS_JoinSession(LocalUserNum, SessionData.Result);
}

bool UPEGameInstance::DestroyEOSSession()
{
	return EOS_DestroySession();
}

void UPEGameInstance::ServerTravelToLevel(const FName LevelName) const
{
	GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
}

void UPEGameInstance::ClientTravelToSessionLevel(const int32 LocalUserNum) const
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), LocalUserNum);
		IsValid(PlayerController) && !EOSCurrentSessionInfo.IsEmpty())
	{
		PlayerController->ClientTravel(EOSCurrentSessionInfo, TRAVEL_Absolute);
	}
}

TArray<FSessionDataHandler> UPEGameInstance::GetSessionsDataHandles() const
{
	TArray<FSessionDataHandler> SessionDataHandle_Arr;
	for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
	{
		SessionDataHandle_Arr.Add(FSessionDataHandler{SearchResult});
	}

	return SessionDataHandle_Arr;
}

bool UPEGameInstance::EOS_CreateSession(const int8 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionCreated);

			return SessionInterface->CreateSession(HostingPlayerNum, NAME_GameSession, NewSessionSettings);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_FindSessions(const int8 SearchingPlayerNum, const bool bIsLANQuery, const int8 MaxResults)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			EOSSearchSettings = MakeShareable(new FOnlineSessionSearch());
			EOSSearchSettings->bIsLanQuery = bIsLANQuery;
			EOSSearchSettings->MaxSearchResults = MaxResults;
			EOSSearchSettings->QuerySettings.Set(SEARCH_KEYWORDS,
			                                     FString("ProjectElementus"), EOnlineComparisonOp::Equals);
			EOSSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionsFound);

			return SessionInterface->FindSessions(SearchingPlayerNum, EOSSearchSettings.ToSharedRef());
		}
	}

	return false;
}

bool UPEGameInstance::EOS_JoinSession(const int8 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
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
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
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

	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
			SessionInterface->ClearOnCreateSessionCompleteDelegates(this);

			EnableListenServer(true);
		}
	}

	CreateSessionDelegate.Broadcast();
}

void UPEGameInstance::OnSessionsFound(const bool bResult)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
		}
	}

	if (EOSSearchSettings.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d - Sessions found: %d"), *FString(__func__), bResult,
		       EOSSearchSettings->SearchResults.Num());

		TArray<FSessionDataHandler> SessionDataHandle_Arr;

		for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
		{
			const FSessionDataHandler SessionDataHandle{SearchResult};

			UE_LOG(LogTemp, Warning, TEXT("Session Found: %s - %s"), *SearchResult.GetSessionIdStr(),
			       *SearchResult.Session.OwningUserName);

			SessionDataHandle_Arr.Add(SessionDataHandle);
		}

		FindSessionsDelegate.Broadcast(SessionDataHandle_Arr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d - No sessions found"), *FString(__func__), bResult);
	}
}

void UPEGameInstance::OnCancelFindSessions(const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d"), *FString(__func__), bResult);

	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnCancelFindSessionsCompleteDelegates(this);
		}
	}

	CancelFindSessionsDelegate.Broadcast();

	if (bResult)
	{
		EOSSearchSettings.Reset();
	}
}

void UPEGameInstance::OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), Result);

	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
			SessionInterface->ClearOnJoinSessionCompleteDelegates(this);

			EnableListenServer(false);
		}
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		JoinSessionDelegate.Broadcast();
	}
}

void UPEGameInstance::OnSessionDestroyed(const FName SessionName, const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(),
	       bResult);

	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
		}
	}

	DestroySessionDelegate.Broadcast();
}

void UPEGameInstance::OnSessionInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
                                              const FUniqueNetIdPtr UserId,
                                              const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("User %s accepted invite to session %s"), *UserId->ToString(),
		       *InviteResult.GetSessionIdStr());

		EOS_JoinSession(LocalUserNum, InviteResult);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("User %s failed to accept invite to session %s"),
		       *UserId->ToString(), *InviteResult.GetSessionIdStr());
	}
}

bool UPEGameInstance::EOSLogin(const int32 LocalUserNum, const FString Token, const int32 Port,
                               const bool bUsePortal)
{
	return EOS_Login(LocalUserNum,
	                 bUsePortal
		                 ? FOnlineAccountCredentials("AccountPortal", "", "")
		                 : FOnlineAccountCredentials("Developer", "localhost:" + FString::FromInt(Port), Token));
}

bool UPEGameInstance::EOSLogout(const int32 LocalUserNum)
{
	return EOS_Logout(LocalUserNum);
}

bool UPEGameInstance::EOS_Login(const int8 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
			IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::NotLoggedIn)
		{
			IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLoginComplete);

			return IdentityInterface->Login(LocalUserNum, AccountCredentials);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_Logout(const int8 LocalUserNum)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
			IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
		{
			IdentityInterface->OnLogoutCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLogoutComplete);

			return IdentityInterface->Logout(LocalUserNum);
		}
	}

	return false;
}

void UPEGameInstance::OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
                                      const FString& Error)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);

			IdentityInterface->ClearOnLoginCompleteDelegates(LocalUserNum, this);

			if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
			{
				OnSessionUserInviteAcceptedDelegate.BindUObject(this, &UPEGameInstance::OnSessionInviteAccepted);
				SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
			}
		}
	}

	UserLoginDelegate.Broadcast();
}

void UPEGameInstance::OnLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);

			IdentityInterface->ClearOnLogoutCompleteDelegates(LocalUserNum, this);

			OnSessionUserInviteAcceptedDelegate.Unbind();
		}
	}

	UserLogoutDelegate.Broadcast();
}
