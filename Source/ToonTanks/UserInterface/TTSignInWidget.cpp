// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/UserInterface/TTSignInWidget.h"

#include "Components/EditableTextBox.h"
#include "ToonTanks/Subsystems/TTDatabaseAgent.h"


bool UTTSignInWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	return true;
}

void UTTSignInWidget::Setup()
{
	
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
	if(Password.Len() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please enter a password"));
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance != nullptr);
	UTTDatabaseAgent* DatabaseAgent = GameInstance->GetSubsystem<UTTDatabaseAgent>();
	check(DatabaseAgent != nullptr);
	DatabaseAgent->SignInUser(Username,Password, this);
}
