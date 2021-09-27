// Troy Records Jr. 2021
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "TTDropBase.generated.h"

/*
 * This is the base class for all loot drops. This should
 * not be used but inherited from instead
 */
UCLASS(Abstract)
class TOONTANKS_API ATTDropBase : public AStaticMeshActor
{
    GENERATED_BODY()
    
    ATTDropBase();

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION(BlueprintNativeEvent)
	void OverlappedPlayer(class ATTPawnTank* PlayerTank);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float ScaleReductionFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setup")
	bool bCanShrink;
};