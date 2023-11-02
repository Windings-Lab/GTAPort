// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LyraInventoryTileWidget.h"

#include "CommonTextBlock.h"
#include "InventoryFragment_QuickBarIcon.h"
#include "LyraInventoryItemInstance.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetTextLibrary.h"

void ULyraInventoryTileWidget::UpdateWidget(ULyraInventoryItemInstance* NewItem)
{
	Item = NewItem;
	Item->SetWidget(this);

	if(Item && !Item->IsEmpty() && Item->GetItemCount() > 0)
	{
		auto* QuickBarFragment = Item->FindFragmentByClass<UInventoryFragment_QuickBarIcon>();
		if(QuickBarFragment)
		{
			TileImage->SetBrush(QuickBarFragment->Brush);
		}

		if(Item->IsStackable())
		{
			StackBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			StackCount->SetText(UKismetTextLibrary::Conv_IntToText(Item->GetItemCount()));
		}
		else
		{
			StackBox->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		FSlateBrush Brush;
		Brush.ImageSize = FVector2d(128, 128);
		Brush.Margin = FMargin(0., 0., 0., 0.);
		Brush.TintColor = FSlateColor(FLinearColor::White);
		TileImage->SetBrush(Brush);
		StackBox->SetVisibility(ESlateVisibility::Hidden);
	}

	K2_UpdateWidget();
}

void ULyraInventoryTileWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	if(ULyraInventoryItemInstance* NewItem = Cast<ULyraInventoryItemInstance>(ListItemObject))
	{
		UpdateWidget(NewItem);
	}
}
