#pragma once

#include "TTPlayerController.generated.h"

UCLASS()
class TOONTANKS_API ATTPlayerController : public APlayerController
{

	GENERATED_BODY()
public:

	virtual void Tick(float DeltaSeconds) override;

	void AddScore(float InScore);


private:

	float Score = 0.f;
	
};
