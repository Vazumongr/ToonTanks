// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "ToonTanks/ToonTanks.h"
#include "GameFramework/GameModeBase.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "TankGameModeBase.generated.h"

class APawnTurret;
class APawnTank;

UCLASS()
class TOONTANKS_API ATankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATankGameModeBase();

	FHttpModule* Http;
	
	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	void ScanLeaderboard();
	void AddScoreToLeaderboard(float PlayersScore);
	void SetUsername();
	
	void ActorDied(AActor* DeadActor);
	
	UFUNCTION(BlueprintImplementableEvent)
	void GameStart();
	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool PlayerWon);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowEndGameMenu(bool inPlayerWon);
	
private:

	FHttpRequestPtr InsertRequest;
	bool bDevTest = false;

	bool bPlayerWon = false;

	int32 TargetTurrets = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	APawnTank* PlayerTank;
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
