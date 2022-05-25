// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEGameInstance.h"
#include "OnlineSubsystemEOS.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"

void UPEGameInstance::Init()
{
	Super::Init();

	OnlineSubsystemEOS = MakeShareable(static_cast<FOnlineSubsystemEOS*>(FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM)));
}

void UPEGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UPEGameInstance::InitializeVoiceChatFramework()
{
	if (!IVoiceChat::Get()->IsInitialized())
	{
		OnVoiceChatInitializeCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatInitialized);
		IVoiceChat::Get()->Initialize(OnVoiceChatInitializeCompleteDelegate);
	}
}

void UPEGameInstance::UninitializeVoiceChatFramework()
{
	if (IVoiceChat::Get()->IsInitialized())
	{
		DisconnectVoiceChatFramework();

		for (auto& VoiceChatUser : EOSVoiceChatUsers)
		{
			if (VoiceChatUser.Value.IsValid())
			{
				IVoiceChat::Get()->ReleaseUser(VoiceChatUser.Value.Get());
				VoiceChatUser.Value.Reset();
			}
		}

		EOSVoiceChatUsers.Empty();

		OnVoiceChatUninitializeCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatUninitialized);
		IVoiceChat::Get()->Uninitialize(OnVoiceChatUninitializeCompleteDelegate);
	}
}

void UPEGameInstance::ConnectVoiceChatFramework()
{
	if (!IVoiceChat::Get()->IsConnected() && !IVoiceChat::Get()->IsConnecting())
	{
		OnVoiceChatConnectCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatConnected);
		IVoiceChat::Get()->Connect(OnVoiceChatConnectCompleteDelegate);
	}
}

void UPEGameInstance::DisconnectVoiceChatFramework()
{
	if (IVoiceChat::Get()->IsConnected())
	{
		OnVoiceChatDisconnectCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatDisconnected);
		IVoiceChat::Get()->Disconnect(OnVoiceChatDisconnectCompleteDelegate);
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
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface();
			IdentityInterface.IsValid()
			&& IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
		{
			if (const TSharedPtr<IVoiceChatUser> VoiceChatUser = MakeShareable(IVoiceChat::Get()->CreateUser());
				VoiceChatUser.IsValid())
			{
				EOSVoiceChatUsers.Add(LocalUserNum, VoiceChatUser);

				OnVoiceChatLoginCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatLogin);

				VoiceChatUser->Login(
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
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface();
			IdentityInterface.IsValid())
		{
			if (EOSVoiceChatUsers.FindRef(LocalUserNum).IsValid())
			{
				OnVoiceChatLogoutCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatLogout);
				EOSVoiceChatUsers.FindRef(LocalUserNum)->Logout(OnVoiceChatLogoutCompleteDelegate);

				IVoiceChat::Get()->ReleaseUser(EOSVoiceChatUsers.FindRef(LocalUserNum).Get());
				EOSVoiceChatUsers.FindRef(LocalUserNum).Reset();
				EOSVoiceChatUsers.Remove(LocalUserNum);
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

void UPEGameInstance::MuteVoiceChatUser(const int32 LocalUserNum, const bool bMute) const
{
	if (IVoiceChat::Get()->IsInitialized() && IVoiceChat::Get()->IsConnected())
	{
		UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Mute: %d"), *FString(__func__), LocalUserNum, bMute);

		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
		{
			if (IVoiceChatUser* ChatUser =
				OnlineSubsystemEOS->GetVoiceChatUserInterface(*IdentityInterface->GetUniquePlayerId(LocalUserNum)))
			{
				ChatUser->SetAudioInputDeviceMuted(bMute);
			}
		}
	}
}

void UPEGameInstance::ConnectVoiceChatToSessionChannel(const int32 LocalUserNum, const FString ChannelName,
                                                       const FEOSVoiceChatChannelCredentials Credentials)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum,
	       *ChannelName);

	if (EOSVoiceChatUsers.FindRef(LocalUserNum).IsValid())
	{
		OnVoiceChatChannelJoinCompleteDelegate.
			BindUObject(this, &UPEGameInstance::OnVoiceChatChannelJoined);

		FString CredentialsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Credentials, CredentialsJson);

		EOSVoiceChatUsers.FindRef(LocalUserNum)->JoinChannel(
			ChannelName,
			CredentialsJson,
			EVoiceChatChannelType::Echo,
			OnVoiceChatChannelJoinCompleteDelegate);
	}
}

