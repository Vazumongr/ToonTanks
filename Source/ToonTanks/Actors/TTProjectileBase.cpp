// Fill out your copyright notice in the Description page of Project Settings.


#include "TTProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/Pawns/TTPawnBase.h"

// Sets default values
ATTProjectileBase::ATTProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ATTProjectileBase::OnHit);
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<class UProjectileMovementComponent>(TEXT("Projectile Movement"));

	ProjectileMovement->InitialSpeed = MovementSpeed;
	ProjectileMovement->MaxSpeed = MovementSpeed;

	InitialLifeSpan = 3.0f;

}

void ATTProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Try to get a reference to the owning class
	AActor* MyOwner = GetOwner();
	if(!MyOwner) { return; }

	// If the OtherActors isn't self or owner && exists, then apply damage.
	if(OtherActor != nullptr && OtherActor != this && OtherActor != MyOwner)
	{
		ATTPawnBase* PawnHit = Cast<ATTPawnBase>(OtherActor);
		if(PawnHit != nullptr)
		{
			PawnHit->MyTakeDamage(RealDamage, this, MyOwner->GetInstigatorController());
		}
	}

	// Do a bunch of effects here during polish phase.

	Destroy();
}

// Called when the game starts or when spawned
void ATTProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

