#include "MonsterSpawner.h"
#include "Kismet/GameplayStatics.h"

#include "LogUtils.h"
#include "CustomARPawn.h"
#include "CustomGameInstance.h"

// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
	// we never use Tick
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnMonster();
}

void AMonsterSpawner::SpawnMonster() 
{
	// don't spawn another monster if there are enough already
	if (MonsterCount >= MaxMonsters) {
		return;
	}
	++MonsterCount;

	// Spawn in random position 
	FVector Loc = {
		FMath::FRandRange(-1.f, 1.f),
		FMath::FRandRange(-1.f, 1.f),
		0.f
	};
	// get rotation so the monster faces the player
	FRotator Rot = FRotator::ZeroRotator;
	Rot.Yaw = Loc.Rotation().Yaw;
	// move the monster away enough from the player
	Loc *= MaxDistance;
	// get the plane transform
	FTransform PlaneT = FTransform::Identity;
	if (auto Instance = GetGameInstance<UCustomGameInstance>()) {
		PlaneT = Instance->PlaneTransform;
	}
	// move the monster on the plane
	FTransform MonsterT = PlaneT * FTransform(Rot, Loc);
	// put the monster on the ground
	MonsterT.AddToTranslation({ 0.f, 0.f, 92.f });

	auto Spawned = GetWorld()->SpawnActor<AMonster>(MonsterClass, MonsterT);
	if (!Spawned) {
		err("couldn't spawn monster");
		return;
	}

	Spawned->GetMesh()->SetAnimInstanceClass(AnimClass);

	float RandomTime = FMath::RandRange(TimeRangeMin, TimeRangeMax);

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		UnusedHandle,
		this,
		&AMonsterSpawner::SpawnMonster,
		RandomTime
	);
}

void AMonsterSpawner::OnMonsterDeath() 
{
	// if there is a free space now, spawn a new monster in a bit
	if (MonsterCount >= MaxMonsters) {
		float RandomTime = FMath::RandRange(TimeRangeMin, TimeRangeMax);
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle,
			this,
			&AMonsterSpawner::SpawnMonster,
			RandomTime
		);
	}

	if (MonsterCount) {
		--MonsterCount;
	}

	++DeathCount;
	DeathDelegate.Broadcast(DeathCount);
}

