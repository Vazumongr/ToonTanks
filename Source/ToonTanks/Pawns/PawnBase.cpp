// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ToonTanks/Actors/ProjectileBase.h"
#include "ToonTanks/Components/HealthComponent.h"
#include "ToonTanks/GameModes/TankGameModeBase.h"

// Sets default values
APawnBase::APawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Defaults to root component since it is the first declared
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;	// Ensures that it is the root component

	// Makes the base mesh a child of the root component
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	// Makes the turret mesh a child of the base mesh
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	// Makes the Projectile Spawn Point a child of the turret mesh
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment((TurretMesh));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

}

void APawnBase::PawnDestroyed()
{
	HandleDestruction();
}

void APawnBase::TakeDamage(float Damage, AActor* DamagedBy, AController* InstigatedBy)
{
	if(HealthComponent != nullptr)
	{
		HealthComponent->TakeDamage(Damage, DamagedBy, InstigatedBy);
	}
}

void APawnBase::RotateTurret(FVector LookAtTarget)
{
	// Update Turretmesh rotation to face towards the LookAtTarget passed in from the child class.
	// TurretMesh->SetWorldRotation()...

	// Find the rotation value to look at (the FRotator)
	FVector StartLocation = TurretMesh->GetComponentLocation();
	FRotator TurretRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, FVector(LookAtTarget.X, LookAtTarget.Y, TurretMesh->GetComponentLocation().Z));

	// Rotate the turret to match that FRotator
	TurretMesh->SetWorldRotation(TurretRotation);
}

void APawnBase::Fire()
{
	// Get ProjectileSpawnPoint Location && Rotation -> Spawn Projectile class at location towards rotation
	if(ProjectileClass)
	{
		FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
		
		AProjectileBase* TempProjecile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation);
		TempProjecile->SetOwner(this);
	}
	
}

void APawnBase::HandleDestruction()
{
	GetWorld()->SpawnActor<AActor>(DropClass, GetActorLocation(), FRotator(0,0,0));
}


