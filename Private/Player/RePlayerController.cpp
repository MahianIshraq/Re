#include "Player/RePlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "Struct/Asset/FInputAssetPaths.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "Game/Manager/SaveManager.h"
#include "Engine/GameInstance.h"
// #include "Game/Manager/InteractionManager.h"
#include "Game/Manager/InputManager.h"
#include "Game/Manager/ContextManager.h"
// #include "Game/Manager/FocusManager.h"
#include "Game/Manager/PointerManager.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"

ARePlayerController::ARePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// IMC
	
	IMC_Universal = ConstructorHelpers::FObjectFinder<UInputMappingContext>(FInputAssetPaths::IMC_Universal).Object;
	IMC_Menu = ConstructorHelpers::FObjectFinder<UInputMappingContext>(FInputAssetPaths::IMC_Menu).Object;
	IMC_Overworld = ConstructorHelpers::FObjectFinder<UInputMappingContext>(FInputAssetPaths::IMC_Overworld).Object;

	// IA | Universal

	IA_Escape = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_Escape).Object;
	IA_ToggleInventory = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_ToggleInventory).Object;
	
	IA_Escape->bTriggerWhenPaused = true;
	IA_ToggleInventory->bTriggerWhenPaused = true;

	// IA | Menu

	IA_GamepadFocusUp = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadFocusUp).Object;
	IA_GamepadFocusDown = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadFocusDown).Object;
	IA_GamepadFocusLeft = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadFocusLeft).Object;
	IA_GamepadFocusRight = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadFocusRight).Object;

	IA_GamepadFocusUp->bTriggerWhenPaused = true;
	IA_GamepadFocusDown->bTriggerWhenPaused = true;
	IA_GamepadFocusLeft->bTriggerWhenPaused = true;
	IA_GamepadFocusRight->bTriggerWhenPaused = true;

	// IA | Overworld
	
	IA_Interact = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_Interact).Object;
	IA_Inspect = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_Inspect).Object;
	IA_QuickSave = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_QuickSave).Object;
	IA_QuickLoad = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_QuickLoad).Object;
}

void ARePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// IA | Universal
	
	EnhancedInputComponent->BindAction(IA_Escape, ETriggerEvent::Started, this, &ARePlayerController::EscapePressed);
	EnhancedInputComponent->BindAction(IA_Escape, ETriggerEvent::Completed, this, &ARePlayerController::EscapeReleased);
	EnhancedInputComponent->BindAction(IA_ToggleInventory, ETriggerEvent::Started, this, &ARePlayerController::ToggleInventoryPressed);
	EnhancedInputComponent->BindAction(IA_ToggleInventory, ETriggerEvent::Completed, this, &ARePlayerController::ToggleInventoryReleased);

	// IA | Menu

	EnhancedInputComponent->BindAction(IA_GamepadFocusUp, ETriggerEvent::Started, this, &ARePlayerController::GamepadFocusUp);
	EnhancedInputComponent->BindAction(IA_GamepadFocusDown, ETriggerEvent::Started, this, &ARePlayerController::GamepadFocusDown);
	EnhancedInputComponent->BindAction(IA_GamepadFocusLeft, ETriggerEvent::Started, this, &ARePlayerController::GamepadFocusLeft);
	EnhancedInputComponent->BindAction(IA_GamepadFocusRight, ETriggerEvent::Started, this, &ARePlayerController::GamepadFocusRight);
	
	// IA | Overworld

	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ARePlayerController::Interact);
	EnhancedInputComponent->BindAction(IA_Inspect, ETriggerEvent::Started, this, &ARePlayerController::Inspect);
	EnhancedInputComponent->BindAction(IA_QuickSave, ETriggerEvent::Started, this, &ARePlayerController::QuickSavePressed);
	EnhancedInputComponent->BindAction(IA_QuickSave, ETriggerEvent::Completed, this, &ARePlayerController::QuickSaveReleased);
	EnhancedInputComponent->BindAction(IA_QuickLoad, ETriggerEvent::Started, this, &ARePlayerController::QuickLoadPressed);
	EnhancedInputComponent->BindAction(IA_QuickLoad, ETriggerEvent::Completed, this, &ARePlayerController::QuickLoadReleased);
}

void ARePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PreInit();
	
	USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>();
	
	if (SaveManager->IsGameLoading())
	{
		SaveManager->OnGameLoadCompleteDelegate.AddWeakLambda
		(
			this, [this] { Init(); }
		);
		
		SaveManager->CompleteGameLoad();
	}
	
	else
	{
		Init();
	}

#if	WITH_EDITOR
	
	SetFolderPath(FName(TEXT("[2] Player")));
	SetActorLabel(TEXT("Player Controller"));
	
#endif
}

void ARePlayerController::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
	// GetGameInstance()->GetSubsystem<UInteractionManager>()->TickInteraction(this);
}

