#pragma once

class UReSaveGame;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UPlayerMovementComponent;
class UWorldPartitionStreamingSourceComponent;
class APlayerCamera;
class UMaterialParameterCollection;

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS
(
	HideCategories =
	(
		Pawn,
		Camera,
		Rendering,
		HLOD,
		Replication,
		Collision,
		Physics,
		Networking,
		Input,
		Actor,
		WorldPartition,
		LevelInstance,
		Cooking,
		DataLayers
	)
)
class RE_API APlayerPawn final : public APawn
{
	GENERATED_BODY()

public:

	APlayerPawn();

private:

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float InDeltaTime) override;
	
	// Delegate Listeners

	void OnGameSavedListener(UReSaveGame* InSaveGame);
	void OnGameLoadedListener(const UReSaveGame* InSaveGame);
	void OnInputEnabledListener();
	void OnInputDisabledListener();

private:

	// Components

	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UStaticMeshComponent> MovementPlaneComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UPlayerMovementComponent> PlayerMovementComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UWorldPartitionStreamingSourceComponent> WorldPartitionStreamingSourceComponent;

	// Owned Actors

	UPROPERTY(Transient) TObjectPtr<APlayerCamera> PlayerCamera;

	// Properties
	
	inline static constexpr float HalfHeight = 85.0f;
	inline static constexpr float Radius = 30.0f;
	
	// Assets | MPC
	
	UPROPERTY(Transient) TObjectPtr<const UMaterialParameterCollection> MPC_Player;
};