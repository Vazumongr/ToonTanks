// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuGameMode.h"

#include "ToonTanks/PlayerStates/TTPlayerState.h"

#include "aws/core/Aws.h"
#include "aws/core/utils/Outcome.h" 
#include "aws/dynamodb/DynamoDBClient.h"
#include "aws/dynamodb/model/AttributeDefinition.h"
#include "aws/dynamodb/model/CreateTableRequest.h"
#include "aws/dynamodb/model/KeySchemaElement.h"
#include "aws/dynamodb/model/ProvisionedThroughput.h"
#include "aws/dynamodb/model/ScalarAttributeType.h"
#include "aws/dynamodb/model/ScanRequest.h"
#include "aws/dynamodb/model/ListTablesRequest.h"
#include "aws/dynamodb/model/ListTablesResult.h"

void AMainMenuGameMode::SetPlayerStateUserName(FString UserName, APlayerState* InState)
{
	ATTPlayerState* PlayerState = Cast<ATTPlayerState>(InState);
	if(PlayerState != nullptr)
	{
		PlayerState->UserName = UserName;
		PlayerState->SetUserName(UserName);
		UE_LOG(LogTemp, Warning, TEXT("Playerstate user name is  : %s"), *PlayerState->UserName);
	}
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
    MakeTable();
    ListTables();
}

void AMainMenuGameMode::MakeTable()
{
	const std::string USAGE = "\n" \
        "Usage:\n"
        "    create_table <table> <optional:region>\n\n"
        "Where:\n"
        "    table - the table to create\n"
        "    region- optional region\n\n"
        "Example:\n"
        "    create_table HelloTable us-west-2\n";

    FString Message = FString(USAGE.c_str());
    AddOnScreenMessage(Message);
    
/*
    if (argc < 2)
    {
        std::cout << USAGE;
        return 1;
    }
*/
    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {
        const Aws::String table("HelloTable");
        const Aws::String region("us-west-2");

        // snippet-start:[dynamodb.cpp.create_table.code]
        Aws::Client::ClientConfiguration clientConfig;
	    Client = clientConfig;
	    
        if (!region.empty())
            clientConfig.region = region;
	    
        Aws::DynamoDB::DynamoDBClient dynamoClient(Client);

        std::cout << "Creating table " << table <<
            " with a simple primary key: \"Name\"" << std::endl;

	    //FString::Printf(TEXT("Creating table %s with a simple primary key \"Name\""), *table.c_str());
	    AddOnScreenMessage(FString::Printf(TEXT("Creating table %s with a simple primary key \"Name\""), *FString(table.c_str())));

	    

        Aws::DynamoDB::Model::CreateTableRequest req;

        Aws::DynamoDB::Model::AttributeDefinition haskKey;
        haskKey.SetAttributeName("Name");
        haskKey.SetAttributeType(Aws::DynamoDB::Model::ScalarAttributeType::S);
        req.AddAttributeDefinitions(haskKey);

        Aws::DynamoDB::Model::KeySchemaElement keyscelt;
        keyscelt.WithAttributeName("Name").WithKeyType(Aws::DynamoDB::Model::KeyType::HASH);
        req.AddKeySchema(keyscelt);

        Aws::DynamoDB::Model::ProvisionedThroughput thruput;
        thruput.WithReadCapacityUnits(5).WithWriteCapacityUnits(5);
        req.SetProvisionedThroughput(thruput);

        req.SetTableName(table);

        const Aws::DynamoDB::Model::CreateTableOutcome& result = dynamoClient.CreateTable(req);
        if (result.IsSuccess())
        {
            //std::cout << "Table \"" << result.GetResult().GetTableDescription().GetTableName() <<
//                " created!" << std::endl;
            AddOnScreenMessage(FString::Printf(TEXT("Table \" %s \" created!"), *FString(result.GetResult().GetTableDescription().GetTableName().c_str())));
        }
        else
        {
            //std::cout << "Failed to create table: " << result.GetError().GetMessage();
            AddOnScreenMessage(FString::Printf(TEXT("Failed to create table: %s"), *FString(result.GetError().GetMessage().c_str())));
        }
        // snippet-end:[dynamodb.cpp.create_table.code]
    }
    Aws::ShutdownAPI(options);
    
}

