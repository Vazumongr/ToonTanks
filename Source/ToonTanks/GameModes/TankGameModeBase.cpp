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
    
}

void ATankGameModeBase::BeginPlay()
{
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerController = PlayerTank->GetController<ATTPlayerController>();

    HandleGameStart();
    
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
    AddPlayerScoreToLb();
}

int32 ATankGameModeBase::GetTargetTurretCount()
{
    TSubclassOf<APawnTurret> ClassToFind;
    ClassToFind = APawnTurret::StaticClass();
    TArray<AActor*> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, TurretActors);
    return TurretActors.Num();
}

void ATankGameModeBase::AddPlayerScoreToLb()
{
    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {
        
        const Aws::String table("HelloTable");
        const Aws::String region("us-east-2");
        const Aws::String name("PlayerOne");

        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = region;
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);

        Aws::DynamoDB::Model::PutItemRequest pir;
        pir.SetTableName(table);

        Aws::DynamoDB::Model::AttributeValue av;
        av.SetS(name);
        pir.AddItem("Name", av);
/*
        for (int x = 3; x < argc; x++)
        {
            const Aws::String arg(argv[x]);
            const Aws::Vector<Aws::String>& flds = Aws::Utils::StringUtils::Split(arg, '=');
            if (flds.size() == 2)
            {
                Aws::DynamoDB::Model::AttributeValue val;
                val.SetS(flds[1]);
                pir.AddItem(flds[0], val);
            }
            else
            {
                return;
            }
        }
*/
        const Aws::DynamoDB::Model::PutItemOutcome result = dynamoClient.PutItem(pir);
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


