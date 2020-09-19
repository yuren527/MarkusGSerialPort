// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "Templates/SharedPointer.h"

class ISlateStyle;


/**
 * Implements an action for UTextAsset assets.
 */
class FSerialPortActions
	: public FAssetTypeActions_Base
{
public:

	FSerialPortActions(const TSharedRef<ISlateStyle>& InStyle);

public:

	virtual bool CanFilter() override;
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	
private:

	/** Pointer to the style set to use for toolkits. */
	TSharedRef<ISlateStyle> Style;
};
