// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/UserInterface/LeaderboardWidget.h"


ULeaderboardWidget::ULeaderboardWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();
}

bool ULeaderboardWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	return true;
}

void ULeaderboardWidget::Setup()
{
	ScanLeaderboardRequest();
}

void ULeaderboardWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(Response->GetHeader("Result-Type") == "scan")
	{
		ProcessScanResponse(Request, Response);
	}
	UE_LOG(LogHttp, Warning, TEXT("Response Received..."));
	/*
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	*/
	int32 ResponseCode = Response->GetResponseCode();
	TArray<FString> Headers = Response->GetAllHeaders();
	FString Content = Response->GetContentAsString();

	UE_LOG(LogHttp, Warning, TEXT("%i"), Headers.Num());

	FString HeaderString;
	for(FString& msg : Headers)
	{
		HeaderString.Append(msg);
	}
    
	UE_LOG(LogHttp, Warning, TEXT("StatusCode: %i | Headers: %s | Content: %s"), ResponseCode, *HeaderString, *Content);

	FString OnScreenMessage = FString::Printf(TEXT("StatusCode: %i \n| Headers: %s \n| Content: %s"), ResponseCode, *HeaderString, *Content);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, OnScreenMessage);
	/*
	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TMap<FString, TSharedPtr<FJsonValue>> Values = JsonObject->Values;
		const TArray<TSharedPtr<FJsonValue>>& Items = JsonObject->GetArrayField("Items");
		for(const TSharedPtr<FJsonValue>& JsonValuePtr : Items)
		{
			FString Username = JsonValuePtr.Get()->AsObject()->GetStringField("Username");
			float Score = JsonValuePtr.Get()->AsObject()->GetNumberField("Score");
			UE_LOG(LogHttp, Warning, TEXT("Username: %s     Score: %f"), *Username, Score);
		}
	}
	*/
}

void ULeaderboardWidget::ProcessScanResponse(FHttpRequestPtr Request, FHttpResponsePtr Response)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TMap<FString, TSharedPtr<FJsonValue>> Values = JsonObject->Values;
		const TArray<TSharedPtr<FJsonValue>>& Items = JsonObject->GetArrayField("Items");
		for(const TSharedPtr<FJsonValue>& JsonValuePtr : Items)
		{
			FString Username = JsonValuePtr.Get()->AsObject()->GetStringField("Username");
			float Score = JsonValuePtr.Get()->AsObject()->GetNumberField("Score");
			UE_LOG(LogHttp, Warning, TEXT("Username: %s     Score: %f"), *Username, Score);
			UsersScores.Add(FUserScore(Username,Score));
			UpdateLeaderboardList();
		}
	}
}

void ULeaderboardWidget::ScanLeaderboardRequest()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULeaderboardWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/scores");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}
