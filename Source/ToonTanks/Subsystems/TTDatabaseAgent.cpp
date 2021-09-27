// Fill out your copyright notice in the Description page of Project Settings.
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

#include "ToonTanks/GameInstances/TTGameInstance.h"
#include "ToonTanks/UserInterface/TTLeaderboardWidget.h"

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

void UTTDatabaseAgent::ProcessScanResponse(FHttpRequestPtr Request, FHttpResponsePtr Response)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TArray<FUserScore> UsersScores;
		TMap<FString, TSharedPtr<FJsonValue>> Values = JsonObject->Values;
		const TArray<TSharedPtr<FJsonValue>>& Items = JsonObject->GetArrayField("Items");
		for(const TSharedPtr<FJsonValue>& JsonValuePtr : Items)
		{
			FString Username = JsonValuePtr.Get()->AsObject()->GetStringField("Username");
			float Score = JsonValuePtr.Get()->AsObject()->GetNumberField("Score");
			UsersScores.Add(FUserScore(Username,Score));
		}
		Leaderboard->SetUserScores(UsersScores);
	}
}

void UTTDatabaseAgent::ScanLeaderboardRequest(UTTLeaderboardWidget* InLeaderboard)
{
	if(InLeaderboard == nullptr)
		return;
	Leaderboard=InLeaderboard;
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTTDatabaseAgent::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(Apilink);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
	ScanRequest = Request;
}

void UTTDatabaseAgent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(ScanRequest==Request)
	{
		ProcessScanResponse(Request, Response);
	}
}
