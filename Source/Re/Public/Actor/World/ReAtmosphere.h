#pragma once

#include "Containers/Array.h"
#include "Delegates/IDelegateInstance.h"
#include "Engine/EngineTypes.h"
#include "Enum/Atmosphere/EReSeason.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "HAL/Platform.h"
#include "Math/Transform.h"
#include "Struct/Atmosphere/ReTime.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "ReAtmosphere.generated.h"

class	APlayerController;
struct	FPropertyChangedEvent;
class	UDirectionalLightComponent;
class	UReAtmosphereManager;
class	UReSaveManager;
class	UReSave;
class	USceneComponent;
class	USkyAtmosphereComponent;
class	USkyLightComponent;

UCLASS
(
	DisplayName = "Atmosphere",

	HideCategories = 
	(
		Transform, Rendering, HLOD, Replication, Collision, Physics, Networking, Input, Actor, WorldPartition, LevelInstance, Cooking, DataLayers
	),

	NotBlueprintable,
	NotBlueprintType
)
class RE_API AReAtmosphere final : public AActor
{
	GENERATED_BODY()

public:

					AReAtmosphere				();

	FReTime			GetTime						() const;
	void			SetManualTime				(FReTime NewTime);
	void			UseDeviceTime				(bool bFlag = true);
	void			FreezeManualTime			(bool bFlag = true);

private:

	virtual void	BeginPlay					() override;
	virtual void	Tick						(float DeltaTime) override;
	virtual void	EndPlay						(EEndPlayReason::Type Reason) override;

	void			UpdateTickInterval			();
	void			UpdateSeason				();
	void			Synchronize					();

	void 			OnGameStarted				(APlayerController*);
	void 			OnGameSaveStarted			(UReSave* Save);
	void 			OnGameLoaded				(const UReSave* Save);

#if !UE_BUILD_SHIPPING

	void			RegisterConsoleCommands		();
	void			UnregisterConsoleCommands	();

#endif

#if WITH_EDITOR

	virtual void	OnConstruction				(const FTransform& Transform) override;
	virtual void	PostEditChangeProperty		(FPropertyChangedEvent& Event) override;

#endif

private:

	UPROPERTY() TObjectPtr<USceneComponent>				SceneComponent;
	UPROPERTY() TObjectPtr<UDirectionalLightComponent>	SunLightComponent;
	UPROPERTY() TObjectPtr<UDirectionalLightComponent>	MoonLightComponent;
	UPROPERTY() TObjectPtr<USkyLightComponent>			SkyLightComponent;
	UPROPERTY() TObjectPtr<USkyAtmosphereComponent>		SkyAtmosphereComponent;

	static constexpr		float		GameSecondsTickInterval		= 1800.0f;
	static constexpr		int32		DefaultTimeSpeed			= 100;
	static inline const		FReTime		DefaultTime					= FReTime(12, 0, 0);

	UPROPERTY(Category = "Properties	| Lighting",	EditAnywhere, meta = (ToolTip = "Lux",	Delta = "0.25",	ClampMin = "0.0",	ClampMax = "100.0", UIMax = "10.0"	)	) float									SunLightIntensity			= 3.0f;
	UPROPERTY(Category = "Properties	| Lighting",	EditAnywhere, meta = (ToolTip = "Lux",	Delta = "0.05",	ClampMin = "0.0",	ClampMax = "100.0", UIMax = "1.0"	)	) float									MoonLightIntensity			= 0.15f;
	UPROPERTY(Category = "Properties	| Lighting",	EditAnywhere, meta = (ToolTip = "Lux",	Delta = "0.1",	ClampMin = "0.0",	ClampMax = "100.0", UIMax = "2.0"	)	) float									SkyLightIntensity			= 0.5f;
	UPROPERTY(Category = "Properties	| Time",		EditAnywhere, meta = (					Delta = "100",	ClampMin = "1",		ClampMax = "10000"					)	) int32									TimeSpeed					= DefaultTimeSpeed;
	UPROPERTY(Category = "Properties	| Time",		EditAnywhere																										) uint8									bUseDeviceTime		: 1		= false;
	UPROPERTY(Category = "Properties	| Time",		EditAnywhere																										) uint8									bFreezeManualTime	: 1		= false;
	UPROPERTY(Category = "Data			| Time",		VisibleAnywhere,	Transient																						) uint8									bIsDaytime			: 1		= false;
	UPROPERTY(Category = "Data			| Tick",		VisibleAnywhere,	Transient																						) float									CurrentTickInterval			= -1.0f;
	UPROPERTY(Category = "Data			| Time",		EditAnywhere,		Transient																						) FReTime								Time						= DefaultTime;
	UPROPERTY(Category = "Data			| Time",		VisibleAnywhere,	Transient																						) EReSeason								Season						= EReSeason::Spring;
	UPROPERTY(Category = "Data			| Time",		VisibleAnywhere,	Transient																						) int32									SunriseHour					= -1;
	UPROPERTY(Category = "Data			| Time",		VisibleAnywhere,	Transient																						) int32									SunsetHour					= -1;
	UPROPERTY(Category = "Data			| Internal",	VisibleAnywhere,	Transient																						) int32									CurrentSynchronizedHour		= -1;
	UPROPERTY(Category = "Cache			| Manager", 	VisibleAnywhere,	Transient																						) TWeakObjectPtr<UReAtmosphereManager>	AtmosphereManager;
	UPROPERTY(Category = "Cache			| Manager", 	VisibleAnywhere,	Transient																						) TWeakObjectPtr<UReSaveManager>		SaveManager;

	FDelegateHandle OnGameStartedHandle;
	FDelegateHandle OnGameSaveStartedHandle;
	FDelegateHandle OnGameLoadedHandle;

#if !UE_BUILD_SHIPPING

	TArray<IConsoleCommand*> ConsoleCommands;

	static constexpr TCHAR USE_DEVICE_TIME	[] = TEXT("time.sync");
	static constexpr TCHAR FREEZE_TIME		[] = TEXT("time.freeze");
	static constexpr TCHAR SET_TIME			[] = TEXT("time.set");
	static constexpr TCHAR RESET_TIME		[] = TEXT("time.reset");
	static constexpr TCHAR RANDOMIZE_TIME	[] = TEXT("time.randomize");
	static constexpr TCHAR INCREMENT_TIME	[] = TEXT("time++");
	static constexpr TCHAR DECREMENT_TIME	[] = TEXT("time--");
	static constexpr TCHAR TIME_DAY			[] = TEXT("time.day");
	static constexpr TCHAR TIME_NIGHT		[] = TEXT("time.night");
	static constexpr TCHAR SET_TIME_SPEED	[] = TEXT("time.speed");

#endif
};
