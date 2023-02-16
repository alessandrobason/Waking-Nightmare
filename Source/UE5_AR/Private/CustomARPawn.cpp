#include "CustomARPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h"
#include "ARBlueprintLibrary.h"
#include "ARLightEstimate.h"
#include "EngineUtils.h"
#include "TemplateSequence.h"
#include "TemplateSequencePlayer.h"
#include "TemplateSequenceActor.h"

#include "CustomGameMode.h"
#include "Monster.h"
#include "LogUtils.h"

static constexpr float FromCandelas(float Candelas) {
	return Candelas * 625.f;
}

// Sets default values
ACustomARPawn::ACustomARPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	if (!SpotLight) {
		SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
		SpotLight->SetCastShadows(false);
		SpotLight->SetMobility(EComponentMobility::Movable);
		SpotLight->SetIntensity(FromCandelas(50.f));
		SpotLight->SetupAttachment(CameraComponent);
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> ConeAsset(TEXT("/Engine/BasicShapes/Cone"));

	if (ConeAsset.Succeeded()) {
		ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
		// set mesh as cone
		ConeMesh->SetStaticMesh(ConeAsset.Object);
		// make invisible
		ConeMesh->SetVisibility(false);
		// attach to light, so it follows camera
		ConeMesh->SetupAttachment(SpotLight);
		// make it ignore all collisions apart from dynamic overlap
		ConeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//ConeMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		ConeMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		// make sure it actually generates the events
		ConeMesh->SetGenerateOverlapEvents(true);
		// set correct transforms
		FTransform ConeTransform;
		ConeTransform.SetLocation({ 250, 0, 0 });
		ConeTransform.SetRotation(FRotator(90, 0, 0).Quaternion());
		ConeTransform.SetScale3D({ 5, 5, 5 });
		ConeMesh->SetWorldTransform(ConeTransform);
	}
}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();

	if (FlashSequence && !FlashPlayer) {
		ATemplateSequenceActor *OutActor = nullptr;
		FlashPlayer = UTemplateSequencePlayer::CreateTemplateSequencePlayer(this, FlashSequence, {}, OutActor);

		if (OutActor) {
			OutActor->SetBinding(this);
		}
	}

	if (!CameraUI) {
		CameraUI = CreateWidget<UUserWidget>(GetWorld(), CameraUIClass);
	}

	if (CameraUI) {
		CameraUI->AddToViewport();
	}
}

// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InterpolatedHealth != ActualHealth) {
		const float Total = FMath::Abs(StartLerpHealth - ActualHealth);
		const float Current = FMath::Abs(InterpolatedHealth - StartLerpHealth);
		const float Alpha = FMath::Min(Current / Total, 1.f);
		InterpolatedHealth = FMath::Lerp(InterpolatedHealth, ActualHealth, Alpha);
		HealthChangeDelegate.Broadcast(InterpolatedHealth / 100.f);
		// update again if Alpha < 1.f
		if (Alpha >= 1.f) {
			InterpolatedHealth = StartLerpHealth = ActualHealth;
			bUpdatingHealth = false;
		}
	}
}

TObjectPtr<USceneComponent> ACustomARPawn::GetTransformComponent() {
	return CameraComponent;
}

FVector ACustomARPawn::GetPosition() 
{
	if (auto SceneComp = GetTransformComponent()) {
		return SceneComp->GetComponentLocation();
	}
	return FVector::Zero();
}

//float ACustomARPawn::GetHealth() const
//{
//	return ActualHealth;
//}

float ACustomARPawn::GetHealthNorm() const
{
	return ActualHealth / 100.f;
}

void ACustomARPawn::OnShutterClick() 
{
	if (FlashPlayer && !FlashPlayer->IsPlaying()) {
		FlashPlayer->Play();
		UGameplayStatics::PlaySound2D(GetWorld(), CameraShutterNoise);
		if (ConeMesh) {
			TArray<AActor *> Overlapping;
			ConeMesh->GetOverlappingActors(Overlapping, AMonster::StaticClass());
			for (const auto &Actor : Overlapping) {
				if (auto Monster = Cast<AMonster>(Actor)) {
					Monster->TakeHit();
				}
			}
		}
	}
}

void ACustomARPawn::SetPaused(bool IsPaused)
{
	if (!CameraUI) {
		return;
	}

	CameraUI->SetIsEnabled(!IsPaused);
	CameraUI->SetVisibility(IsPaused ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

float ACustomARPawn::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FVector PlayerLoc = GetCollisionComponent()->GetComponentLocation();

	ActualHealth -= DamageAmount;
	if (ActualHealth <= 0.f) {
		ActualHealth = 0.f;
		if (auto GameMode = Cast<ACustomGameMode>(UGameplayStatics::GetGameMode(GetWorld()))) {
			GameMode->OnPlayerDeath();
		}
	}

	if (!bUpdatingHealth) {
		StartLerpHealth = ActualHealth;
		bUpdatingHealth = true;
	}

	return DamageAmount;
}
