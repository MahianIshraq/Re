// ReSharper disable CppMemberFunctionMayBeConst

#include "Player/RePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Camera/CameraComponent.h"
#include "Component/Player/RePlayerCameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "Game/ReGameInstance.h"
#include "Game/Manager/ReContextManager.h"
#include "Game/Manager/ReInputManager.h"
#include "Game/Manager/ReSaveManager.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/ICursor.h"
#include "HAL/Platform.h"
#include "Shared/RePIEFolders.h"
#include "System/ReAssetManager.h"
#include "Templates/Casts.h"
#include "Templates/SharedPointer.h"
#include "WorldPartition/WorldPartitionStreamingSource.h"

ARePlayerController::ARePlayerController()
{
	PrimaryActorTick.bCanEverTick			= true;
	PrimaryActorTick.bTickEvenWhenPaused	= true;
	PrimaryActorTick.bStartWithTickEnabled	= false;

	bAutoManageActiveCameraTarget = false;

	bEnableStreamingSource	= true;
	StreamingSourcePriority	= EStreamingSourcePriority::Highest;

	bEnableClickEvents		= false;
	bEnableTouchEvents		= false;
	bEnableMouseOverEvents	= false;
	bEnableTouchOverEvents	= false;

	CameraComponent = CreateDefaultSubobject<URePlayerCameraComponent>("CameraComponent");
}

void ARePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	IMC_Universal			= UReAssetManager::LoadAssetSync<UInputMappingContext>("IMC_Universal");
	IMC_Menu				= UReAssetManager::LoadAssetSync<UInputMappingContext>("IMC_Menu");
	IMC_Overworld			= UReAssetManager::LoadAssetSync<UInputMappingContext>("IMC_Overworld");

	IA_Escape				= UReAssetManager::LoadAssetSync<UInputAction>("IA_Escape");
	IA_ToggleInventory		= UReAssetManager::LoadAssetSync<UInputAction>("IA_ToggleInventory");
	IA_GamepadFocusUp		= UReAssetManager::LoadAssetSync<UInputAction>("IA_GamepadFocusUp");
	IA_GamepadFocusDown		= UReAssetManager::LoadAssetSync<UInputAction>("IA_GamepadFocusDown");
	IA_GamepadFocusLeft		= UReAssetManager::LoadAssetSync<UInputAction>("IA_GamepadFocusLeft");
	IA_GamepadFocusRight	= UReAssetManager::LoadAssetSync<UInputAction>("IA_GamepadFocusRight");
	IA_Interact				= UReAssetManager::LoadAssetSync<UInputAction>("IA_Interact");
	IA_Inspect				= UReAssetManager::LoadAssetSync<UInputAction>("IA_Inspect");
	IA_QuickSave			= UReAssetManager::LoadAssetSync<UInputAction>("IA_QuickSave");
	IA_QuickLoad			= UReAssetManager::LoadAssetSync<UInputAction>("IA_QuickLoad");

	IA_Escape				->bTriggerWhenPaused = true;
	IA_ToggleInventory		->bTriggerWhenPaused = true;
	IA_GamepadFocusUp		->bTriggerWhenPaused = true;
	IA_GamepadFocusDown		->bTriggerWhenPaused = true;
	IA_GamepadFocusLeft		->bTriggerWhenPaused = true;
	IA_GamepadFocusRight	->bTriggerWhenPaused = true;

	if (!GetWorld()->IsGameWorld())
	{
		return;
	}

	SetViewTarget(this);
}

void ARePlayerController::BeginPlay()
{
	Super::BeginPlay();

	ReGameInstance						= GetGameInstance<UReGameInstance>();
	EnhancedInputLocalPlayerSubsystem	= GetLocalPlayer()	->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	ContextManager						= ReGameInstance	->GetSubsystem<UReContextManager>();
	SaveManager							= ReGameInstance	->GetSubsystem<UReSaveManager>();
	InputManager						= ReGameInstance	->GetSubsystem<UReInputManager>();

	OnGameInitializedHandle	= ReGameInstance	->AddOnGameInitialized	(this, &ARePlayerController::OnGameInitialized);
	OnGameStartedHandle		= ReGameInstance	->AddOnGameStarted		(this, &ARePlayerController::OnGameStarted);
	OnContextAppliedHandle	= ContextManager	->AddOnContextApplied	(this, &ARePlayerController::OnContextApplied);
	OnInputEnabledHandle	= InputManager		->AddOnInputEnabled		(this, &ARePlayerController::OnInputEnabled);
	OnInputDisabledHandle	= InputManager		->AddOnInputDisabled	(this, &ARePlayerController::OnInputDisabled);

	ReGameInstance->InitializeGame();

	if (SaveManager->IsGameLoading())
	{
		SaveManager->ContinueGameLoad();
	}

	ReGameInstance->StartGame(this);

#if	WITH_EDITOR

	SetFolderPath(Re::PIEFolders::Player);
	SetActorLabel(TEXT("Player Controller"));

#endif
}

void ARePlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	ReGameInstance	->RemoveOnGameInitialized(OnGameInitializedHandle);
	ReGameInstance	->RemoveOnGameStarted(OnGameStartedHandle);
	ContextManager	->RemoveOnContextApplied(OnContextAppliedHandle);
	InputManager	->RemoveOnInputEnabled(OnInputEnabledHandle);
	InputManager	->RemoveOnInputDisabled(OnInputDisabledHandle);

	OnGameInitializedHandle	.Reset();
	OnGameStartedHandle		.Reset();
	OnContextAppliedHandle	.Reset();
	OnInputEnabledHandle	.Reset();
	OnInputDisabledHandle	.Reset();

	EnhancedInputLocalPlayerSubsystem->ClearAllMappings();

	ReGameInstance->EndGame();

	Super::EndPlay(Reason);
}

