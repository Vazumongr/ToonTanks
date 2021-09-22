// Fill out your copyright notice in the Description page of Project Settings.
#include "TankGameModeBase.h"

#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "ToonTanks/Controllers/TTPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/GameInstances/TTGameInstance.h"

ATankGameModeBase::ATankGameModeBase()
{
    Http = &FHttpModule::Get();
}

void ATankGameModeBase::ScanLeaderboard()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ATankGameModeBase::OnResponseReceived);
    //This is the url on which to process the request
    Request->SetURL(APILINK);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void ATankGameModeBase::AddScoreToLeaderboard(float PlayersScore)
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
    Request->OnProcessRequestComplete().BindUObject(this, &ATankGameModeBase::OnResponseReceived);
    //This is the url on which to process the request
    Request->SetURL(APILINK);
    Request->SetVerb("PUT");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->SetContentAsString(OutputString);
    Request->ProcessRequest();
    InsertRequest = Request;
}


void ATankGameModeBase::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if(Request == InsertRequest)
    {
        UE_LOG(LogHttp, Warning, TEXT("Our insert finished!"));
        ShowEndGameMenu(bPlayerWon);
    }
}

void ATankGameModeBase::BeginPlay()
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerController = PlayerTank->GetController<ATTPlayerController>();

    HandleGameStart();
    
    ScanLeaderboard();

    UTTGameInstance* GameInstance = Cast<UTTGameInstance>(GetGameInstance());
    
    if(GameInstance == nullptr) return;
    
    const FString PlayerUsername = GameInstance->GetPlayerUsername();
    if(PlayerUsername == "DevTest")
    {
        bDevTest = true;
    }
    Super::BeginPlay();
}

void ATankGameModeBase::ActorDied(AActor* DeadActor)
{
    if(DeadActor == PlayerTank)
    {
        PlayerTank->PawnDestroyed();
        HandleGameOver(false);
    }
    else if(APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
    {
        DestroyedTurret->PawnDestroyed();
        AddScore(DeadActor);
        if(--TargetTurrets <= 0 || bDevTest)
        {
            HandleGameOver(true);
        }
    }
}

void ATankGameModeBase::AddScore(AActor* KilledActor)
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

    UE_LOG(LogTemp, Warning, TEXT("KilledActor class %s is found in the Target Score Table with a score of %d!"), *KilledActor->GetClass()->GetName(), *TargetScore);
    FString Message = (FString::Printf(TEXT("KilledActor class %s is found in the Target Score Table with a score of %f!"), *KilledActor->GetClass()->GetName(), *TargetScore));
    GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Yellow,Message);

    //PlayerController = PlayerTank->GetController<ATTPlayerController>();
    if(PlayerController != nullptr)
        PlayerController->AddScore(*TargetScore);
}

void ATankGameModeBase::HandleGameStart()
{
    GameStart();
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon)
{
    GameOver(PlayerWon);
    bPlayerWon = PlayerWon;
    float PlayersScore = PlayerController->GetScore();
    UE_LOG(LogTemp, Warning, TEXT("Score: %f"), PlayersScore);
    AddScoreToLeaderboard(PlayersScore);
}

int32 ATankGameModeBase::GetTargetTurretCount()
{
    TSubclassOf<APawnTurret> ClassToFind;
    ClassToFind = APawnTurret::StaticClass();
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, TurretActors);
    return TurretActors.Num();
}



