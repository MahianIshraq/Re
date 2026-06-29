#include "Player/PlayerPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"
#include "Struct/Asset/FSkeletalMeshPaths.h"
#include "Animation/AnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Struct/Asset/FStaticMeshPaths.h"
#include "Struct/Shared/FTraceChannels.h"
#include "Player/PlayerMovementComponent.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Struct/Asset/FMPCPaths.h"
#include "EnhancedInputComponent.h"
#include "Player/PlayerCamera.h"
#include "GameFramework/PlayerController.h"
#include "Game/Manager/SaveManager.h"
#include "Engine/GameInstance.h"
#include "Game/Manager/InputManager.h"
#include "Engine/World.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Game/ReSaveGame.h"
#include "Game/ReLog.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// CapsuleComponent

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	
	CapsuleComponent->SetCapsuleSize(Radius, HalfHeight);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	// SkeletalMeshComponent

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	
	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -HalfHeight));
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	SkeletalMeshComponent->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(FSkeletalMeshPaths::SKM_Mannequin).Object);
	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SkeletalMeshComponent->SetAnimInstanceClass(ConstructorHelpers::FClassFinder<UAnimInstance>(FSkeletalMeshPaths::ABP_Mannequin).Class);

	// MovementPlaneComponent
	
	MovementPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovementPlaneComponent"));
	MovementPlaneComponent->SetupAttachment(RootComponent);
	
	MovementPlaneComponent->SetRelativeLocation(FVector(0.0f, 0.0f, HalfHeight));
	MovementPlaneComponent->SetRelativeScale3D(FVector(100.0f, 100.0f, 1.0f));
	
	MovementPlaneComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(FStaticMeshPaths::SM_Plane).Object);
	MovementPlaneComponent->SetVisibility(false);
	MovementPlaneComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MovementPlaneComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MovementPlaneComponent->SetCollisionResponseToChannel(FTraceChannels::MovementPlane, ECR_Block);

	// PlayerMovementComponent

	PlayerMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));

	// WorldPartitionStreamingSourceComponent

	WorldPartitionStreamingSourceComponent = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(TEXT("WorldPartitionStreamingSourceComponent"));
	WorldPartitionStreamingSourceComponent->EnableStreamingSource();
	WorldPartitionStreamingSourceComponent->Priority = EStreamingSourcePriority::Highest;

	// MPC
	
	MPC_Player = ConstructorHelpers::FObjectFinder<UMaterialParameterCollection>(FMPCPaths::MPC_Player).Object;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InInputComponent);

	// PlayerCamera

	PlayerCamera = GetWorld()->SpawnActor<APlayerCamera>();
	PlayerCamera->SetupInput(EnhancedInputComponent);

	// PlayerMovementComponent
	
	PlayerMovementComponent->SetupInput(EnhancedInputComponent);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// PlayerCamera

	APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	PlayerCamera->Setup(this, PlayerController);
	PlayerController->SetViewTarget(PlayerCamera);
	RE_LOG("View target set to PlayerCamera.");

	// PlayerMovementComponent

	PlayerMovementComponent->Setup(this, PlayerCamera, HalfHeight, Radius);

	// Delegate Bindings

	USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>();
	UInputManager* InputManager = GetGameInstance()->GetSubsystem<UInputManager>();
	
	SaveManager->OnGameSavedDelegate.AddUObject(this, &APlayerPawn::OnGameSavedListener);
	SaveManager->OnGameLoadedDelegate.AddUObject(this, &APlayerPawn::OnGameLoadedListener);
	InputManager->OnInputEnabledDelegate.AddUObject(this, &APlayerPawn::OnInputEnabledListener);
	InputManager->OnInputDisabledDelegate.AddUObject(this, &APlayerPawn::OnInputDisabledListener);
	
#if WITH_EDITOR

	SetFolderPath(FName(TEXT("[2] Player")));
	SetActorLabel(TEXT("Player Pawn"));

#endif
}

void APlayerPawn::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
	
	GetWorld()->GetParameterCollectionInstance(MPC_Player)->SetVectorParameterValue
	(
		FMPCPaths::MPC_Player_LocationParameterName,
		GetActorLocation()
	);
}

void APlayerPawn::OnGameSavedListener(UReSaveGame* InSaveGame)
{
	InSaveGame->PlayerPawnTransform = GetActorTransform();
}

void APlayerPawn::OnGameLoadedListener(const UReSaveGame* InSaveGame)
{
	SetActorTransform(InSaveGame->PlayerPawnTransform);
}

void APlayerPawn::OnInputEnabledListener()
{
	EnableInput(nullptr);
}

void APlayerPawn::OnInputDisabledListener()
{
	DisableInput(nullptr);
}