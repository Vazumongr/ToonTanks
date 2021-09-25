// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "ToonTanks/ToonTanks.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "TTSignInWidget.generated.h"

/**
* 
*/
UCLASS()
class TOONTANKS_API UTTSignInWidget : public UUserWidget
{
	GENERATED_BODY()

	UTTSignInWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	class FHttpModule* Http;
	FHttpRequestPtr CreateUserRequest;
	
	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FString Username;
	FString Password;

protected:
	
	UFUNCTION(BlueprintCallable)
	void SignInUser();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayInvalidLogin();

	void CreateUser();
	void ValidateUserPassComb(FHttpResponsePtr Response);

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HideFields();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEditableTextBox* UsernameTextBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEditableTextBox* PasswordTextBox;

public:
	UFUNCTION(BlueprintCallable)
	void Setup();
};
