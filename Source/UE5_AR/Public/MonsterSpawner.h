#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monster.h"
#include "MonsterSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeathSignature, int32, DeathCount);

UCLASS()
class UE5_AR_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	void OnMonsterDeath();

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintAssignable)
	FOnMonsterDeathSignature DeathDelegate;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	int32 DeathCount = 0;

protected:
	AMonsterSpawner();
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnMonster();

	UPROPERTY(Category = "Spawner", VisibleAnywhere, BlueprintReadOnly)
	int32 MonsterCount = 0;
	
	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMonster> MonsterClass;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	int32 MaxMonsters = 10;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 400.f;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	float TimeRangeMin = 2.f;

	UPROPERTY(Category = "Spawner", EditAnywhere, BlueprintReadWrite)
	float TimeRangeMax = 5.f;
};
