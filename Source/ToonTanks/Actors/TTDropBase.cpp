// Troy Records Jr. 2021

#include "ToonTanks/Actors/TTDropBase.h"

#include "ToonTanks/Pawns/TTPawnTank.h"

ATTDropBase::ATTDropBase()
{
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->SetCollisionProfileName(FName("OverlapAll"));
	GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATTDropBase::BeginOverlap);
	ScaleReductionFloat = .05;
	bCanShrink = false;
}

void ATTDropBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bCanShrink)
	{
		FVector ActorScale = GetActorScale();
		if(ActorScale.Equals(FVector::ZeroVector, 0.1f))
		{
			Destroy();
		}
		ActorScale -= FVector(ScaleReductionFloat);
		SetActorScale3D(ActorScale);
		
	}
}

void ATTDropBase::OverlappedPlayer_Implementation(ATTPawnTank* PlayerTank)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped player tank"));
}

void ATTDropBase::BeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATTPawnTank* PawnTank = Cast<ATTPawnTank>(OtherActor);
	if(PawnTank == nullptr) return;
	bCanShrink = true;
	OverlappedPlayer(PawnTank);
}
