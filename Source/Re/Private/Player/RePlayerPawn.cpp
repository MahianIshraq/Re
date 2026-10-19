#include "Player/RePlayerPawn.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Component/Player/RePlayerPawnMoveComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Game/ReSave.h"
#include "Game/Manager/ReSaveManager.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "HAL/Platform.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Math/Color.h"
#include "Math/Quat.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Shared/ReCollisionChannels.h"
#include "Shared/RePIEFolders.h"
#include "System/ReAssetManager.h"
#include "System/ReDebug.h"

ARePlayerPawn::ARePlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent		= CreateDefaultSubobject<UCapsuleComponent>				("CapsuleComponent"	);
	SkeletalMeshComponent	= CreateDefaultSubobject<USkeletalMeshComponent>		("SkeletalMeshComponent");
	MovementPlaneComponent	= CreateDefaultSubobject<UStaticMeshComponent>			("MovementPlaneComponent");
	PlayerPawnMoveComponent	= CreateDefaultSubobject<URePlayerPawnMoveComponent>	("PlayerPawnMoveComponent");

	RootComponent			= CapsuleComponent;
	SkeletalMeshComponent	->SetupAttachment(RootComponent);
	MovementPlaneComponent	->SetupAttachment(RootComponent);

	CapsuleComponent->SetCapsuleSize			(Radius, HalfHeight);
	CapsuleComponent->SetCollisionProfileName	(UCollisionProfile::Pawn_ProfileName);

	SkeletalMeshComponent->SetRelativeLocation	(FVector(0.0f, 0.0f, -HalfHeight));
	SkeletalMeshComponent->SetRelativeRotation	(FRotator(0.0f, -90.0f, 0.0f));
	SkeletalMeshComponent->SetAnimationMode		(EAnimationMode::AnimationBlueprint);

	MovementPlaneComponent->SetRelativeLocation					(FVector(0.0f, 0.0f, HalfHeight));
	MovementPlaneComponent->SetRelativeScale3D					(FVector(100.0f, 100.0f, 1.0f));
	MovementPlaneComponent->SetVisibility						(false);
	MovementPlaneComponent->SetCollisionEnabled					(ECollisionEnabled::QueryOnly);
	MovementPlaneComponent->SetCollisionResponseToAllChannels	(ECR_Ignore);
	MovementPlaneComponent->SetCollisionResponseToChannel		(Re::CollisionChannels::MovementPlane, ECR_Block);
}

void ARePlayerPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SkeletalMeshComponent	->SetSkeletalMesh		(UReAssetManager::LoadAssetSync				<USkeletalMesh>					("SKM_Mannequin"));
	SkeletalMeshComponent	->SetAnimInstanceClass	(UReAssetManager::LoadBlueprintClassSync	<UAnimInstance, UAnimBlueprint>	("ABP_Mannequin"));
	MovementPlaneComponent	->SetStaticMesh			(UReAssetManager::LoadAssetSync				<UStaticMesh>					("SM_Plane"));
}

void ARePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	SaveManager	= GetGameInstance()->GetSubsystem<UReSaveManager>();
	MPC_Player	= GetWorld()->GetParameterCollectionInstance(UReAssetManager::LoadAssetSync<UMaterialParameterCollection>(MPC_Player_Name));

	OnGameSaveStartedHandle	= SaveManager->AddOnGameSaveStarted	(this, &ARePlayerPawn::OnGameSaveStarted);
	OnGameLoadedHandle		= SaveManager->AddOnGameLoaded		(this, &ARePlayerPawn::OnGameLoaded);

#if WITH_EDITOR

	SetFolderPath(Re::PIEFolders::Player);
	SetActorLabel(TEXT("Player Pawn"));

#endif
}

void ARePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MPC_Player->SetVectorParameterValue(MPC_Player_LocationParameterName, GetActorLocation());

#if !UE_BUILD_SHIPPING

	if (Re::Debug::Console::CVarEnableDebugView.GetValueOnGameThread())
	{
		DrawDebugCapsule(GetWorld(), GetActorLocation(), HalfHeight, Radius, FQuat::Identity, Re::Debug::Colors::Collision.ToFColor(false));
	}

#endif
}

void ARePlayerPawn::EndPlay(EEndPlayReason::Type Reason)
{
	SaveManager->RemoveOnGameSaveStarted	(OnGameSaveStartedHandle);
	SaveManager->RemoveOnGameLoaded			(OnGameLoadedHandle);

	OnGameSaveStartedHandle	.Reset();
	OnGameLoadedHandle		.Reset();

	Super::EndPlay(Reason);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ARePlayerPawn::OnGameSaveStarted(UReSave* Save)
{
	Save->PlayerPawnTransform = GetActorTransform();
}

void ARePlayerPawn::OnGameLoaded(const UReSave* Save)
{
	SetActorTransform(Save->PlayerPawnTransform);
}
