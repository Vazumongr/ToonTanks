// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#if UE_BUILD_SHIPPING
	#define APILINK "https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/testScores"
#else
	#define APILINK "https://9bkd1wd39i.execute-api.us-east-2.amazonaws.com/scores"
#endif
#include "CoreMinimal.h"
