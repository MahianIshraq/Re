#pragma once

// TODO: Use UAF?

#include "Animation/AnimInstance.h"
#include "UObject/ObjectMacros.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "RePlayerPawnAnimInstance.generated.h"

class URePlayerPawnMoveComponent;

UCLASS() class URePlayerPawnAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

private:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:

	TWeakObjectPtr<const URePlayerPawnMoveComponent> RePlayerPawnMoveComponent;
	UPROPERTY(Category = "Cache", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient) float EffectiveSpeed = 0.0f;
};
