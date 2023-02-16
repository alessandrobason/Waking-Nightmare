#pragma once

#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class AHelloARManager;
class AMonsterSpawner;

UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AMonsterSpawner* GetSpawner();

	void OnPlayerDeath();

	UFUNCTION(BlueprintCallable)
	void PauseGame();

	UFUNCTION(BlueprintCallable)
	void ResumeGame();

protected:
	ACustomGameMode();
	virtual void StartPlay() override;
	void SetupPlayableZone();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Classes")
	TSubclassOf<AMonsterSpawner> SpawnerClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Classes")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<AHelloARManager> ARManager = nullptr;
	
	UPROPERTY()
	TObjectPtr<AMonsterSpawner> MonsterSpawner = nullptr;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenu = nullptr;
};
