// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPawnTurret.h"

#include "Kismet/GameplayStatics.h"
#include "TTPawnTank.h"

ATTPawnTurret::ATTPawnTurret()
{
	
}

// Called when the game starts or when spawned
void ATTPawnTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ATTPawnTurret::CheckFireCondition, FireRate, true, false);

	PlayerPawn = Cast<ATTPawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATTPawnTurret::CheckFireCondition()
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

float ATTPawnTurret::ReturnDistanceToPlayer() const
{
	if(!PlayerPawn) { return 0.0f; }

	const float Distance = (PlayerPawn->GetActorLocation() - GetActorLocation()).Size();
	return Distance;
}

void ATTPawnTurret::HandleDestruction()
{
	Super::HandleDestruction();
	UE_LOG(LogTemp, Warning, TEXT("Called Destroy"));
	Destroy();
}

// Called every frame
void ATTPawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!PlayerPawn || ReturnDistanceToPlayer() > FireRange)
	{
		return;
	}
	RotateTurret(PlayerPawn->GetActorLocation());
	
}