void ARePlayerController::OnContextChangedListener(EContext InContext)
{
	
	
	switch (InContext)
	{
		case EContext::Overworld:
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

void ARePlayerController::OnGameLoadStartedListener()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->DisableInput();
	SetActorTickEnabled(false);
	GetGameInstance()->GetSubsystem<UContextManager>()->SetContext(EContext::None);
}

void ARePlayerController::OnInputEnabledListener()
{
	EnableInput(nullptr);
}

void ARePlayerController::OnInputDisabledListener()
{
	DisableInput(nullptr);
}

void ARePlayerController::EscapePressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_Escape);
}

void ARePlayerController::EscapeReleased()
{
	if (GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_Escape))
	{
		GetGameInstance()->GetSubsystem<UContextManager>()->Escape();
	}
}

void ARePlayerController::ToggleInventoryPressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_ToggleInventory);
}

void ARePlayerController::ToggleInventoryReleased()
{
	if (GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_ToggleInventory))
	{
		GetGameInstance()->GetSubsystem<UContextManager>()->ToggleInventory();
	}
}

void ARePlayerController::GamepadFocusUp()
{
	// GetGameInstance()->GetSubsystem<UFocusManager>()->GetOnFocusChangedDelegate().Broadcast(EFocusNavigationType::Up);
}

void ARePlayerController::GamepadFocusDown()
{
	// GetGameInstance()->GetSubsystem<UFocusManager>()->GetOnFocusChangedDelegate().Broadcast(EFocusNavigationType::Down);
}

void ARePlayerController::GamepadFocusLeft()
{
	// GetGameInstance()->GetSubsystem<UFocusManager>()->GetOnFocusChangedDelegate().Broadcast(EFocusNavigationType::Left);
}

void ARePlayerController::GamepadFocusRight()
{
	// GetGameInstance()->GetSubsystem<UFocusManager>()->GetOnFocusChangedDelegate().Broadcast(EFocusNavigationType::Right);
}

void ARePlayerController::Interact()
{
	// GetGameInstance()->GetSubsystem<UInteractionManager>()->Interact();
}

void ARePlayerController::Inspect()
{
	// GetGameInstance()->GetSubsystem<UInteractionManager>()->Inspect();
}

void ARePlayerController::QuickSavePressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_QuickSave);
}

void ARePlayerController::QuickSaveReleased()
{
	if (GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_QuickSave))
	{
		GetGameInstance()->GetSubsystem<USaveManager>()->SaveGame();
	}
}

void ARePlayerController::QuickLoadPressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_QuickLoad);
}

void ARePlayerController::QuickLoadReleased()
{
	if (GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_QuickLoad))
	{
		GetGameInstance()->GetSubsystem<USaveManager>()->StartGameLoad();
	}
}

void ARePlayerController::PreInit()
{
	// Manager Setup

	GetGameInstance()->GetSubsystem<UInputManager>()->Setup(this);

	// This

	bShowMouseCursor = true;
	CurrentMouseCursor = DefaultMouseCursor = EMouseCursor::Custom;
	bEnableStreamingSource = false;
}

void ARePlayerController::Init()
{
	// Delegate Bindings

	UContextManager* ContextManager = GetGameInstance()->GetSubsystem<UContextManager>();
	USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>();
	UInputManager* InputManager = GetGameInstance()->GetSubsystem<UInputManager>();
	
	ContextManager->OnContextChangedDelegate.AddUObject(this, &ARePlayerController::OnContextChangedListener);
	SaveManager->OnGameLoadStartedDelegate.AddUObject(this, &ARePlayerController::OnGameLoadStartedListener);
	InputManager->OnInputEnabledDelegate.AddUObject(this, &ARePlayerController::OnInputEnabledListener);
	InputManager->OnInputDisabledDelegate.AddUObject(this, &ARePlayerController::OnInputDisabledListener);

	// Input
	
	GetGameInstance()->GetSubsystem<UPointerManager>()->ShowPointer();
	InputManager->EnableInput();
	GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(IMC_Universal, 0);
	
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	// Disable Default Focus Navigation

	FSlateApplication& SlateApplication = FSlateApplication::Get();
	const TSharedRef<FNavigationConfig> Config = SlateApplication.GetNavigationConfig();
	Config->bTabNavigation = false;
	Config->bKeyNavigation = false;
	Config->bAnalogNavigation = false;
	SlateApplication.SetNavigationConfig(Config);

	// Other
	
	SetActorTickEnabled(true);
	ContextManager->SetContext(EContext::Overworld);
}

void ARePlayerController::SetActiveInputMappingContext(const UInputMappingContext* InInputMappingContext)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	if (ActiveInputMappingContext)
	{
		Subsystem->RemoveMappingContext(ActiveInputMappingContext);
	}
	
	ActiveInputMappingContext = InInputMappingContext;
	
	if (ActiveInputMappingContext)
	{
		Subsystem->AddMappingContext(InInputMappingContext, 1);
	}
}