// Fill out your copyright notice in the Description page of Project Settings.
#include "TTTankGameModeBase.h"

#include "ToonTanks/Pawns/TTPawnTank.h"
#include "ToonTanks/Pawns/TTPawnTurret.h"
#include "ToonTanks/Controllers/TTPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/GameInstances/TTGameInstance.h"
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

void ATTTankGameModeBase::SetUsername()
{
    UTTGameInstance* GameInstance = Cast<UTTGameInstance>(GetGameInstance());
    
    if(GameInstance == nullptr) return;
    
    const FString PlayerUsername = GameInstance->GetPlayerUsername();
    if(PlayerUsername == "DevTest")
    {
        bDevTest = true;
    }
}

void ATTTankGameModeBase::BeginPlay()
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<ATTPawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerController = PlayerTank->GetController<ATTPlayerController>();

    HandleGameStart();
    SetUsername();
    
    Super::BeginPlay();
}

void ATTTankGameModeBase::ActorDied(AActor* DeadActor)
{
    if(DeadActor == PlayerTank)
    {
        PlayerTank->PawnDestroyed();
        HandleGameOver(false);
    }
    else if(ATTPawnTurret* DestroyedTurret = Cast<ATTPawnTurret>(DeadActor))
    {
        DestroyedTurret->PawnDestroyed();
        AddScore(DeadActor);
        if(--TargetTurrets <= 0 || bDevTest)
        {
            HandleGameOver(true);
        }
    }
}

void ATTTankGameModeBase::AddScore(AActor* KilledActor)
{
    if(KilledActor == nullptr)
    {
        return;
    }
    float* TargetScore = TargetValues.Find(KilledActor->GetClass());
    if(TargetScore == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("KilledActor class %s does not exist within the Target Score Table! Please fix inside of the game mode!"), *KilledActor->GetClass()->GetName());
        return;
    }
    if(PlayerController != nullptr)
        PlayerController->AddScore(*TargetScore);
}

void ATTTankGameModeBase::HandleGameStart()
{
    GameStart();
}

void ATTTankGameModeBase::HandleGameOver(bool PlayerWon)
{
    GameOver(PlayerWon);
    ShowEndGameMenu(PlayerWon);
    float PlayersScore = PlayerController->GetScore();
    UGameInstance* GameInstance = GetGameInstance();
    if(GameInstance != nullptr)
    {
        UTTDatabaseAgent* DatabaseAgent = GameInstance->GetSubsystem<UTTDatabaseAgent>();
        if(DatabaseAgent != nullptr)
        {
            DatabaseAgent->AddScoreToLeaderboard(PlayersScore);
        }
    }
}

int32 ATTTankGameModeBase::GetTargetTurretCount()
{
    TSubclassOf<ATTPawnTurret> ClassToFind;
    ClassToFind = ATTPawnTurret::StaticClass();
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, TurretActors);
    return TurretActors.Num();
}