void UPEGameInstance::LeaveVoiceChatSessionChannel(const int32 LocalUserNum, const FString ChannelName)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum,
	       *ChannelName);

	if (EOSVoiceChatUsers.FindRef(LocalUserNum).IsValid())
	{
		OnVoiceChatChannelLeaveCompleteDelegate.BindUObject(this, &UPEGameInstance::OnVoiceChatChannelLeft);

		EOSVoiceChatUsers.FindRef(LocalUserNum)->LeaveChannel(
			ChannelName,
			OnVoiceChatChannelLeaveCompleteDelegate);
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

bool UPEGameInstance::CreateDefaultSession(const bool bIsLAN, const bool bIsDedicated,
                                           const int32 NumOfPublicConnections)
{
	const TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->bIsLANMatch = bIsLAN;
	SessionSettings->bIsDedicated = bIsDedicated;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowInvites = true;
	SessionSettings->NumPublicConnections = NumOfPublicConnections;

	SessionSettings->Set(SEARCH_KEYWORDS, FString("ProjectElementus"), EOnlineDataAdvertisementType::ViaOnlineService);

	return EOS_CreateSession(0, *SessionSettings.Get());
}

bool UPEGameInstance::DefaultFindSessions(const int32 LocalUserNum, const bool bIsLANQuery, const int32 MaxResults)
{
	return EOS_FindSessions(LocalUserNum, bIsLANQuery, MaxResults);
}

bool UPEGameInstance::CancelFindSessions()
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->OnCancelFindSessionsCompleteDelegates.AddUObject(
				this, &UPEGameInstance::OnCancelFindSessions);

			return SessionInterface->CancelFindSessions();
		}
	}

	return false;
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

void UPEGameInstance::ServerTravelToLevel(const FName LevelName) const
{
	GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
}

void UPEGameInstance::ClientTravelToSessionLevel() const
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		IsValid(PlayerController) && !EOSCurrentSessionInfo.IsEmpty())
	{
		PlayerController->ClientTravel(EOSCurrentSessionInfo, TRAVEL_Absolute);
	}
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

bool UPEGameInstance::EOS_CreateSession(const int8 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionCreated);

			return SessionInterface->CreateSession(HostingPlayerNum, NAME_GameSession, NewSessionSettings);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_FindSessions(const int8 SearchingPlayerNum, const bool bIsLANQuery, const int8 MaxResults)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			EOSSearchSettings = MakeShareable(new FOnlineSessionSearch());
			EOSSearchSettings->bIsLanQuery = bIsLANQuery;
			EOSSearchSettings->MaxSearchResults = MaxResults;
			EOSSearchSettings->QuerySettings.Set(SEARCH_KEYWORDS,
			                                     FString("ProjectElementus"), EOnlineComparisonOp::Equals);

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionsFound);

			return SessionInterface->FindSessions(SearchingPlayerNum, EOSSearchSettings.ToSharedRef());
		}
	}

	return false;
}

bool UPEGameInstance::EOS_JoinSession(const int8 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPEGameInstance::OnSessionJoined);

			return SessionInterface->JoinSession(LocalUserNum, NAME_GameSession, DesiredSession);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_DestroySession()
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
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

	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
			SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
		}
	}

	CreateSessionDelegate.Broadcast();
}

void UPEGameInstance::OnSessionsFound(const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d - Sessions found: %d"), *FString(__func__), bResult,
	       EOSSearchSettings->SearchResults.Num());

	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
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

void UPEGameInstance::OnCancelFindSessions(const bool bResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Result: %d"), *FString(__func__), bResult);

	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->ClearOnCancelFindSessionsCompleteDelegates(this);
		}
	}

	CancelFindSessionsDelegate.Broadcast();
}

void UPEGameInstance::OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), Result);

	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
		{
			SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
			SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
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

	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
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

bool UPEGameInstance::DefaultLogin(const int32 LocalUserNum, const FString Token, const int32 Port,
                                   const bool bUsePortal)
{
	return EOS_Login(LocalUserNum,
	                 bUsePortal
		                 ? FOnlineAccountCredentials("AccountPortal", "", "")
		                 : FOnlineAccountCredentials("Developer", "localhost:" + FString::FromInt(Port), Token));
}

bool UPEGameInstance::DefaultLogout(const int32 LocalUserNum)
{
	return EOS_Logout(LocalUserNum);
}

bool UPEGameInstance::EOS_Login(const int8 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface();
			IdentityInterface.IsValid()
			&& IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::NotLoggedIn)
		{
			IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLoginComplete);

			return IdentityInterface->Login(LocalUserNum, AccountCredentials);
		}
	}

	return false;
}

bool UPEGameInstance::EOS_Logout(const int8 LocalUserNum)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface();
			IdentityInterface.IsValid()
			&& IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
		{
			IdentityInterface->OnLogoutCompleteDelegates->AddUObject(this, &UPEGameInstance::OnLogoutComplete);

			return IdentityInterface->Logout(LocalUserNum);
		}
	}

	return false;
}

bool UPEGameInstance::IsUserLoggedIn(const int32 LocalUserNum) const
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
		{
			return IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn;
		}
	}

	return false;
}

void UPEGameInstance::OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId,
                                      const FString& Error)
{
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);

			IdentityInterface->ClearOnLoginCompleteDelegates(LocalUserNum, this);

			if (const IOnlineSessionPtr SessionInterface = OnlineSubsystemEOS->GetSessionInterface())
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
	if (OnlineSubsystemEOS.IsValid())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__),
			       LocalUserNum, bWasSuccessful);

			IdentityInterface->ClearOnLogoutCompleteDelegates(LocalUserNum, this);

			OnSessionUserInviteAcceptedDelegate.Unbind();
		}
	}

	UserLogoutDelegate.Broadcast();
}
