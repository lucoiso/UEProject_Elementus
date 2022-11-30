// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEEOSLibrary.h"
#include <EOSVoiceChatUser.h>

FOnlineSubsystemEOS* UPEEOSLibrary::GetOnlineSubsystemEOS()
{
	return static_cast<FOnlineSubsystemEOS*>(FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM));
}

FEOSVoiceChatUser* UPEEOSLibrary::GetEOSVoiceChatUser(const uint8 LocalUserNum)
{
	if (FOnlineSubsystemEOS* const OnlineSubsystemEOS = GetOnlineSubsystemEOS())
	{
		if (const FUniqueNetIdPtr NetId = GetUniqueNetId(LocalUserNum, OnlineSubsystemEOS))
		{
			return OnlineSubsystemEOS->GetEOSVoiceChatUserInterface(*NetId.Get());
		}
	}

	return nullptr;
}

void UPEEOSLibrary::MuteEOSSessionVoiceChatUser(const int32 LocalUserNum, const bool bMute)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Mute: %d"), *FString(__func__), LocalUserNum, bMute);

	if (FEOSVoiceChatUser* const VoiceChatUserPtr = GetEOSVoiceChatUser(LocalUserNum))
	{
		VoiceChatUserPtr->SetAudioInputDeviceMuted(bMute);
	}
}

FString UPEEOSLibrary::GetEOSSessionOwningUserNameFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.Session.OwningUserName;
}

FString UPEEOSLibrary::GetEOSSessionIdFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.Session.GetSessionIdStr();
}

int32 UPEEOSLibrary::GetEOSSessionPingFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.PingInMs;
}

FName UPEEOSLibrary::GetEOSSessionName()
{
	return NAME_GameSession;
}

bool UPEEOSLibrary::IsUserLoggedInEOS(const int32 LocalUserNum)
{
	if (const IOnlineIdentityPtr IdentityInterface = GetIdentityInterface())
	{
		return IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UPEEOSLibrary::IsHostingEOSSession()
{
	if (const IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		if (const FNamedOnlineSession* const CheckSession = SessionInterface->GetNamedSession(NAME_GameSession))
		{
			return CheckSession->bHosting;
		}
	}

	return false;
}

bool UPEEOSLibrary::IsUserInAEOSSession()
{
	if (const IOnlineSessionPtr SessionInterface = GetSessionInterface())
	{
		return SessionInterface->GetNamedSession(NAME_GameSession) != nullptr;
	}

	return false;
}

FSessionSettingsHandler UPEEOSLibrary::GenerateEOSSessionSettings(const int32 NumPublicConnections, const int32 NumPrivateConnections, const bool bShouldAdvertise, const bool bAllowJoinInProgress, const bool bIsLANMatch, const bool bIsDedicated, const bool bUsesStats, const bool bAllowInvites, const bool bUsesPresence, const bool bAllowJoinViaPresence, const bool bAllowJoinViaPresenceFriendsOnly, const bool bAntiCheatProtected, const bool bUseLobbiesIfAvailable, const bool bUseLobbiesVoiceChatIfAvailable)
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.NumPublicConnections = NumPublicConnections;
	SessionSettings.NumPrivateConnections = NumPrivateConnections;
	SessionSettings.bShouldAdvertise = bShouldAdvertise;
	SessionSettings.bAllowJoinInProgress = bAllowJoinInProgress;
	SessionSettings.bIsLANMatch = bIsLANMatch;
	SessionSettings.bIsDedicated = bIsDedicated;
	SessionSettings.bUsesStats = bUsesStats;
	SessionSettings.bAllowInvites = bAllowInvites;
	SessionSettings.bUsesPresence = bUsesPresence;
	SessionSettings.bAllowJoinViaPresence = bAllowJoinViaPresence;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	SessionSettings.bAntiCheatProtected = bAntiCheatProtected;
	SessionSettings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;

	SessionSettings.Set(SEARCH_KEYWORDS, FString("ProjectElementus"), EOnlineDataAdvertisementType::ViaOnlineService);

	return FSessionSettingsHandler{SessionSettings};
}

void UPEEOSLibrary::UpdateEOSPresence(const int32 LocalUserNum, const FString& PresenceText, const bool bOnline)
{
	IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM);

	if (const IOnlineIdentityPtr IdentityInterface = GetIdentityInterface(OnlineSubsystem))
	{
		if (const IOnlinePresencePtr PresenceInterface = GetPresenceInterface(OnlineSubsystem))
		{
			FOnlineUserPresenceStatus NewStatus;
			NewStatus.Properties.Add(DefaultPresenceKey);
			NewStatus.State = bOnline ? EOnlinePresenceState::Online : EOnlinePresenceState::Offline;
			NewStatus.StatusStr = PresenceText;

			PresenceInterface->SetPresence(*IdentityInterface->GetUniquePlayerId(LocalUserNum).Get(), NewStatus);
		}
	}
}

