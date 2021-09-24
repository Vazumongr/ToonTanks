// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ToonTanks/GameModes/TankGameModeBase.h"

#include "HealthComponent.generated.h"

class ATankGameModeBase;

DECLARE_DELEGATE_OneParam(FHealthChangedDelegate, float);
DECLARE_DYNAMIC_DELEGATE_OneParam(FHealthChangedDynamicDelegate, float, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOONTANKS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	FHealthChangedDelegate HealthChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthChangedDynamicDelegate HealthChangedDynamic;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION()
	void TakeDamage(float Damage, AActor* DamagedBy, class AController* InstigatedBy);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float DefaultHealth = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health = 0.0f;

	UPROPERTY()
	AActor* Owner;
	UPROPERTY()
	ATankGameModeBase* GameModeRef;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//UFUNCTION()
	//void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
