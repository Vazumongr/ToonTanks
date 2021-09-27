// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TTLeaderboardWidget.generated.h"

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
class TOONTANKS_API UTTLeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

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

	void SetUserScores(TArray<FUserScore> InScores);
};
