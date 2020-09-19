// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SerialPortActions.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SerialPort_UE.h"
#include "Styling/SlateStyle.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FSerialPortActions::FSerialPortActions(const TSharedRef<ISlateStyle>& InStyle)
	: Style(InStyle)
{ }

bool FSerialPortActions::CanFilter()
{
	return true;
}

uint32 FSerialPortActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}


FText FSerialPortActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SerialPort", "Serial Port");
}


UClass* FSerialPortActions::GetSupportedClass() const
{
	return USerialPort_UE::StaticClass();
}


FColor FSerialPortActions::GetTypeColor() const
{
	return FColor::Red;
}

#undef LOCTEXT_NAMESPACE
