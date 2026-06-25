#include "Player/PlayerAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Player/PlayerMovementComponent.h"
#include "Game/ReLog.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (const APawn* OwnerPawn = TryGetPawnOwner())
	{
		PlayerMovementComponentCache = OwnerPawn->FindComponentByClass<UPlayerMovementComponent>();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float InDeltaTime)
{
	Super::NativeUpdateAnimation(InDeltaTime);
	
	if (PlayerMovementComponentCache.IsValid())
	{
		EffectiveSpeedCache = PlayerMovementComponentCache->GetEffectiveSpeed();
		bIsGroundedCache = PlayerMovementComponentCache->GetIsGrounded();
	}
	else
	{
		#if !WITH_EDITOR
		RE_LOG("PlayerAnimInstance | PlayerMovementComponentCache is nullptr.");
		#endif
	}
}