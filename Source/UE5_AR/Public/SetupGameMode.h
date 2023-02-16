#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SetupGameMode.generated.h"

class AHelloARManager;
class ADefaultPawn;

UENUM(BlueprintType)
enum ESetupState {
	SetupStateStart,
	SetupStatePinSelected,
	SetupStatePlaneChosen
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetupStateSignature, ESetupState, State);

UCLASS()
class UE5_AR_API ASetupGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void SelectPlane(const FVector& ScreenPos);

	UFUNCTION(BlueprintCallable)
	void ResetPlane();

	UFUNCTION(BlueprintCallable)
	void UpdatePlane(FTransform Transform);

	UPROPERTY(Category="Setup", EditAnywhere)
	TObjectPtr<UStaticMeshComponent> PlaneMesh;

	UPROPERTY(Category="Setup", EditAnywhere)
	TObjectPtr<UMaterialInstance> BlinkMaterial;

	UPROPERTY(Category="Setup", EditAnywhere)
	FVector3d PlaneScale = FVector3d::OneVector;

	UPROPERTY(Category="Setup", BlueprintAssignable)
	FOnSetupStateSignature SetupDelegate;

protected:
	ASetupGameMode();
	virtual void StartPlay() override;

	UPROPERTY()
	TObjectPtr<AHelloARManager> ARManager;

	UPROPERTY()
	TObjectPtr<ADefaultPawn> SpawnedPawn;

	bool bIsPlaneSelected = false;
};
