// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTPawnBase.h"
#include "TTPawnTurret.generated.h"

class ATTPawnTank;

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATTPawnTurret : public ATTPawnBase
{
	GENERATED_BODY()
	
public:

	ATTPawnTurret();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float FireRate = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    float FireRange = 2000.0f;
	
	FTimerHandle FireRateTimerHandle;

	UPROPERTY()
	ATTPawnTank* PlayerPawn;

	void CheckFireCondition();

	float ReturnDistanceToPlayer() const;

protected:
    
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HandleDestruction() override;
};
