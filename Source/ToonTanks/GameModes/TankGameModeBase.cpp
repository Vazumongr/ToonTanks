// Fill out your copyright notice in the Description page of Project Settings.
#include "TankGameModeBase.h"

#include "aws/core/Aws.h"
#include "aws/core/utils/Outcome.h" 
#include "aws/dynamodb/DynamoDBClient.h"
#include "aws/dynamodb/model/AttributeDefinition.h"
#include "aws/dynamodb/model/PutItemRequest.h"
#include "aws/dynamodb/model/PutItemResult.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "ToonTanks/Controllers/TTPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATankGameModeBase::ATankGameModeBase()
{
    Http = &FHttpModule::Get();
}

void ATankGameModeBase::MyHttpCall()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ATankGameModeBase::OnResponseReceived);
    //This is the url on which to process the request
    Request->SetURL("https://httpbin.org/anything");
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}


void ATankGameModeBase::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogHttp, Warning, TEXT("Response Received..."));
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    //Deserialize the json data given Reader and the actual object to deserialize
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        //Get the value of the json object by field name
        //int32 recievedInt = JsonObject->GetIntegerField("customInt");
        TMap<FString, TSharedPtr<FJsonValue>> Values = JsonObject->Values;
        for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : Values)
        {
            FString JsonPair = FString::Printf(TEXT("%s: %s"), *pair.Key, *pair.Value->AsString());
            UE_LOG(LogHttp, Warning, TEXT("JsonPair: %s"), *JsonPair);
        }
        UE_LOG(LogHttp, Warning, TEXT("Response: %s"), *Response->GetContentAsString());
        //Output it to the engine
        //GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::FromInt(recievedInt));
    }
}

void ATankGameModeBase::BeginPlay()
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerController = PlayerTank->GetController<ATTPlayerController>();

    HandleGameStart();
    
    MyHttpCall();
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
        if(--TargetTurrets <= 0)
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
    float PlayersScore = PlayerController->GetScore();
    UE_LOG(LogTemp, Warning, TEXT("Score: %f"), PlayersScore);
    AddPlayerScoreToLb(PlayersScore);
}

int32 ATankGameModeBase::GetTargetTurretCount()
{
    TSubclassOf<APawnTurret> ClassToFind;
    ClassToFind = APawnTurret::StaticClass();
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, TurretActors);
    return TurretActors.Num();
}

void ATankGameModeBase::AddPlayerScoreToLb(float PlayersScore)
{
    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {
        
        const Aws::String score(std::to_string(PlayersScore).c_str());
        const Aws::String table("Scores");
        const Aws::String region("us-east-2");
        const Aws::String name("PlayerTwo");

        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = region;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        Aws::DynamoDB::Model::PutItemRequest PutItemRequest;
        PutItemRequest.SetTableName(table);

        Aws::DynamoDB::Model::AttributeValue AttributeValue;
        AttributeValue.SetS(name);
        PutItemRequest.AddItem("Username", AttributeValue);
        AttributeValue.SetN(score);
        PutItemRequest.AddItem("Score", AttributeValue);
        
        const Aws::DynamoDB::Model::PutItemOutcome result = dynamoClient.PutItem(PutItemRequest);
        if (!result.IsSuccess())
        {
            std::cout << result.GetError().GetMessage() << std::endl;
            FString ErrorMsg = FString::Printf(TEXT("Error: %s"), *FString(result.GetError().GetMessage().c_str()));
            UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
            return;
        }
        UE_LOG(LogTemp, Warning, TEXT("Database Update Complete!"));
        std::cout << "Done!" << std::endl;
    }
    Aws::ShutdownAPI(options);
}


