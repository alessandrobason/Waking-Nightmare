#pragma once

#include "GameFramework/Pawn.h"
#include "GameFramework/DefaultPawn.h"
#include "CustomARPawn.generated.h"

class UCameraComponent;
class USpotLightComponent;
class UTemplateSequence;
class UTemplateSequencePlayer;
class USceneComponent;
class AController;
class UUserWidget;
class USoundBase;
struct FDamageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHealthSignature, float, HealthNormalized);

UCLASS()
class UE5_AR_API ACustomARPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OnShutterClick();

	void SetPaused(bool IsPaused);

	FVector GetPosition();

	UFUNCTION(BlueprintPure)
	float GetHealthNorm() const;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;

	UPROPERTY(Category = "Health", BlueprintAssignable)
	FOnPlayerHealthSignature HealthChangeDelegate;

protected:
	ACustomARPawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	TObjectPtr<USceneComponent> GetTransformComponent();

	float ActualHealth = 100.f;
	float InterpolatedHealth = 100.f;
	float StartLerpHealth = 100.f;
	bool bUpdatingHealth = false;

	UPROPERTY()
	TObjectPtr<UUserWidget> CameraUI = nullptr;

	UPROPERTY(Category = "AR", VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(Category = "AR", VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(Category = "Flash", VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpotLightComponent> SpotLight;

	UPROPERTY(Category = "Flash", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTemplateSequence> FlashSequence;

	UPROPERTY(Category = "Flash", VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UTemplateSequencePlayer> FlashPlayer;

	UPROPERTY(Category = "Flash", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> ConeMesh;

	UPROPERTY(Category = "Flash", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> CameraUIClass;

	UPROPERTY(Category = "Flash", EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> CameraShutterNoise;

	UPROPERTY(Category = "Health", EditAnywhere, BlueprintReadWrite)
	float RegenerationSpeed = 10.f;
};
