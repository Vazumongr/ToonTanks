// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/UserInterface/SignInWidget.h"

#include "Components/EditableTextBox.h"


USignInWidget::USignInWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Http = &FHttpModule::Get();
}

bool USignInWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	return true;
}

void USignInWidget::Setup()
{
	
}

void USignInWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(CreateUserRequest == Request)
	{
		StartGame();
		return;
	}
	if(Response->GetContentAsString().Len() <= 2)
	{
		//DisplayInvalidLogin();
		UE_LOG(LogHttp, Warning, TEXT("User doesn't exist. Creating user."));
		CreateUser();
	}
	else
	{
		ValidateUserPassComb(Response);
	}

	
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
}

void USignInWidget::SignInUser()
{
	UE_LOG(LogHttp, Warning, TEXT("Signing in..."));
	Username = UsernameTextBox->GetText().ToString();
	Password = PasswordTextBox->GetText().ToString();

	FString URL = FString::Printf(TEXT("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users/%s"), *Username);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &USignInWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void USignInWidget::CreateUser()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("Username", Username);
	JsonObject->SetStringField("Password", Password);
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &USignInWidget::OnResponseReceived);
	//This is the url on which to process the request
	Request->SetURL("https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/users");
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
	CreateUserRequest = Request;
}

void USignInWidget::ValidateUserPassComb(FHttpResponsePtr Response)
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
