// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GTATileView.h"

void UGTATileView::SetEntryWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	EntryWidgetClass = WidgetClass;
}

void UGTATileView::SetHorizontalSpacing(float Value)
{
	InitHorizontalEntrySpacing(Value);
}

void UGTATileView::SetVerticalSpacing(float Value)
{
	InitVerticalEntrySpacing(Value);
}
