// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "TTDatabaseAgent.generated.h"

/**
 * This class is to be used when accessing database.
 * Objects should be making requests to this class
 * instead of accessing the database themselves.
 */
UCLASS()
class UTTDatabaseAgent : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	class FHttpModule* Http;

public:
	UTTDatabaseAgent();
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	void AddScoreToLeaderboard(float PlayersScore);
	void ProcessScanResponse(FHttpRequestPtr Request, FHttpResponsePtr Response);
	void ScanLeaderboardRequest(class UTTLeaderboardWidget* InLeaderboard);
	
	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
private:
	// All my variables
	FString InsertScoreURL = "https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/scores";
	FString Apilink = "https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/scores";
	FHttpRequestPtr ScanRequest;
	UPROPERTY()
	UTTLeaderboardWidget* Leaderboard;
};
