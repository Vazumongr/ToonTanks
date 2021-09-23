#include "TTPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "ToonTanks/PlayerStates/TTPlayerState.h"

void ATTPlayerController::Tick(float DeltaSeconds)
{
	ATTPlayerState* TTPS = Cast<ATTPlayerState>(PlayerState);
	FString Name = PlayerState->GetPlayerName();
	FString Message = (FString::Printf(TEXT("%s score: %f"), *TTPS->UserName, Score));
	GEngine->AddOnScreenDebugMessage(-1,0,FColor::Emerald,Message);
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
