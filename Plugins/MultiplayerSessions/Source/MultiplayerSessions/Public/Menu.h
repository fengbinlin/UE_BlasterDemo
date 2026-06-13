// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 MaxConnectionsNumSetup=4, FString Matchtype=FString(TEXT("FreeForAll")), FString LobbyPathSetup=FString(TEXT("/Game/ThirdPerson/Lobby")));
 
protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//MultiplayerSessionsSubsystem»Øµ÷º¯Êý
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResult, bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta=(BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();


	//SessionSubsystem
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 MaxConnectionsNum{4};
	FString MatchType{TEXT("FreeForAll")};

	FString LobbyPath{TEXT("") };

};
