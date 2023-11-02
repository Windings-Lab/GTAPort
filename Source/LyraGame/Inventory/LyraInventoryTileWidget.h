// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "LyraInventoryTileWidget.generated.h"

class UImage;
class UCommonTextBlock;
class USizeBox;
class UButton;
class UBorder;
class ULyraInventoryItemInstance;
/**
 * 
 */
UCLASS()
class LYRAGAME_API ULyraInventoryTileWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateWidget(ULyraInventoryItemInstance* NewItem);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_UpdateWidget();

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<ULyraInventoryItemInstance> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UBorder> DynamicBorder;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UButton> SlotButton;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<USizeBox> StackBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UCommonTextBlock> StackCount;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UImage> TileImage;
};
