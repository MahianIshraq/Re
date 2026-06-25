#include "Widget/ReWidget.h"
#include "Engine/GameInstance.h"
#include "Game/Manager/PointerManager.h"

void UReWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetCursor(EMouseCursor::Custom);
}

void UReWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshPointer();
}

void UReWidget::SetPointer(EPointer InPointer)
{
	Pointer = InPointer;
	RefreshPointer();
}

void UReWidget::RefreshPointer()
{
	GetGameInstance()->GetSubsystem<UPointerManager>()->SetPointer(Pointer);
}