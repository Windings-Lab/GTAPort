// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GTATileView.h"

UUserWidget* UGTATileView::GetWidgetFromItem(const UObject* Item)
{
	return GetEntryWidgetFromItem(Item);
}
