// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetUserName(FString InUserName);

	FString UserName;
	
};
