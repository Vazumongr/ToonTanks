#include "TTPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "ToonTanks/Components/TTHealthComponent.h"
#include "ToonTanks/Pawns/TTPawnBase.h"
#include "ToonTanks/PlayerStates/TTPlayerState.h"
#include "ToonTanks/UserInterface/TTHUD.h"

ATTPlayerController::ATTPlayerController()
{
	bShowMouseCursor = true;
}

void ATTPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ATTPlayerState* TTPS = Cast<ATTPlayerState>(PlayerState);
	FString Name = PlayerState->GetPlayerName();
	FString Message = (FString::Printf(TEXT("%s score: %f"), *TTPS->UserName, Score));
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Emerald,Message);
}

void ATTPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UTTHUD* HUD = Cast<UTTHUD>(CreateWidget(this, HUDClass, TEXT("HUD")));
	if(HUD!=nullptr)
	{
		HUD->SetOwner(this);
		HUD->AddToViewport();
		ATTPawnBase* PlayerChar = GetPawn<ATTPawnBase>();
		if(PlayerChar != nullptr)
		{
			UTTHealthComponent* HealthComponent = PlayerChar->GetHealthComponent();
			if(HealthComponent != nullptr)
			{
				HealthComponent->HealthChanged.BindUObject(HUD, &UTTHUD::UpdateHealth);
				HealthComponent->HealthChangedDynamic.BindUFunction(HUD, "UpdateHealth");
			}
		}
	}
}

void ATTPlayerController::AddScore(float InScore)
{
	Score += InScore;
}

void ATTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(InputComponent == nullptr) return;

	
	InputComponent->BindAction("Open Menu", IE_Pressed, this, &ATTPlayerController::OpenMenu);
}

void ATTPlayerController::OpenMenu()
{
	if(PauseMenuWidget==nullptr)
		PauseMenuWidget = CreateWidget(this, PauseMenuClass);
	if(PauseMenuWidget != nullptr)
	{
		PauseMenuWidget->AddToViewport();
		SetPause(true);
	}
}
