#include "Widget/Border/ReBorder.h"
#include "UObject/ConstructorHelpers.h"
#include "Struct/Asset/FTexturePaths.h"
#include "Engine/Texture2D.h"

UReBorder::UReBorder()
{
	T_Border_Outer = ConstructorHelpers::FObjectFinder<UTexture2D>(FTexturePaths::T_Border_Outer).Object;
	T_Border_Inner = ConstructorHelpers::FObjectFinder<UTexture2D>(FTexturePaths::T_Border_Inner).Object;

	SetPadding(FMargin());
	UseOuterBorderBrush();
}

void UReBorder::UseOuterBorderBrush()
{
	FSlateBrush Brush;
	Brush.SetResourceObject(T_Border_Outer);
	Brush.DrawAs = ESlateBrushDrawType::Box;
	Brush.Margin = FMargin(2.0f / 32.0f);
	SetBrush(Brush);
}

void UReBorder::UseInnerBorderBrush()
{
	FSlateBrush Brush;
	Brush.SetResourceObject(T_Border_Inner);
	Brush.DrawAs = ESlateBrushDrawType::Box;
	Brush.Margin = FMargin(2.0f / 32.0f);
	SetBrush(Brush);
}