// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TTSignInWidget.generated.h"

/**
* 
*/
UCLASS()
class TOONTANKS_API UTTSignInWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

	FString Username;
	FString Password;

protected:
	
	UFUNCTION(BlueprintCallable)
	void SignInUser();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HideFields();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEditableTextBox* UsernameTextBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UEditableTextBox* PasswordTextBox;

public:
	UFUNCTION(BlueprintCallable)
	void Setup();

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayInvalidLogin();
};
