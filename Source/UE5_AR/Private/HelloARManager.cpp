#include "HelloARManager.h"
#include "Kismet/GameplayStatics.h"
#include "ARBlueprintLibrary.h"
#include "ARSessionConfig.h"
#include "EngineUtils.h"
#include "ARPin.h"

#include "ARPlaneActor.h"
#include "LogUtils.h"

// Sets default values
AHelloARManager::AHelloARManager()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UARSessionConfig> ConfigAsset(TEXT("ARSessionConfig'/Game/Blueprints/HelloARSessionConfig.HelloARSessionConfig'"));
	Config = ConfigAsset.Object;

	//Populate the plane colours array
	PlaneColors.Add(FColor::Blue);
	PlaneColors.Add(FColor::Red);
	PlaneColors.Add(FColor::Green);
	PlaneColors.Add(FColor::Cyan);
	PlaneColors.Add(FColor::Magenta);
	PlaneColors.Add(FColor::Emerald);
	PlaneColors.Add(FColor::Orange);
	PlaneColors.Add(FColor::Purple);
	PlaneColors.Add(FColor::Turquoise);
	PlaneColors.Add(FColor::White);
	PlaneColors.Add(FColor::Yellow);
}

// Called when the game starts or when spawned
void AHelloARManager::BeginPlay()
{
	Super::BeginPlay();
	
	//Start the AR Session
	UARBlueprintLibrary::StartARSession(Config);
}

void AHelloARManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!bIsSetup) {
		UARBlueprintLibrary::StopARSession();
	}
}

// Called every frame
void AHelloARManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsSetup) {
		return;
	}

	switch (UARBlueprintLibrary::GetARSessionStatus().Status) {
	case EARSessionStatus::Running:
		UpdatePlaneActors();
		break;

	case EARSessionStatus::FatalError:
		ResetARCoreSession();
		UARBlueprintLibrary::StartARSession(Config);
		break;
	}
}

void AHelloARManager::Setup(bool IsSetup) {
	bIsSetup = IsSetup;
	SetActorTickEnabled(bIsSetup);
}

//Updates the geometry actors in the world
void AHelloARManager::UpdatePlaneActors()
{
	//Get all world geometries and store in an array
	auto Geometries = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	bool bFound = false;

	//Loop through all geometries
	for (auto& It : Geometries)
	{
		//Check if current plane exists 
		if (PlaneActors.Contains(It))
		{
			AARPlaneActor* CurrentPActor = *PlaneActors.Find(It);
			//Check if plane is subsumed
			if (It->GetSubsumedBy()->IsValidLowLevel())
			{
				CurrentPActor->Destroy();
				PlaneActors.Remove(It);
				break;
			}
			else
			{
				//Get tracking state switch
				switch (It->GetTrackingState())
				{
					//If tracking update
				case EARTrackingState::Tracking:
					CurrentPActor->UpdatePlanePolygonMesh();
					break;
					//If not tracking destroy the actor and remove from map of actors
				case EARTrackingState::StoppedTracking:
					CurrentPActor->Destroy();
					PlaneActors.Remove(It);
					break;
				}
			}
		}
		else
		{
			//Get tracking state switch
			switch (It->GetTrackingState())
			{

			case EARTrackingState::Tracking:
				if (!It->GetSubsumedBy()->IsValidLowLevel())
				{
					PlaneActor = GetWorld()->SpawnActor<AARPlaneActor>();
					PlaneActor->SetColor(GetPlaneColor(PlaneIndex));
					PlaneActor->ARCorePlaneObject = It;

					PlaneActors.Add(It, PlaneActor);
					PlaneActor->UpdatePlanePolygonMesh();
					PlaneIndex++;
				}
				break;
			}
		}
	}
}

//Gets the colour to set the plane to when its spawned
FColor AHelloARManager::GetPlaneColor(int Index)
{
	return PlaneColors[Index % PlaneColors.Num()];
}

void AHelloARManager::ResetARCoreSession()
{
	//Get all actors in the level and destroy them as well as emptying the respective arrays
	for (TActorIterator<AActor> It(GetWorld(), AARPlaneActor::StaticClass()); It; ++It) {
		It->Destroy();
	}
	
	PlaneActors.Empty();
}
