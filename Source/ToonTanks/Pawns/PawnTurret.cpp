// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "PawnTank.h"

APawnTurret::APawnTurret()
{
	
}

// Called when the game starts or when spawned
void APawnTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &APawnTurret::CheckFireCondition, FireRate, true, false);

	PlayerPawn = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void APawnTurret::CheckFireCondition()
{
	// Checks for nullptr
	if(!PlayerPawn) { return; }

	// Checks if player is in range
	if(ReturnDistanceToPlayer() <= FireRange)
	{
		// TODO attack the player
		Fire();
	}
}

float APawnTurret::ReturnDistanceToPlayer() const
{
	if(!PlayerPawn) { return 0.0f; }

	float Distance = (PlayerPawn->GetActorLocation() - GetActorLocation()).Size();
	return Distance;
}

void APawnTurret::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

// Called every frame
void APawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!PlayerPawn || ReturnDistanceToPlayer() > FireRange)
	{
		return;
	}
	RotateTurret(PlayerPawn->GetActorLocation());
	
}