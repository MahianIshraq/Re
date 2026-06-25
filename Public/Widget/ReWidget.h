#pragma once

#include "Enum/Game/EPointer.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReWidget.generated.h"

UCLASS() class RE_API UReWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
	void SetPointer(EPointer InPointer);

private:

	// Helper Functions

	void RefreshPointer();

private:

	EPointer Pointer = EPointer::Default;
};