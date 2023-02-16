#include "MonsterAnimation.h"

void UMonsterAnimation::NativeBeginPlay()
{
	if (auto Monster = Cast<AMonster>(TryGetPawnOwner())) {
		//Monster->FollowDelegate.BindDynamic(this, &UMonsterAnimation::OnPlayerSeen);
		Monster->StateDelegate.BindDynamic(this, &UMonsterAnimation::OnMonsterState);
	}
}

void UMonsterAnimation::OnHitAnimationFinished() 
{
	bIsHit = false;
	bIsAttacking = false;
	AnimDelegate.ExecuteIfBound(MonsterStateDamaged);
}

void UMonsterAnimation::OnDeathAnimationFinished() 
{
	AnimDelegate.ExecuteIfBound(MonsterStateDead);
}

void UMonsterAnimation::OnAttackAnimationHit()
{
	AnimDelegate.ExecuteIfBound(MonsterStateAttackHit);
}

void UMonsterAnimation::OnAttackAnimationFinished()
{
	bIsAttacking = false;
	AnimDelegate.ExecuteIfBound(MonsterStateAttacking);
}

void UMonsterAnimation::OnMonsterState(EMonsterState State)
{
	switch (State) {
	case MonsterStateAttacking:
		bIsAttacking = true;
		break;
	case MonsterStateDamaged:
		bIsHit = true;
		break;
	case MonsterStateDead:
		bIsAlive = false;
		break;
	}
}

void UMonsterAnimation::OnPlayerSeen(bool CanSeePlayer) 
{
	bCanSeePlayer = CanSeePlayer;
}
