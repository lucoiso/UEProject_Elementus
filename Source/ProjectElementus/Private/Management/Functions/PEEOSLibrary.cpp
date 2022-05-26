// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/Functions/PEEOSLibrary.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "EOSVoiceChatUser.h"

FOnlineSubsystemEOS* UPEEOSLibrary::GetOnlineSubsystemEOS()
{
	return static_cast<FOnlineSubsystemEOS*>(FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM));
}

FEOSVoiceChatUser* UPEEOSLibrary::GetEOSVoiceChatUser(const int8 LocalUserNum)
{
	if (FOnlineSubsystemEOS* OnlineSubsystemEOS = GetOnlineSubsystemEOS())
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

void UPEEOSLibrary::MuteSessionVoiceChatUser(const int32 LocalUserNum, const bool bMute)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Local User Num: %d; Mute: %d"), *FString(__func__), LocalUserNum, bMute);

	if (FEOSVoiceChatUser* VoiceChatUserPtr = GetEOSVoiceChatUser(LocalUserNum))
	{
		VoiceChatUserPtr->SetAudioInputDeviceMuted(bMute);
	}
}

FString UPEEOSLibrary::GetSessionOwningUserNameFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.Session.OwningUserName;
}

FString UPEEOSLibrary::GetSessionIdFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.Session.GetSessionIdStr();
}

int32 UPEEOSLibrary::GetSessionPingFromHandle(const FSessionDataHandler DataHandle)
{
	return DataHandle.Result.PingInMs;
}

FName UPEEOSLibrary::GetGameSessionName()
{
	return NAME_GameSession;
}

bool UPEEOSLibrary::IsUserLoggedIn(const int32 LocalUserNum)
{
	if (const IOnlineSubsystem* OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM))
	{
		if (const IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface())
		{
			return IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn;
		}
	}

	return false;
}

FSessionSettingsHandler UPEEOSLibrary::GenerateSessionSettings(const int32 NumPublicConnections,
                                                               const int32 NumPrivateConnections,
                                                               const bool bShouldAdvertise,
                                                               const bool bAllowJoinInProgress,
                                                               const bool bIsLANMatch, const bool bIsDedicated,
                                                               const bool bUsesStats, const bool bAllowInvites,
                                                               const bool bUsesPresence,
                                                               const bool bAllowJoinViaPresence,
                                                               const bool bAllowJoinViaPresenceFriendsOnly,
                                                               const bool bAntiCheatProtected,
                                                               const bool bUseLobbiesIfAvailable,
                                                               const bool bUseLobbiesVoiceChatIfAvailable)
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
