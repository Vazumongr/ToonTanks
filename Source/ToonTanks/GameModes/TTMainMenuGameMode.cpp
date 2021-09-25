// Fill out your copyright notice in the Description page of Project Settings.

#include "TTMainMenuGameMode.h"

#include "ToonTanks/PlayerStates/TTPlayerState.h"

void ATTMainMenuGameMode::SetPlayerStateUserName(FString UserName, APlayerState* InState)
{
	ATTPlayerState* PlayerState = Cast<ATTPlayerState>(InState);
	if(PlayerState != nullptr)
	{
		PlayerState->UserName = UserName;
		PlayerState->SetUserName(UserName);
		UE_LOG(LogTemp, Warning, TEXT("Playerstate user name is  : %s"), *PlayerState->UserName);
	}
}

void ATTMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ATTMainMenuGameMode::AddOnScreenMessage(const FString& InMessage)
{
    const FString DisplayMessage = FString::Printf(TEXT("DynamoDB: %s"), *InMessage);
    GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red, DisplayMessage);
}
