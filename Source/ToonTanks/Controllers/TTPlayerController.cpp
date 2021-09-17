﻿#include "TTPlayerController.h"
#include "ToonTanks/PlayerStates/TTPlayerState.h"

void ATTPlayerController::Tick(float DeltaSeconds)
{
	ATTPlayerState* TTPS = Cast<ATTPlayerState>(PlayerState);
	FString Name = PlayerState->GetPlayerName();
	FString Message = (FString::Printf(TEXT("%s score: %f"), *TTPS->UserName, Score));
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Emerald,Message);
}

void ATTPlayerController::AddScore(float InScore)
{
	Score += InScore;
}