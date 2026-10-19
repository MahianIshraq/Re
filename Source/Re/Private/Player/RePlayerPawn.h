#pragma once

// TODO: Use UAF?

#include "Delegates/IDelegateInstance.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "RePlayerPawn.generated.h"

class UCapsuleComponent;
class UMaterialParameterCollectionInstance;
class URePlayerPawnMoveComponent;
class UReSave;
class UReSaveManager;
class USkeletalMeshComponent;
class UStaticMeshComponent;

UCLASS
(
	HideCategories = (Pawn, Camera, Rendering, HLOD, Replication, Collision, Physics, Networking, Input, Actor, WorldPartition, LevelInstance, Cooking, DataLayers),
	NotBlueprintable, NotBlueprintType
)
class ARePlayerPawn final : public APawn
{
	GENERATED_BODY()

public:

					ARePlayerPawn				();

private:

	virtual void	PostInitializeComponents	() override;
	virtual void	BeginPlay					() override;
	virtual void	Tick						(float DeltaTime) override;
	virtual void	EndPlay						(EEndPlayReason::Type Reason) override;

	void			OnGameSaveStarted			(UReSave* Save);
	void			OnGameLoaded				(const UReSave* Save);

private:

	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UCapsuleComponent>			CapsuleComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<USkeletalMeshComponent>		SkeletalMeshComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UStaticMeshComponent>			MovementPlaneComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<URePlayerPawnMoveComponent>	PlayerPawnMoveComponent;

	static constexpr float		HalfHeight							= 85.0f;
	static constexpr float		Radius								= 30.0f;
	inline static const FName	MPC_Player_Name						= "MPC_Player";
	inline static const FName	MPC_Player_LocationParameterName	= "Location";

	UPROPERTY(Category = "Cache", VisibleAnywhere, Transient) TWeakObjectPtr<UReSaveManager>						SaveManager;
	UPROPERTY(Category = "Cache", VisibleAnywhere, Transient) TWeakObjectPtr<UMaterialParameterCollectionInstance>	MPC_Player;

	FDelegateHandle OnGameSaveStartedHandle;
	FDelegateHandle OnGameLoadedHandle;
};
