// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/PlayerStates/TTPlayerState.h"

void ATTPlayerState::SetUserName(FString InUserName)
{
	UserName = InUserName;
	UE_LOG(LogTemp, Warning, TEXT("playerstate being called %s"), *UserName);
}
