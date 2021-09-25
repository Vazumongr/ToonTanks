// Fill out your copyright notice in the Description page of Project Settings.


#include "TTHealthComponent.h"
#include "ToonTanks/GameModes/TTTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTTHealthComponent::UTTHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTTHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;
	GameModeRef = Cast<ATTTankGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	Owner = GetOwner();
	HealthChanged.ExecuteIfBound(Health);
	//HealthChangedDynamic.ExecuteIfBound(Health);
}

void UTTHealthComponent::TakeDamage(float Damage, AActor* DamagedBy, AController* InstigatedBy)
{
	if(Damage == 0 || Health <= 0) return;
	
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	HealthChanged.ExecuteIfBound(Health);
	//HealthChangedDynamic.ExecuteIfBound(Health);
	if(Health <= 0)
	{
		if(GameModeRef)
		{
			GameModeRef->ActorDied(Owner);
			UE_LOG(LogTemp, Warning, TEXT("Notified gamemode of death"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Health component has no reference to the GameMode"));
		}
	}
}
