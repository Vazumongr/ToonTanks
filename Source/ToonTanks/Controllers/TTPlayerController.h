#pragma once

#include "TTPlayerController.generated.h"

UCLASS()
class TOONTANKS_API ATTPlayerController : public APlayerController
{

	GENERATED_BODY()
public:

	ATTPlayerController();

	virtual void Tick(float DeltaSeconds) override;

	void AddScore(float InScore);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetScore() const { return Score; }
	
protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UUserWidget* PauseMenuWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class TSubclassOf<UUserWidget> PauseMenuClass;


private:
	void OpenMenu();
	float Score = 0.f;
	
};
