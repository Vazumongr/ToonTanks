// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "TTTankGameModeBase.generated.h"

class ATTPawnTurret;
class ATTPawnTank;

UCLASS()
class TOONTANKS_API ATTTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void SetUsername();
	
	void ActorDied(AActor* DeadActor);
	
	UFUNCTION(BlueprintImplementableEvent)
	void GameStart();
	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool PlayerWon);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowEndGameMenu(bool inPlayerWon);
	
private:

	bool bDevTest = false;

	bool bPlayerWon = false;

	int32 TargetTurrets = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	ATTPawnTank* PlayerTank;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	class ATTPlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scoring", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<AActor>, float> TargetValues;
	
	void HandleGameStart();
	void HandleGameOver(bool PlayerWon);

	void AddScore(AActor* KilledActor);

	int32 GetTargetTurretCount();

	
protected:
	virtual void BeginPlay() override;
};