void ARePlayerController::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	CameraComponent->GetCameraView(DeltaTime, OutResult);
}

void ARePlayerController::GetStreamingSourceLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = CameraComponent->GetComponentLocation();
	OutRotation = CameraComponent->GetComponentRotation();
}

void ARePlayerController::SetActiveInputMappingContext(const UInputMappingContext* NewIMC)
{
	if (ActiveIMC == NewIMC)
	{
		return;
	}

	if (ActiveIMC)
	{
		EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(ActiveIMC);
	}

	ActiveIMC = NewIMC;

	if (ActiveIMC)
	{
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(NewIMC, 1);
	}
}

void ARePlayerController::OnGameInitialized()
{
	bShowMouseCursor	= true;
	DefaultMouseCursor	= EMouseCursor::Custom;
	CurrentMouseCursor	= EMouseCursor::Custom;

	SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false).SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	FSlateApplication::Get().SetNavigationConfig(MakeShared<FNullNavigationConfig>());

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Universal, 0);
}

void ARePlayerController::OnGameStarted(APlayerController*)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(IA_Escape,				ETriggerEvent::Started,		this, &ARePlayerController::EscapePressed);
	EnhancedInputComponent->BindAction(IA_Escape,				ETriggerEvent::Completed,	this, &ARePlayerController::EscapeReleased);
	EnhancedInputComponent->BindAction(IA_ToggleInventory,		ETriggerEvent::Started,		this, &ARePlayerController::ToggleInventoryPressed);
	EnhancedInputComponent->BindAction(IA_ToggleInventory,		ETriggerEvent::Completed,	this, &ARePlayerController::ToggleInventoryReleased);
	EnhancedInputComponent->BindAction(IA_GamepadFocusUp,		ETriggerEvent::Started,		this, &ARePlayerController::GamepadFocusUp);
	EnhancedInputComponent->BindAction(IA_GamepadFocusDown,		ETriggerEvent::Started,		this, &ARePlayerController::GamepadFocusDown);
	EnhancedInputComponent->BindAction(IA_GamepadFocusLeft,		ETriggerEvent::Started,		this, &ARePlayerController::GamepadFocusLeft);
	EnhancedInputComponent->BindAction(IA_GamepadFocusRight,	ETriggerEvent::Started,		this, &ARePlayerController::GamepadFocusRight);
	EnhancedInputComponent->BindAction(IA_Interact,				ETriggerEvent::Started,		this, &ARePlayerController::Interact);
	EnhancedInputComponent->BindAction(IA_Inspect,				ETriggerEvent::Started,		this, &ARePlayerController::Inspect);
	EnhancedInputComponent->BindAction(IA_QuickSave,			ETriggerEvent::Started,		this, &ARePlayerController::QuickSavePressed);
	EnhancedInputComponent->BindAction(IA_QuickSave,			ETriggerEvent::Completed,	this, &ARePlayerController::QuickSaveReleased);
	EnhancedInputComponent->BindAction(IA_QuickLoad,			ETriggerEvent::Started,		this, &ARePlayerController::QuickLoadPressed);
	EnhancedInputComponent->BindAction(IA_QuickLoad,			ETriggerEvent::Completed,	this, &ARePlayerController::QuickLoadReleased);
}

void ARePlayerController::OnContextApplied(EReContext Context)
{
	switch (Context)
	{
		case EReContext::Overworld:
		{
			SetPause(false);
			SetActiveInputMappingContext(IMC_Overworld);
			break;
		}

		default:
		{
			SetPause(true);
			SetActiveInputMappingContext(IMC_Menu);
			break;
		}
	}
}

void ARePlayerController::OnInputEnabled()
{
	SetActorTickEnabled(true);
	EnableInput(nullptr);
}

void ARePlayerController::OnInputDisabled()
{
	DisableInput(nullptr);
	SetActorTickEnabled(false);
}

void ARePlayerController::EscapePressed()
{
	InputManager->SetActiveClickableInputAction(IA_Escape);
}

void ARePlayerController::EscapeReleased()
{
	if (InputManager->TryConsumeActiveClickableInputAction(IA_Escape))
	{
		ContextManager->Escape();
	}
}

void ARePlayerController::ToggleInventoryPressed()
{
	InputManager->SetActiveClickableInputAction(IA_ToggleInventory);
}

void ARePlayerController::ToggleInventoryReleased()
{
	if (InputManager->TryConsumeActiveClickableInputAction(IA_ToggleInventory))
	{
		ContextManager->ToggleInventory();
	}
}

void ARePlayerController::GamepadFocusUp()
{
	
}

void ARePlayerController::GamepadFocusDown()
{
	
}

void ARePlayerController::GamepadFocusLeft()
{
	
}

void ARePlayerController::GamepadFocusRight()
{
	
}

void ARePlayerController::Interact()
{
	
}

void ARePlayerController::Inspect()
{
	
}

void ARePlayerController::QuickSavePressed()
{
	InputManager->SetActiveClickableInputAction(IA_QuickSave);
}

void ARePlayerController::QuickSaveReleased()
{
	if (InputManager->TryConsumeActiveClickableInputAction(IA_QuickSave))
	{
		SaveManager->StartGameSave();
	}
}

void ARePlayerController::QuickLoadPressed()
{
	InputManager->SetActiveClickableInputAction(IA_QuickLoad);
}

void ARePlayerController::QuickLoadReleased()
{
	if (InputManager->TryConsumeActiveClickableInputAction(IA_QuickLoad))
	{
		SaveManager->StartGameLoad();
	}
}
