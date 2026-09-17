#include "Actor/World/ReAtmosphere.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameInstance.h"
#include "Game/ReGameInstance.h"
#include "Game/ReSave.h"
#include "Game/Manager/ReAtmosphereManager.h"
#include "Game/Manager/ReSaveManager.h"
#include "Math/ReMath.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "Misc/DateTime.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "Shared/RePIEFolders.h"
#include "System/ReDebug.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"

AReAtmosphere::AReAtmosphere()
{
	PrimaryActorTick.bCanEverTick			= true;
	PrimaryActorTick.bStartWithTickEnabled	= false;

	SceneComponent			= CreateDefaultSubobject<USceneComponent>				("SceneComponent");
	SunLightComponent		= CreateDefaultSubobject<UDirectionalLightComponent>	("SunLightComponent");
	MoonLightComponent		= CreateDefaultSubobject<UDirectionalLightComponent>	("MoonLightComponent");
	SkyLightComponent		= CreateDefaultSubobject<USkyLightComponent>			("SkyLightComponent");
	SkyAtmosphereComponent	= CreateDefaultSubobject<USkyAtmosphereComponent>		("SkyAtmosphereComponent");
	RootComponent			= SceneComponent;

	SunLightComponent		->SetupAttachment(RootComponent);
	MoonLightComponent		->SetupAttachment(RootComponent);
	SkyLightComponent		->SetupAttachment(RootComponent);
	SkyAtmosphereComponent	->SetupAttachment(RootComponent);

	SceneComponent			->SetMobility(EComponentMobility::Static);
	SunLightComponent		->SetMobility(EComponentMobility::Movable);
	MoonLightComponent		->SetMobility(EComponentMobility::Movable);
	SkyLightComponent		->SetMobility(EComponentMobility::Movable);
	SkyAtmosphereComponent	->SetMobility(EComponentMobility::Static);

	SunLightComponent		->SetIntensity(SunLightIntensity);
	MoonLightComponent		->SetIntensity(MoonLightIntensity);
	SkyLightComponent		->SetIntensity(SkyLightIntensity);

	SunLightComponent		->SetLightSourceAngle(0.0f);
	MoonLightComponent		->SetLightSourceAngle(0.0f);
}

FReTime AReAtmosphere::GetTime() const
{
	return Time;
}

void AReAtmosphere::SetManualTime(FReTime NewTime)
{
	bUseDeviceTime	= false;
	Time			= NewTime;

	Synchronize();
	UpdateTickInterval();
}

void AReAtmosphere::UseDeviceTime(bool bFlag)
{
	bUseDeviceTime		= bFlag;
	bFreezeManualTime	= bFlag;
	Time				= FDateTime::Now();

	Synchronize();
	UpdateTickInterval();
}

void AReAtmosphere::FreezeManualTime(bool bFlag)
{
	if (!bUseDeviceTime)
	{
		bFreezeManualTime = bFlag;
	}
}

void AReAtmosphere::BeginPlay()
{
	Super::BeginPlay();

	UReGameInstance* ReGameInstance		= GetGameInstance<UReGameInstance>();

	AtmosphereManager					= ReGameInstance	->GetSubsystem<UReAtmosphereManager>();
	SaveManager							= ReGameInstance	->GetSubsystem<UReSaveManager>();

	OnGameStartedHandle					= ReGameInstance	->AddOnGameStarted		(this, &AReAtmosphere::OnGameStarted);
	OnGameSaveStartedHandle				= SaveManager		->AddOnGameSaveStarted	(this, &AReAtmosphere::OnGameSaveStarted);
	OnGameLoadedHandle					= SaveManager		->AddOnGameLoaded		(this, &AReAtmosphere::OnGameLoaded);

	AtmosphereManager->RegisterAtmosphere(this);

	Time								= bUseDeviceTime ? FDateTime::Now() : DefaultTime;

	UpdateSeason();
	Synchronize();

#if !UE_BUILD_SHIPPING
	RegisterConsoleCommands();
#endif

#if WITH_EDITOR
	SetFolderPath(Re::PIEFolders::World);
#endif
}

void AReAtmosphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TRACE_CPUPROFILER_EVENT_SCOPE(ATMOSPHERE_TICK);

	if (bUseDeviceTime)
	{
		Time = FDateTime::Now();
		Synchronize();
		return;
	}

	if (!bFreezeManualTime)
	{
		Time.AdvanceByDeltaTime(TimeSpeed, DeltaTime);
		Synchronize();
	}
}

void AReAtmosphere::EndPlay(EEndPlayReason::Type Reason)
{
	AtmosphereManager					->UnregisterAtmosphere();

#if !UE_BUILD_SHIPPING
	UnregisterConsoleCommands();
#endif

	GetGameInstance<UReGameInstance>()	->RemoveOnGameStarted(OnGameStartedHandle);
	SaveManager							->RemoveOnGameSaveStarted(OnGameSaveStartedHandle);
	SaveManager							->RemoveOnGameLoaded(OnGameLoadedHandle);

	OnGameStartedHandle					.Reset();
	OnGameSaveStartedHandle				.Reset();
	OnGameLoadedHandle					.Reset();

	Super::EndPlay(Reason);
}

void AReAtmosphere::UpdateTickInterval()
{
	check(TimeSpeed > 0.0f);

	CurrentTickInterval = bUseDeviceTime ? GameSecondsTickInterval : GameSecondsTickInterval / TimeSpeed;

	SetActorTickEnabled(false);
	SetActorTickInterval(CurrentTickInterval);
	SetActorTickEnabled(true);
}

void AReAtmosphere::UpdateSeason()
{
	Season			= Re::SeasonUtilities::GetCurrentSeason();
	SunriseHour		= Re::SeasonUtilities::GetSunriseHour(Season);
	SunsetHour		= Re::SeasonUtilities::GetSunsetHour(Season);

	check(SunsetHour > SunriseHour);
}

void AReAtmosphere::Synchronize()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(ATMOSPHERE_SYNC);

	const int32 NewSynchronizedHour = Time.Hour;

	if (CurrentSynchronizedHour == NewSynchronizedHour)
	{
		return;
	}

	CurrentSynchronizedHour		= NewSynchronizedHour;
	bIsDaytime					= Re::SeasonUtilities::IsDaytime(CurrentSynchronizedHour, Season);

	if (bIsDaytime)
	{
		const float DayProgress		= Re::Math::GetSaturated	((CurrentSynchronizedHour - SunriseHour) / static_cast<float>(SunsetHour - SunriseHour));
		const float SunPitch		= FMath::Lerp				(-180.0f, 0.0f, DayProgress);

		TRACE_CPUPROFILER_EVENT_SCOPE(ATMOSPHERE_SYNC_ROTATION);
		SunLightComponent->SetRelativeRotation					(FRotator(SunPitch, 45.0f, 0.0f));
	}

	else
	{
		const float NightProgress	= Re::Math::GetSaturated	(Re::Math::GetWrapped(24 + CurrentSynchronizedHour - SunsetHour, 0, 24) / static_cast<float>(24 + SunriseHour - SunsetHour));
		const float MoonPitch		= FMath::Lerp				(0.0f, 180.0f, NightProgress) + 180.0f;

		TRACE_CPUPROFILER_EVENT_SCOPE(ATMOSPHERE_SYNC_ROTATION);
		MoonLightComponent->SetRelativeRotation					(FRotator(MoonPitch, 45.0f, 0.0f));
	}

	TRACE_CPUPROFILER_EVENT_SCOPE(ATMOSPHERE_SYNC_VISIBILITY);

	SunLightComponent	->SetVisibility(bIsDaytime);
	MoonLightComponent	->SetVisibility(!bIsDaytime);
	SkyLightComponent	->RecaptureSky();
}

