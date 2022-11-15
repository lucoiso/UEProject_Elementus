// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEEOSLibrary.h"
#include "EOSVoiceChatUser.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

FOnlineSubsystemEOS* UPEEOSLibrary::GetOnlineSubsystemEOS()
{
	return static_cast<FOnlineSubsystemEOS*>(FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM));
}

FEOSVoiceChatUser* UPEEOSLibrary::GetEOSVoiceChatUser(const uint8 LocalUserNum)
{
	if (FOnlineSubsystemEOS* const OnlineSubsystemEOS = GetOnlineSubsystemEOS())
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystemEOS->GetIdentityInterface())
		{
			if (const FUniqueNetIdPtr NetId = IdentityInterface->GetUniquePlayerId(LocalUserNum))
			{
				return OnlineSubsystemEOS->GetEOSVoiceChatUserInterface(*NetId.Get());
			}
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
	if (const IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			return IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn;
		}
	}

	return false;
}

bool UPEEOSLibrary::IsHostingEOSSession()
{
	if (const IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			if (const FNamedOnlineSession* const CheckSession = SessionInterface->GetNamedSession(NAME_GameSession))
			{
				return CheckSession->bHosting;
			}
		}
	}

	return false;
}

bool UPEEOSLibrary::IsUserInAEOSSession()
{
	if (const IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			return SessionInterface->GetNamedSession(NAME_GameSession) != nullptr;
		}
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
	if (const IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			if (const IOnlinePresencePtr PresenceInterface = OnlineSubsystem->GetPresenceInterface())
			{
				FOnlineUserPresenceStatus NewStatus;
				NewStatus.Properties.Add(DefaultPresenceKey);
				NewStatus.State = bOnline ? EOnlinePresenceState::Online : EOnlinePresenceState::Offline;
				NewStatus.StatusStr = PresenceText;

				PresenceInterface->SetPresence(*IdentityInterface->GetUniquePlayerId(LocalUserNum).Get(), NewStatus);
			}
		}
	}
}

void UPEEOSLibrary::IngestEOSStats(const int32 LocalUserNum, const TMap<FName, int32> StatsMap)
{
	if (const IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			if (const IOnlineStatsPtr StatsInterface = OnlineSubsystem->GetStatsInterface())
			{
				const FUniqueNetIdRef UserNetIdRef = IdentityInterface->GetUniquePlayerId(LocalUserNum).ToSharedRef();
				
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

				const FOnlineStatsUpdateStatsComplete UpdateStatsDelegate = FOnlineStatsUpdateStatsComplete::CreateLambda([UserNetIdRef](const FOnlineError& ResultState)
				{
					UE_LOG(LogTemp, Log, TEXT("IngestEOSStats - User ID: %s; Result: %s"), *UserNetIdRef->ToString(), *ResultState.ToLogString());
				});
				
				StatsInterface->UpdateStats(UserNetIdRef, { UpdatedStats }, UpdateStatsDelegate);
			}
		}
	}
}