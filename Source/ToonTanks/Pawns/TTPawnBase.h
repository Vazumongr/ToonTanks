// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "TTPawnBase.generated.h"

class UCapsuleComponent;
class ATTProjectileBase;

UCLASS()
class TOONTANKS_API ATTPawnBase : public APawn
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	ATTPawnBase();
	
	void PawnDestroyed();

	virtual void BeginPlay() override;

	void MyTakeDamage(float Damage, AActor* DamagedBy, class AController* InstigatedBy);
	FORCEINLINE class UTTHealthComponent* GetHealthComponent() const { return HealthComponent;}

	UFUNCTION(BlueprintCallable)
    void Heal(float Amount) const;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Type", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATTProjectileBase> ProjectileClass;

protected:

	// Rotate the turret
	void RotateTurret(FVector LookAtTarget);

	// Create and launch the projectile
	virtual void Fire();

	// Handle the destruction. Virtual because it will be overriden
	virtual void HandleDestruction();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> DropClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UTTHealthComponent* HealthComponent;

};
