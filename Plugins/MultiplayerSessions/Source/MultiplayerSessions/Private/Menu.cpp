// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "MultiplayerSessionsSubsystem.h"
void UMenu::MenuSetup(int32 MaxConnectionsNumSetup, FString MatchtypeSetup, FString LobbyPathSetup)
{
	LobbyPath = FString::Printf(TEXT("%s?listen"),*LobbyPathSetup);
	MaxConnectionsNum = MaxConnectionsNumSetup;
	MatchType = MatchtypeSetup;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionsComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}



bool UMenu::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this,&ThisClass::HostButtonClicked);
	}
	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully!"))
			);
		}
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(LobbyPath);
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created Fail!"))
			);
		}
		HostButton->SetIsEnabled(true);
	}



}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Join Session Failed"));
		JoinButton->SetIsEnabled(true);
		return;
	}
	IOnlineSubsystem* OnlineSubsyetem = IOnlineSubsystem::Get();
	if (OnlineSubsyetem) {
		IOnlineSessionPtr SessionInterface = OnlineSubsyetem->GetSessionInterface();
		if (SessionInterface) {
			FString Address;
			if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, Address) || Address.IsEmpty())
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to resolve connect string"));
				JoinButton->SetIsEnabled(true);
				return;
			}
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Address %s"),*Address) );
			}
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
		else {
			JoinButton->SetIsEnabled(true);
		}
	}
	else {
		JoinButton->SetIsEnabled(true);
	}
	
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResult, bool bWasSuccessful)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Menu: OnFindSessionComplate"))
		);
	}
	if (MultiplayerSessionsSubsystem == nullptr) {
		JoinButton->SetIsEnabled(true);
		return;
	}
	for (auto Result : SearchResult) {
		FString SettingValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);
		if (SettingValue == MatchType) {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Blue,
					FString(TEXT("Menu: FindNeedSession"))
				);
			}
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (bWasSuccessful == false || SearchResult.Num()<=0) {
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(MaxConnectionsNum,MatchType);
		
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSession(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
