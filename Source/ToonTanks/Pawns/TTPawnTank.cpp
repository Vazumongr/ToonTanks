// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPawnTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ATTPawnTank::ATTPawnTank()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment((RootComponent));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}


// Called when the game starts or when spawned
void ATTPawnTank::BeginPlay()
{
	Super::BeginPlay();
	PlayerControllerRef = Cast<APlayerController>(GetController());
}

void ATTPawnTank::HandleDestruction()
{
	Super::HandleDestruction();
	// Hide Player TODO - Create new function to handle this
}

// Called to bind functionality to input
void ATTPawnTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATTPawnTank::CalculateMoveInput);
	PlayerInputComponent->BindAxis("Turn", this, &ATTPawnTank::CalculateRotateInput);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATTPawnTank::Fire);

}

void ATTPawnTank::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Firing!"));
	Super::Fire();
}

// Called every frame
void ATTPawnTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Rotate();
	Move();

	if(PlayerControllerRef)
	{
		PlayerControllerRef->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
		FVector HitLocation = TraceHitResult.ImpactPoint;

		RotateTurret(HitLocation);
	}
	
}

void ATTPawnTank::CalculateMoveInput(float Value)
{
	MoveDirection = FVector(Value * MoveSpeed * GetWorld()->DeltaTimeSeconds, 0, 0);
}

void ATTPawnTank::CalculateRotateInput(float Value)
{
	float RotateAmount = Value * RotateSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator Rotation = FRotator(0, RotateAmount, 0);
	RotationDirection = FQuat(Rotation);
}

void ATTPawnTank::Move()
{
	AddActorLocalOffset(MoveDirection, true);
}

void ATTPawnTank::Rotate()
{
	AddActorLocalRotation(RotationDirection, true);
}