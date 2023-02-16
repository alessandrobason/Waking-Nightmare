#include "SetupGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DefaultPawn.h"
#include "ARBlueprintLibrary.h"
#include "ARPin.h"

#include "CustomGameInstance.h"
#include "SetupPawn.h"
#include "HelloARManager.h"
#include "LogUtils.h"

ASetupGameMode::ASetupGameMode() 
{
	DefaultPawnClass = ASetupPawn::StaticClass();

	PlaneScale = { 6.f, 6.f, 0.2f };
	BlinkMaterial = nullptr;

	ConstructorHelpers::FObjectFinder<UMaterialInstance> BlinkAsset(TEXT("MaterialInstanceConstant'/Game/Assets/Materials/BlinkMaterial_Inst.BlinkMaterial_Inst'"));
	if (BlinkAsset.Succeeded()) {
		BlinkMaterial = BlinkAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneAsset(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneAsset.Succeeded()) {
		PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
		PlaneMesh->SetStaticMesh(PlaneAsset.Object);
		PlaneMesh->SetVisibility(false);
	}
}

void ASetupGameMode::StartPlay()
{
	Super::StartPlay();

	if (auto World = GetWorld()) {
		ARManager = World->SpawnActor<AHelloARManager>();
		ARManager->Setup(true);
	}

	if (PlaneMesh) {
		PlaneMesh->SetVisibility(true);
	}
}

void ASetupGameMode::SelectPlane(const FVector &ScreenPos) 
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;
	FHitResult HitResult;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);
	
	if (SpawnedPawn) {
		FVector TraceEnd = WorldPos + WorldDir * 1000.f;
		bool TraceSuccess = SpawnedPawn->ActorLineTraceSingle(HitResult, WorldPos, TraceEnd, ECollisionChannel::ECC_WorldDynamic, FCollisionQueryParams::DefaultQueryParam);
		if (TraceSuccess) {
			if (auto Instance = GetGameInstance<UCustomGameInstance>()) {
				Instance->PlaneTransform = SpawnedPawn->GetTransform();
				Instance->PlaneTransform.SetScale3D(FVector::OneVector);
			}
			SetupDelegate.Broadcast(SetupStatePlaneChosen);
			return;
		}
	}
		
	// Notice that this LineTrace is in the ARBluePrintLibrary - this means that it's exclusive only for objects tracked by ARKit/ARCore
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);

	//Checks if the location is valid
	if (TraceResult.IsValidIndex(0)) {
		// Get the first found object in the line trace - ignoring the rest of the array elements
		FTransform TrackedTransform = TraceResult[0].GetLocalToWorldTransform();

		if (FVector::DotProduct(TrackedTransform.GetRotation().GetUpVector(), WorldDir) < 0) {
			//Spawn the actor pin and get the transform
			UARPin *ActorPin = UARBlueprintLibrary::PinComponent(
				nullptr, 
				TraceResult[0].GetLocalToWorldTransform(), 
				TraceResult[0].GetTrackedGeometry()
			);

			// Check if ARPins are available on your current device. ARPins are currently not supported locally by ARKit, so on iOS, this will always be "FALSE" 
			if (ActorPin) {
				UpdatePlane(ActorPin->GetLocalToWorldTransform());
			}
			// IF ARPins are Not supported locally (for iOS Devices) We will spawn the object in the location where the line trace has hit
			else {
				UpdatePlane(TrackedTransform);
			}

			SetupDelegate.Broadcast(SetupStatePinSelected);
		}
	}
	else {
		// otherwise reset the plane, this way the player can click on a random point
		// to deselect the current pin
		ResetPlane();
		SetupDelegate.Broadcast(SetupStateStart);
	}
}

void ASetupGameMode::ResetPlane()
{
	if (SpawnedPawn) {
		SpawnedPawn->Destroy();
		SpawnedPawn = nullptr;
	}
}

void ASetupGameMode::UpdatePlane(FTransform Transform) 
{
	//Spawn a new Actor at the location if not done yet
	if (!SpawnedPawn) {
		SpawnedPawn = GetWorld()->SpawnActor<ADefaultPawn>();
	}

	if (SpawnedPawn) {
		Transform.SetScale3D(PlaneScale);
		// move the plane a bit up so it doesn't collide with floor
		Transform.AddToTranslation({ 0.f, 0.f, 20.f });
		SpawnedPawn->SetActorTransform(Transform);
		if (auto Mesh = SpawnedPawn->GetMeshComponent()) {
			Mesh->SetStaticMesh(PlaneMesh->GetStaticMesh());
			Mesh->SetMaterial(0, BlinkMaterial);
		}
	}
}
