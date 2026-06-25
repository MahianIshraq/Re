#pragma once

class UTexture2D;

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "ReBorder.generated.h"

UCLASS() class RE_API UReBorder : public UBorder
{
	GENERATED_BODY()

public:

	UReBorder();

	// Other
	
	void UseOuterBorderBrush();
	void UseInnerBorderBrush();

private:

	UPROPERTY() TObjectPtr<UTexture2D> T_Border_Outer;
	UPROPERTY() TObjectPtr<UTexture2D> T_Border_Inner;
};