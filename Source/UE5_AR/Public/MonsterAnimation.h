#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Monster.h"
#include "MonsterAnimation.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAnimFinishedSignature, EMonsterState, Animation);

UCLASS()
class UE5_AR_API UMonsterAnimation : public UAnimInstance
{
	GENERATED_BODY()

public:
	FOnAnimFinishedSignature AnimDelegate;

protected:
	virtual void NativeBeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnHitAnimationFinished();

	UFUNCTION(BlueprintCallable)
	void OnDeathAnimationFinished();
	
	UFUNCTION(BlueprintCallable)
	void OnAttackAnimationHit();
	
	UFUNCTION(BlueprintCallable)
	void OnAttackAnimationFinished();

	UFUNCTION()
	void OnMonsterState(EMonsterState State);
	
	UFUNCTION()
	void OnPlayerSeen(bool CanSeePlayer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanSeePlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
};
