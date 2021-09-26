// Fill out your copyright notice in the Description page of Project Settings.
#include "TTTankGameModeBase.h"

#include "ToonTanks/Pawns/TTPawnTank.h"
#include "ToonTanks/Pawns/TTPawnTurret.h"
#include "ToonTanks/Controllers/TTPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/GameInstances/TTGameInstance.h"
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

ATTTankGameModeBase::ATTTankGameModeBase()
{
    Http = &FHttpModule::Get();
}

void ATTTankGameModeBase::ScanLeaderboard()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ATTTankGameModeBase::OnResponseReceived);
    //This is the url on which to process the request
    Request->SetURL(APILINK);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void ATTTankGameModeBase::AddScoreToLeaderboard(float PlayersScore)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    UTTGameInstance* GameInstance = Cast<UTTGameInstance>(GetGameInstance());
    
    if(GameInstance == nullptr) return;
    
    const FString PlayerUsername = GameInstance->GetPlayerUsername();
    JsonObject->SetStringField("Username", PlayerUsername);
    JsonObject->SetNumberField("Score", PlayersScore);
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ATTTankGameModeBase::OnResponseReceived);
    //This is the url on which to process the request
    Request->SetURL(APILINK);
    Request->SetVerb("PUT");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->SetContentAsString(OutputString);
    Request->ProcessRequest();
    InsertRequest = Request;
}

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


void ATTTankGameModeBase::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if(Request == InsertRequest)
    {
        UE_LOG(LogHttp, Warning, TEXT("Our insert finished!"));
    }
}

void ATTTankGameModeBase::BeginPlay()
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<ATTPawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerController = PlayerTank->GetController<ATTPlayerController>();

    HandleGameStart();
    ScanLeaderboard();
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
    //AddScoreToLeaderboard(PlayersScore);
}

int32 ATTTankGameModeBase::GetTargetTurretCount()
{
    TSubclassOf<ATTPawnTurret> ClassToFind;
    ClassToFind = ATTPawnTurret::StaticClass();
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, TurretActors);
    return TurretActors.Num();
}



