// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanks/UserInterface/TTHUD.h"

void UTTHUD::SetOwner(AActor* InActor)
{
	if(InActor!=nullptr) Owner=InActor;
}
