// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "TTMainMenuGameMode.generated.h"

UCLASS()
class TOONTANKS_API ATTMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerStateUserName(FString UserName, APlayerState* PlayersState);

	virtual void BeginPlay() override;

private:
	void AddOnScreenMessage(const FString& Message);

};