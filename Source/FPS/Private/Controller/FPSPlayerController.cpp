// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FPSPlayerController.h"
#include "Blueprint/UserWidget.h"

AFPSPlayerController::AFPSPlayerController()
{
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
