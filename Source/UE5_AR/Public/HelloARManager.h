#pragma once

#include "GameFramework/Actor.h"
#include "HelloARManager.generated.h"

class UARSessionConfig;
class AARPlaneActor;
class UARPlaneGeometry;

UCLASS()
class UE5_AR_API AHelloARManager : public AActor
{
	GENERATED_BODY()

public:
	void Setup(bool IsSetup);
	
protected:
	AHelloARManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdatePlaneActors();

	FColor GetPlaneColor(int Index);
	void ResetARCoreSession();

	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComponent;
	
	TObjectPtr<UARSessionConfig> Config;

	TObjectPtr<AARPlaneActor> PlaneActor;

	//Map of geometry planes
	TMap<UARPlaneGeometry*, AARPlaneActor*> PlaneActors;

	//Index for plane colours adn array of colours
	int PlaneIndex = 0;
	TArray<FColor> PlaneColors;

	bool bIsSetup = false;
};
