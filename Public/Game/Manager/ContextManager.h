#pragma once

#include "Enum/Game/EContext.h"

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ContextManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextChanged, EContext);

UCLASS() class RE_API UContextManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Getters

	EContext GetContext() const;

	// Setters
	
	void SetContext(EContext InContext);
	
	// Other
	
	void Escape();
	void Back();
	void ToggleInventory();

private:

	// Helper Functions

	void ApplyContext(EContext InContext);

public:

	// Delegate Instances

	FOnContextChanged OnContextChangedDelegate;

private:

	// Data
	
	EContext Context = EContext::None;
	EContext RootContext = EContext::Overworld;
	TArray<EContext> ContextHistory;
};