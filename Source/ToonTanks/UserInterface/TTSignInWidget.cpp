// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/UserInterface/TTSignInWidget.h"

#include "Components/EditableTextBox.h"


UTTSignInWidget::UTTSignInWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();
}

bool UTTSignInWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	return true;
}

void UTTSignInWidget::Setup()
{
	
}

void UTTSignInWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(CreateUserRequest == Request)
	{
		StartGame();
		return;
	}
	if(Response->GetContentAsString().Len() <= 2)
	{
		CreateUser();
	}
	else
	{
		ValidateUserPassComb(Response);
	}
}

void UTTSignInWidget::SignInUser()
{
	UE_LOG(LogHttp, Warning, TEXT("Signing in..."));
	Username = UsernameTextBox->GetText().ToString();
	Password = PasswordTextBox->GetText().ToString();

	if(Username.Len() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please enter a username"));
		return;
	}
	else if(Password.Len() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please enter a password"));
		return;
	}

	FString URL = FString::Printf(TEXT("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users/%s"), *Username);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTTSignInWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void UTTSignInWidget::CreateUser()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("Username", Username);
	JsonObject->SetStringField("Password", Password);
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTTSignInWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users");
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
	CreateUserRequest = Request;
}

void UTTSignInWidget::ValidateUserPassComb(FHttpResponsePtr Response)
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
			StartGame();
		}
		else
		{
			UE_LOG(LogHttp, Warning, TEXT("Invalid login"));
			DisplayInvalidLogin();
		}
		
	}
}
