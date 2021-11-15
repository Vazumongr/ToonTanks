// Fill out your copyright notice in the Description page of Project Settings.
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"

#include "ToonTanks/GameInstances/TTGameInstance.h"
#include "ToonTanks/UserInterface/TTLeaderboardWidget.h"
#include "ToonTanks/UserInterface/TTSignInWidget.h"
#include "Misc/AES.h"
#include "ToonTanks/ToonTanks.h"

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
	FString ResponseString = Response->GetContentAsString();
	if(ResponseString.Equals("Valid"))
		SignInMenu->StartGame();
	else if(ResponseString.Equals("Invalid"))
		SignInMenu->DisplayInvalidLogin();
	else if(ResponseString.Equals("Nonexistent"))
		CreateUser();
	else
		UE_LOG(LogLogin, Error, TEXT("Unexpected response from server: %s"), *ResponseString);
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
	Password = "";
	if(InSignInMenu == nullptr)
		return;
	SignInMenu = InSignInMenu;
	Username = InUsername;
	Password = EncryptPassword(InPassword);

	FString URL = FString::Printf(TEXT("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users/%s/%s"), *InUsername, *Password);
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

void UTTDatabaseAgent::ValidateLogin(FString& ResponseString)
{
	if(ResponseString == "Success")
	{
		SignInMenu->StartGame();
	}
	else
	{
		SignInMenu->DisplayInvalidLogin();
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

FString UTTDatabaseAgent::TempEncryptPassword(FString InPassword)
{
	return "";
}

FString UTTDatabaseAgent::EncryptPassword(FString& InPassword)
{
	if(InPassword.IsEmpty()) return InPassword;  //empty string? do nothing
	
	/** Found on japanese forum. Works without but keeping here just in case
	 * because I surely don't know what I'm doing
	 */
	//FString SplitSymbol = "EL@$@!";
	//InPassword.Append(SplitSymbol);
	
	// If passwords are 3 chars, it breaks (or i thought. idk anymore. too tired)
 	InPassword.Append("salty");
	/** Any number that is not a multiple of 3,
	 *  can be converted to a multiple of 3 with below formula
	 *  Number + (2 + (3x) * r) = Multiple of 3. Where r is the remainder from Number % 3 and x is any positive integer
	 *  Idk if there's a name for this. I just spent the last hour
	 *  figuring that out.
	 */
	uint8 Count = InPassword.Len() % 3;
	for(int i = 0; i < Count; i++)
	{
		InPassword.Append("salty");
	}

	uint32 Size = InPassword.Len();
	Size = Size + (FAES::AESBlockSize - (Size % FAES::AESBlockSize));
	uint8* Blob = new uint8[Size]; //So once we calculated size we allocating space in memory

	FString KeyStr = TEXT("7x!A%D*G-KaPdSgVkYp3s6v9y/B?E(H+");	// the key
	KeyStr = FMD5::HashAnsiString(*KeyStr);	// saw on forum. Think this might have been part of my issue originally. idk
	TCHAR *KeyTChar = KeyStr.GetCharArray().GetData();           
	ANSICHAR *KeyAnsi = (ANSICHAR*)TCHAR_TO_ANSI(KeyTChar);

	// StringToBytes instead of ToBlob because this is working
	if( StringToBytes(InPassword, Blob, Size)) {
		
		FAES::EncryptData(Blob,Size,KeyAnsi); //We encrypt the data
		InPassword = FString::FromHexBlob(Blob,Size); 
		InPassword = InPassword.Mid(0,InPassword.Len()/2); // Cut off back half of gibberish
		delete Blob; //deleting allocation for safety
		return InPassword; //and return it
	}
	else
	{
		UE_LOG(LogHttp, Error, TEXT("Password Encryption Failed: Password cannot be coverted to blob."));
	}
	delete Blob; //deleting allocation for safety
	return ""; //If failed return empty string
}