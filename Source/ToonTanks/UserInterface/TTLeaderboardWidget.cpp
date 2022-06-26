// Fill out your copyright notice in the Description page of Project Settings.
#include "ToonTanks/UserInterface/TTLeaderboardWidget.h"

#include "ToonTanks/Subsystems/TTDatabaseAgent.h"


bool UTTLeaderboardWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	return true;
}

void UTTLeaderboardWidget::Setup()
{
	HideFields();
	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance != nullptr);
	UTTDatabaseAgent* DatabaseAgent = GameInstance->GetSubsystem<UTTDatabaseAgent>();
	check(DatabaseAgent != nullptr);
	DatabaseAgent->ScanLeaderboardRequest(this);
}

void UTTLeaderboardWidget::SetUserScores(TArray<FUserScore> InScores)
{
	UsersScores=InScores;
	SortUsersScores();
	UpdateLeaderboardList();
}

void UTTLeaderboardWidget::SortUsersScores()
{
	int8 i, j, min_idx;
	FUserScore temp;
	for(i = 0; i < UsersScores.Num() - 1; i++)
	{
		min_idx = i;
		for(j = i+1; j < UsersScores.Num(); j++)
		{
			if(UsersScores[j].Score < UsersScores[min_idx].Score)
			{
				min_idx = j;
			}
			temp = UsersScores[min_idx];
			UsersScores[min_idx] = UsersScores[i];
			UsersScores[i] = temp;
		}
	}
}
