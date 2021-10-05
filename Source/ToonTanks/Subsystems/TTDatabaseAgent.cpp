// Fill out your copyright notice in the Description page of Project Settings.
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

#include "ToonTanks/GameInstances/TTGameInstance.h"
#include "ToonTanks/UserInterface/TTLeaderboardWidget.h"
#include "ToonTanks/UserInterface/TTSignInWidget.h"

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

void UTTDatabaseAgent::ProcessScanRequest(FHttpRequestPtr Request, FHttpResponsePtr Response) const
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
			FString TempUsername = JsonValuePtr.Get()->AsObject()->GetStringField("Username");
			float Score = JsonValuePtr.Get()->AsObject()->GetNumberField("Score");
			UsersScores.Add(FUserScore(TempUsername,Score));
		}
		Leaderboard->SetUserScores(UsersScores);
	}
}

void UTTDatabaseAgent::ProcessSignInRequest(FHttpRequestPtr Request, FHttpResponsePtr Response)
{
	if(Response->GetContentAsString().Len() <= 2)
	{
		CreateUser();
	}
	else
	{
		ValidateUserPassComb(Response);
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

void UTTDatabaseAgent::SignInUser(FString& InUsername, FString& InPassword, UTTSignInWidget* InSignInMenu)
{
	if(InSignInMenu == nullptr)
		return;
	SignInMenu = InSignInMenu;
	Username = InUsername;
	Password = InPassword;
	
	FString URL = FString::Printf(TEXT("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users/%s"), *InUsername);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTTDatabaseAgent::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
	SignInRequest = Request;
}

void UTTDatabaseAgent::CreateUser()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("Username", Username);
	JsonObject->SetStringField("Password", Password);
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTTDatabaseAgent::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users");
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
	CreateUserRequest = Request;
}

void UTTDatabaseAgent::ValidateUserPassComb(FHttpResponsePtr Response)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const TSharedPtr<FJsonObject>& ObjField = JsonObject->GetObjectField("Item");
		FString BackName = ObjField.Get()->GetStringField("Username");
		FString BackPass = ObjField.Get()->GetStringField("Password");
		if(BackName == Username && BackPass == Password)
		{
			UE_LOG(LogHttp, Warning, TEXT("Valid login"));
			SignInMenu->StartGame();
		}
		else
		{
			UE_LOG(LogHttp, Warning, TEXT("Invalid login"));
			SignInMenu->DisplayInvalidLogin();
		}
		
	}
}

void UTTDatabaseAgent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(ScanRequest==Request)
	{
		ProcessScanRequest(Request, Response);
	}
	if(SignInRequest == Request)
	{
		ProcessSignInRequest(Request, Response);
	}
	if(CreateUserRequest == Request)
	{
		SignInMenu->StartGame();
	}
}
