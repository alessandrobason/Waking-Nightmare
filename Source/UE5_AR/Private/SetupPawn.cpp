#include "SetupPawn.h"
#include "Camera/CameraComponent.h"

#include "SetupGameMode.h"
#include "LogUtils.h"

ASetupPawn::ASetupPawn() {
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);
}

void ASetupPawn::BeginPlay() {
	Super::BeginPlay();
}

void ASetupPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos) {
	if (auto GameMode = GetWorld()->GetAuthGameMode<ASetupGameMode>()) {
		GameMode->SelectPlane(ScreenPos);
	}
}

void ASetupPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASetupPawn::OnScreenTouch);
}
