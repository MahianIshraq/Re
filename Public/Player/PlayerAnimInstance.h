#pragma once

class UPlayerMovementComponent;

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS() class RE_API UPlayerAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

private:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaTime) override;

private:

	TWeakObjectPtr<const UPlayerMovementComponent> PlayerMovementComponentCache;

	UPROPERTY(Category = "Caches | Tick", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient)
	float EffectiveSpeedCache = 0.0f;

	UPROPERTY(Category = "Caches | Tick", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient)
	bool bIsGroundedCache = true;
};