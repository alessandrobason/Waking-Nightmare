#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "Monster.generated.h"

class ACustomARPawn;

UENUM()
enum EMonsterState {
	MonsterStateNone,
	MonsterStateAttacking,
	MonsterStateAttackHit,
	MonsterStateDamaged,
	MonsterStateDead
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMonsterStateSignature, EMonsterState, State);

UCLASS()
class UE5_AR_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	void TakeHit();
	bool IsDead() const;

	FOnMonsterStateSignature StateDelegate;

protected:
	AMonster();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnAnimNotify(EMonsterState Animation);

	UFUNCTION()
	void PlayRandomZombieNoise();

	bool bCanBeHit = true;
	bool bCanMove = true;
	//FVector PlayerPos = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<ACustomARPawn> Player;

	int32 HitTaken = 0;
	int32 PreviousSound = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Health")
	int32 TotalHealth = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack")
	float Speed = 50.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Attack")
	float DamagePower = 20.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Attack")
	float MaxAttackDistanceSquared = 30000.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Sounds")
	TObjectPtr<UAudioComponent> AudioComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundAttenuation> SoundAttenuation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sounds")
	TArray<TObjectPtr<USoundBase>> ZombieSounds;
};
