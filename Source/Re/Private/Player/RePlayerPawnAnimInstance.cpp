#include "Player/RePlayerPawnAnimInstance.h"

#include "Component/Player/RePlayerPawnMoveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

void URePlayerPawnAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (!GetWorld()->IsGameWorld())
	{
		return;
	}

	RePlayerPawnMoveComponent = TryGetPawnOwner()->FindComponentByClass<URePlayerPawnMoveComponent>();
}

void URePlayerPawnAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!GetWorld()->IsGameWorld())
	{
		return;
	}

	EffectiveSpeed = RePlayerPawnMoveComponent->GetEffectiveSpeed();
}
