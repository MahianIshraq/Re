#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/ObjectMacros.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "ReAtmosphereManager.generated.h"

class AReAtmosphere;

UCLASS(DisplayName = "Atmosphere Manager")
class UReAtmosphereManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	[[nodiscard]] AReAtmosphere*	GetAtmosphere			() const;
	void							RegisterAtmosphere		(AReAtmosphere* NewAtmosphere);
	void							UnregisterAtmosphere	();

private:

	UPROPERTY(Category = "Data", VisibleAnywhere) TWeakObjectPtr<AReAtmosphere> Atmosphere;
};
