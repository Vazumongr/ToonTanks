// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"

ATankGameModeBase::ATankGameModeBase()
{
    
}

void ATankGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
    // Check what type of Actor died. If Turret, tally. if player -> go to lose condition
    
    UE_LOG(LogTemp, Warning, TEXT("An Actor Died: %s"), *DeadActor->GetName());
}

void ATankGameModeBase::HandleGameStart()
{
    // Get references and game win/lose conditions

    // Call handleGameStart to initialise the start countdown, turret activation, pawn check, etc.
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
    // See if player has destroyed all the turrets, show win result
    // else if turret destroyed player, show lose result.
    // Call blueprint version GameOver();
}
