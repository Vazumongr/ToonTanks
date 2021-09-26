// Fill out your copyright notice in the Description page of Project Settings.
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

#include "ToonTanks/GameInstances/TTGameInstance.h"

UTTDatabaseAgent::UTTDatabaseAgent()
{
	Http = &FHttpModule::Get();
}

void UTTDatabaseAgent::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTTDatabaseAgent::Deinitialize()
{
	Super::Deinitialize();
}

void UTTDatabaseAgent::AddScoreToLeaderboard(float PlayersScore)
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
	Request->OnProcessRequestComplete().BindUObject(this, &UTTDatabaseAgent::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(InsertScoreURL);
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void UTTDatabaseAgent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}
