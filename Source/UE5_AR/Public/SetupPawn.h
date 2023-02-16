#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "SetupPawn.generated.h"

class UCameraComponent;

UCLASS()
class UE5_AR_API ASetupPawn : public ADefaultPawn
{
	GENERATED_BODY()

protected:
	ASetupPawn();
	virtual void BeginPlay() override;
	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	UPROPERTY()
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;
};
