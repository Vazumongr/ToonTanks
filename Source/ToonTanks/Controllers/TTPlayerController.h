#pragma once

#include "TTPlayerController.generated.h"

UCLASS()
class TOONTANKS_API ATTPlayerController : public APlayerController
{

	GENERATED_BODY()
public:

	ATTPlayerController();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void AddScore(float InScore);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetScore() const { return Score; }
	
protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="User Interface")
	class UUserWidget* PauseMenuWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="User Interface")
	class TSubclassOf<UUserWidget> PauseMenuClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="User Interface")
	class UTTHUD* HUDWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="User Interface")
	class TSubclassOf<UTTHUD> HUDClass;


private:
	void OpenMenu();
	float Score = 0.f;
	
};
