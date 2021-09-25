// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "ToonTanks/ToonTanks.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "TTHUD.generated.h"

/**
* 
*/
UCLASS()
class TOONTANKS_API UTTHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float NewHealth);

	void SetOwner(AActor* InActor);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* Owner;
	
};
