// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TTGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API UTTGameInstance : public UGameInstance
{
	GENERATED_BODY()

	FString PlayersUsername;

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayersUsername(FString InName) { PlayersUsername = InName; }
	UFUNCTION(BlueprintCallable)
	FString GetPlayerUsername() const { return PlayersUsername; }
};