void AReAtmosphere::OnGameStarted(APlayerController*)
{
	UpdateTickInterval();
	SetActorTickEnabled(true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AReAtmosphere::OnGameSaveStarted(UReSave* Save)
{
	Save->bUseDeviceTime		= bUseDeviceTime;
	Save->bFreezeManualTime		= bFreezeManualTime;
	Save->Time					= Time;
}

void AReAtmosphere::OnGameLoaded(const UReSave* Save)
{
	bUseDeviceTime		= Save->bUseDeviceTime;
	bFreezeManualTime	= Save->bFreezeManualTime;
	Time				= bUseDeviceTime ? FDateTime::Now() : Save->Time;

	Synchronize();
}

#if !UE_BUILD_SHIPPING

void AReAtmosphere::RegisterConsoleCommands()
{
	IConsoleManager& ConsoleManager = IConsoleManager::Get();

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			USE_DEVICE_TIME, TEXT(""),

			FConsoleCommandWithArgsDelegate::CreateWeakLambda
			(
				this, [this](const TArray<FString>& Args)
				{
					UseDeviceTime(Re::Debug::Helpers::GetFlag(Args));
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			FREEZE_TIME, TEXT(""),

			FConsoleCommandWithArgsDelegate::CreateWeakLambda
			(
				this, [this](const TArray<FString>& Args)
				{
					FreezeManualTime(Re::Debug::Helpers::GetFlag(Args));
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			SET_TIME, TEXT(""),

			FConsoleCommandWithArgsDelegate::CreateWeakLambda
			(
				this, [this](const TArray<FString>& Args)
				{
					SetManualTime
					(
						FReTime::GetTimeFromHour(Re::Debug::Helpers::GetNonNegativeInt(Args))
					);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			RESET_TIME, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime(DefaultTime);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			RANDOMIZE_TIME, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime
					(
						FReTime(FMath::RandRange(0, 23))
					);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			INCREMENT_TIME, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime(++Time);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			DECREMENT_TIME, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime(--Time);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			TIME_DAY, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime
					(
						FReTime(12, 0, 0)
					);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			TIME_NIGHT, TEXT(""),

			FConsoleCommandDelegate::CreateWeakLambda
			(
				this, [this]()
				{
					SetManualTime
					(
						FReTime(0, 0, 0)
					);
				}
			),

			ECVF_Cheat
		)
	);

	ConsoleCommands.Add
	(
		ConsoleManager.RegisterConsoleCommand
		(
			SET_TIME_SPEED, TEXT(""),

			FConsoleCommandWithArgsDelegate::CreateWeakLambda
			(
				this, [this](const TArray<FString>& Args)
				{
					const int32 Value = Re::Debug::Helpers::GetNonNegativeInt(Args);

					if (Value == 0)
					{
						TimeSpeed = DefaultTimeSpeed;
					}

					else
					{
						TimeSpeed = Value;
					}

					UpdateTickInterval();
				}
			),

			ECVF_Cheat
		)
	);
}

void AReAtmosphere::UnregisterConsoleCommands()
{
	IConsoleManager& ConsoleManager = IConsoleManager::Get();

	for (IConsoleCommand* Command : ConsoleCommands)
	{
		ConsoleManager.UnregisterConsoleObject(Command);
	}

	ConsoleCommands.Empty();
}

#endif

#if WITH_EDITOR

void AReAtmosphere::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorTransform(FTransform::Identity);

	if (bUseDeviceTime)
	{
		Time = FDateTime::Now();
	}

	UpdateSeason();
	Synchronize();
}

void AReAtmosphere::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	const FName MemberPropertyName = Event.GetMemberPropertyName();

	if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, SunLightIntensity)
	)
	{
		SunLightComponent->SetIntensity(SunLightIntensity);
	}

	else if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, MoonLightIntensity)
	)
	{
		MoonLightComponent->SetIntensity(MoonLightIntensity);
	}

	else if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, SkyLightIntensity)
	)
	{
		SkyLightComponent->SetIntensity(SkyLightIntensity);
		SkyLightComponent->RecaptureSky();
	}

	else if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, TimeSpeed)
	)
	{
		UpdateTickInterval();
	}

	else if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, bUseDeviceTime)
	)
	{
		UseDeviceTime(bUseDeviceTime);
	}

	else if
	(
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AReAtmosphere, Time)
	)
	{
		SetManualTime(Time);
	}

	Super::PostEditChangeProperty(Event);
}

#endif
