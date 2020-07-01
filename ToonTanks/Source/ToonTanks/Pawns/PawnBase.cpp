// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnBase.h"
#include "Components/CapsuleComponent.h"

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

}

void APawnBase::RotateTurret(FVector LookAtTarget)
{
	// Update Turretmesh rotation to face towards the LookAtTarget passed in from the child class.
	// TurretMesh->SetWorldRotation()...
}

void APawnBase::Fire()
{
	// Get ProjectileSpawnPoint Location && Rotation -> Spawn Projectile class at location towards rotation
}

void APawnBase::HandleDestruction()
{
	// Universal functionality ---
	// Play death effects particles, sound, camera shake, etc.
	//
	// ... Then do unique child overrides
	// -- PawnTurret - Inform GameMode Turret died -> Then Destroy() self
	//
	// -- PawnTank - Inform GameMode Player died -> Then Hide() all components && stop movement Input
}