void AMainMenuGameMode::ScanTable()
{
    const std::string USAGE = "\n" \
        "Usage:\n"
        "    scan_table <table> [projection_expression]\n\n"
        "Where:\n"
        "    table - the table to Scan.\n\n"
        "You can add an optional projection expression (a quote-delimited,\n"
        "comma-separated list of attributes to retrieve) to limit the\n"
        "fields returned from the table.\n\n"
        "Example:\n"
        "    scan_table HelloTable \n"
        "    scan_table SiteColors \"default, bold\"\n";

    FString Message = FString(USAGE.c_str());
    AddOnScreenMessage(Message);
    
    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {
        const Aws::String table("HelloTable");
        const Aws::String projection("Hiya");

        // snippet-start:[dynamodb.cpp.scan_table.code]
        Aws::Client::ClientConfiguration clientConfig;
       
        Aws::DynamoDB::DynamoDBClient dynamoClient(clientConfig);
        Aws::DynamoDB::Model::ScanRequest req;
        req.SetTableName(table);

        if (!projection.empty())
            req.SetProjectionExpression(projection);        


        // Perform scan on table
        const Aws::DynamoDB::Model::ScanOutcome& result = dynamoClient.Scan(req);
        if (result.IsSuccess())
        {
            // Reference the retrieved items
            const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>>& items = result.GetResult().GetItems();
            if(items.size() > 0) 
            {
                std::cout << "Number of items retrieved from scan: " << items.size() << std::endl;

                AddOnScreenMessage(FString::Printf(TEXT("Number of items retrieved from scan: %llu"), items.size()));
                //Iterate each item and print
                for(const auto &item: items)
                {
                std::cout << "******************************************************" << std::endl;
                    AddOnScreenMessage(FString("******************************************************"));
                // Output each retrieved field and its value
                for (const auto& i : item)
                {
                    std::cout << i.first << ": " << i.second.GetS() << std::endl;
                    FString::Printf(TEXT("%s: %s"), *FString(i.first.c_str()), *FString(i.second.GetS().c_str()));
                }
                    
                }
            }

            else
            {
                std::cout << "No item found in table: " << table << std::endl;
                AddOnScreenMessage(FString::Printf(TEXT("No item found in table: %s"), *FString(table.c_str())));
            }
        }
        else
        {
            std::cout << "Failed to Scan items: " << result.GetError().GetMessage();
            AddOnScreenMessage(FString::Printf(TEXT("Failed to scan items: %s"), *FString(result.GetError().GetMessage().c_str())));
        }
        // snippet-end:[dynamodb.cpp.scan_table.code]
    }
    Aws::ShutdownAPI(options);
}

void AMainMenuGameMode::ListTables()
{
    //std::cout << "Your DynamoDB Tables:" << std::endl;
    AddOnScreenMessage("Your DynamoDB Tables:");

    Aws::SDKOptions options;

    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        Aws::DynamoDB::DynamoDBClient dynamoClient(Client);

        Aws::DynamoDB::Model::ListTablesRequest ltr;
        ltr.SetLimit(50);
        do
        {
            const Aws::DynamoDB::Model::ListTablesOutcome& lto = dynamoClient.ListTables(ltr);
            if (!lto.IsSuccess())
            {
                std::cout << "Error: " << lto.GetError().GetMessage() << std::endl;
                AddOnScreenMessage(FString::Printf(TEXT("Error: %s"), *FString(lto.GetError().GetMessage().c_str())));
                return;
            }
            for (const auto& s : lto.GetResult().GetTableNames())
            {
                std::cout << s << std::endl;
                AddOnScreenMessage(FString::Printf(TEXT("Table Name: %s"), *FString(s.c_str())));
            }
               
            ltr.SetExclusiveStartTableName(lto.GetResult().GetLastEvaluatedTableName());
        } while (!ltr.GetExclusiveStartTableName().empty());
    }
    Aws::ShutdownAPI(options);
}

void AMainMenuGameMode::AddOnScreenMessage(const FString& InMessage)
{
    const FString DisplayMessage = FString::Printf(TEXT("DynamoDB: %s"), *InMessage);
    GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red, DisplayMessage);
}
