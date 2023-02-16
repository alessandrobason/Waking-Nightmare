#include "CustomGameMode.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StaticMeshActor.h"
#include "ARBlueprintLibrary.h"
#include "ARPin.h"

#include "CustomARPawn.h"
#include "CustomGameInstance.h"
#include "HelloARManager.h"
#include "MonsterSpawner.h"
#include "LogUtils.h"

ACustomGameMode::ACustomGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set the default pawn and gamestate to be our custom pawn and gamestate programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	SpawnerClass = AMonsterSpawner::StaticClass();
}

void ACustomGameMode::StartPlay() 
{
	if (auto World = GetWorld()) {
		MonsterSpawner = World->SpawnActor<AMonsterSpawner>(SpawnerClass);
		ARManager = World->SpawnActor<AHelloARManager>();
		ARManager->Setup(false);
	}

	if (!PauseMenu) {
		PauseMenu = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);
		PauseMenu->SetIsEnabled(false);
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		PauseMenu->AddToViewport(1);
	}

	SetupPlayableZone();

	Super::StartPlay();
}

void ACustomGameMode::SetupPlayableZone() {
	FTransform PlaneTransform = FTransform::Identity;
	if (auto Instance = GetGameInstance<UCustomGameInstance>()) {
		PlaneTransform = Instance->PlaneTransform;
	}

	TArray<AActor *> Planes;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AStaticMeshActor::StaticClass(), TEXT("MoveZone"), Planes);
	for (AActor *Plane : Planes) {
		PlaneTransform.SetScale3D(Plane->GetActorScale3D());
		Plane->SetActorTransform(PlaneTransform);
	}
}

AMonsterSpawner* ACustomGameMode::GetSpawner()
{
	return MonsterSpawner.Get();
}

void ACustomGameMode::OnPlayerDeath()
{
	if (auto Instance = Cast<UCustomGameInstance>(GetGameInstance())) {
		Instance->Score = MonsterSpawner->DeathCount;
	}

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("DeathMenu"));
}

void ACustomGameMode::PauseGame()
{
	if (!PauseMenu) {
		return;
	}

	PauseMenu->SetIsEnabled(true);
	PauseMenu->SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (auto Player = Cast<ACustomARPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		Player->SetPaused(true);
	}
}

void ACustomGameMode::ResumeGame()
{
	if (!PauseMenu) {
		return;
	}

	PauseMenu->SetIsEnabled(false);
	PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (auto Player = Cast<ACustomARPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		Player->SetPaused(false);
	}
}
