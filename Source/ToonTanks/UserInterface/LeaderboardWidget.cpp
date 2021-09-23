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
	HideFields();
	ScanLeaderboardRequest();
}

void ULeaderboardWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(Response->GetHeader("Result-Type") == "scan")
	{
		ProcessScanResponse(Request, Response);
	}
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
			UsersScores.Add(FUserScore(Username,Score));
		}
		SortUsersScores();
		UpdateLeaderboardList();
	}
}

void ULeaderboardWidget::ScanLeaderboardRequest()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULeaderboardWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(APILINK);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void ULeaderboardWidget::SortUsersScores()
{
	int8 i, j, min_idx;
	FUserScore temp;
	for(i = 0; i < UsersScores.Num(); i++)
	{
		min_idx = i;
		for(j = i+1; j < UsersScores.Num(); j++)
		{
			if(UsersScores[j].Score < UsersScores[min_idx].Score)
			{
				min_idx = j;
			}
			temp = UsersScores[min_idx];
			UsersScores[min_idx] = UsersScores[j];
			UsersScores[j] = temp;
		}
	}
}