void UPEEOSLibrary::IngestEOSStats(const int32 LocalUserNum, const TMap<FName, int32>& StatsMap)
{
	IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM);

	if (const IOnlineStatsPtr StatsInterface = GetStatsInterface(OnlineSubsystem))
	{
		const FUniqueNetIdRef UserNetIdRef = GetUniqueNetId(LocalUserNum, OnlineSubsystem).ToSharedRef();

		FOnlineStatsUserUpdatedStats UpdatedStats = FOnlineStatsUserUpdatedStats(UserNetIdRef);

		for (const auto& [StatName, StatValue] : StatsMap)
		{
			if (StatName.IsNone())
			{
				continue;
			}

			UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Stat Name: %s; Value: %d"), *FString(__func__), LocalUserNum, *StatName.ToString(), StatValue);

			const FOnlineStatUpdate StatUpdate(StatValue, FOnlineStatUpdate::EOnlineStatModificationType::Unknown);
			UpdatedStats.Stats.Add(StatName.ToString(), StatUpdate);
		}

		const FOnlineStatsUpdateStatsComplete UpdateStatsDelegate = FOnlineStatsUpdateStatsComplete::CreateLambda([UserNetIdRef, FuncName = __func__](const FOnlineError& ResultState)
		{
			UE_LOG(LogTemp, Log, TEXT("%s - User ID: %s; Result: %s"), *FString(__func__ ), *UserNetIdRef->ToString(), *ResultState.ToLogString());
		});

		StatsInterface->UpdateStats(UserNetIdRef, { UpdatedStats }, UpdateStatsDelegate);
	}
}

bool UPEEOSLibrary::ValidateOnlineSubsystem(IOnlineSubsystem* OnlineSubsystem)
{
	if (!OnlineSubsystem)
	{
		OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM);
	}

	return OnlineSubsystem != nullptr;
}

const FUniqueNetIdPtr UPEEOSLibrary::GetUniqueNetId(const int32 LocalUserNum, IOnlineSubsystem* OnlineSubsystem)
{
	if (const IOnlineIdentityPtr Interface = GetIdentityInterface(OnlineSubsystem))
	{
		return Interface->GetUniquePlayerId(LocalUserNum);
	}
	
	return nullptr;
}

const IOnlineIdentityPtr UPEEOSLibrary::GetIdentityInterface(IOnlineSubsystem* OnlineSubsystem)
{
	if (ValidateOnlineSubsystem(OnlineSubsystem))
	{
		return OnlineSubsystem->GetIdentityInterface();
	}
	
	return nullptr;
}

const IOnlineSessionPtr UPEEOSLibrary::GetSessionInterface(IOnlineSubsystem* OnlineSubsystem)
{
	if (ValidateOnlineSubsystem(OnlineSubsystem))
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	
	return nullptr;
}

const IOnlinePresencePtr UPEEOSLibrary::GetPresenceInterface(IOnlineSubsystem* OnlineSubsystem)
{
	if (ValidateOnlineSubsystem(OnlineSubsystem))
	{
		return OnlineSubsystem->GetPresenceInterface();
	}

	return nullptr;
}

const IOnlineStatsPtr UPEEOSLibrary::GetStatsInterface(IOnlineSubsystem* OnlineSubsystem)
{
	if (ValidateOnlineSubsystem(OnlineSubsystem))
	{
		return OnlineSubsystem->GetStatsInterface();
	}

	return nullptr;
}
