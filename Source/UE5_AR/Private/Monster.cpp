#include "Monster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"

#include "LogUtils.h"
#include "CustomGameMode.h"
#include "MonsterSpawner.h"
#include "MonsterAnimation.h"
#include "CustomARPawn.h"

// Sets default values
AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void AMonster::TakeHit() 
{
	if (!bCanBeHit) {
		return;
	}

	bCanMove = false;
	bCanBeHit = false;
	HitTaken++;
	StateDelegate.ExecuteIfBound(MonsterStateDamaged);
	if (IsDead()) {
		if (auto GameMode = GetWorld()->GetAuthGameMode<ACustomGameMode>()) {
			if (auto Spawner = GameMode->GetSpawner()) {
				Spawner->OnMonsterDeath();
			}
		}
		StateDelegate.ExecuteIfBound(MonsterStateDead);
	}
}

bool AMonster::IsDead() const {
	return HitTaken >= TotalHealth;
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	if (auto MonsterMesh = GetMesh()) {
		MonsterMesh->SetGenerateOverlapEvents(true);
		if (auto MonsterAnim = Cast<UMonsterAnimation>(MonsterMesh->GetAnimInstance())) {
			MonsterAnim->AnimDelegate.BindDynamic(this, &AMonster::OnAnimNotify);
		}
	}

	if (AudioComponent) {
		AudioComponent->AttenuationSettings = SoundAttenuation;
		AudioComponent->OnAudioFinished.AddDynamic(this, &AMonster::PlayRandomZombieNoise);
	}

	if (auto Capsule = GetCapsuleComponent()) {
		Capsule->SetVisibility(false);
	}

	Player = Cast<ACustomARPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	PlayRandomZombieNoise();
}

void AMonster::Tick(float DeltaTime) {
	if (!bCanMove) {
		return;
	}

	// check that the Player pointer is not null
	if (!Player) {
		Player = Cast<ACustomARPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		// are you *REEEAAALLY* sure that its not null?
		if (!Player) {
			return;
		}
	}

	FVector Pos = GetActorLocation();
	FVector PlayerToSelf = Player->GetPosition() - Pos;
	if (PlayerToSelf.SquaredLength() <= MaxAttackDistanceSquared) {
		// attack player
		bCanMove = false;
		StateDelegate.ExecuteIfBound(MonsterStateAttacking);
		return;
	}
	FVector Dir = PlayerToSelf.GetSafeNormal2D();
	FVector Movement = Dir * Speed * DeltaTime;
	SetActorRotation(Dir.Rotation());
	SetActorLocation(Pos + Movement);
}

void AMonster::OnAnimNotify(EMonsterState Animation)
{
	switch (Animation) {
	case MonsterStateAttacking:
		bCanMove = true;
		break;
	case MonsterStateAttackHit:
		// we use the capsule here instead of the usual mesh as it feels more fair
		// in a AR game, otherwise dodging is too hard
		if (Player) {
			FVector Pos = GetActorLocation();
			FVector PlayerToSelf = Player->GetPosition() - Pos;
			if (PlayerToSelf.SquaredLength() <= MaxAttackDistanceSquared) {
				Player->TakeDamage(DamagePower, {}, nullptr, nullptr);
			}
		}
		break;
	case MonsterStateDamaged:
		bCanBeHit = !IsDead();
		bCanMove = bCanBeHit;
		break;
	case MonsterStateDead:
		Destroy();
		break;
	}
}

void AMonster::PlayRandomZombieNoise()
{
	if (!AudioComponent || AudioComponent->IsPlaying()) {
		return;
	}

	if (ZombieSounds.IsEmpty()) {
		err("zombie sounds array is empty");
		return;
	}

	int32 RandomIndex = -1;
	do {
		RandomIndex = FMath::RandRange(0, ZombieSounds.Num() - 1);
	} while (RandomIndex == PreviousSound);
	
	PreviousSound = RandomIndex;
	AudioComponent->Sound = ZombieSounds[RandomIndex];
	AudioComponent->Play();
}
