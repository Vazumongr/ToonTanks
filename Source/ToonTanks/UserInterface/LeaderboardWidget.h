// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "ToonTanks/ToonTanks.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "LeaderboardWidget.generated.h"

USTRUCT(BlueprintType)
struct TOONTANKS_API FUserScore
{
	GENERATED_BODY()

	FUserScore() {}

	FUserScore(FString InName, float InScore)
	{
		Username=InName;
		Score=InScore;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Username = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Score = 0;
};

/**
 * 
 */
UCLASS()
class TOONTANKS_API ULeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

	ULeaderboardWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	class FHttpModule* Http;
	
	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProcessScanResponse(FHttpRequestPtr Request, FHttpResponsePtr Response);
	
	void ScanLeaderboardRequest();

protected:

	UFUNCTION(BlueprintCallable)
	void SortUsersScores();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLeaderboardList();

	UFUNCTION(BlueprintImplementableEvent)
	void HideFields();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FUserScore> UsersScores;

public:
	UFUNCTION(BlueprintCallable)
	void Setup();
};
