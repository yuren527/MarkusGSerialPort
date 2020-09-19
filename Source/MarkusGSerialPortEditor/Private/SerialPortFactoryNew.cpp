// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialPortFactoryNew.h"
#include "SerialPort_UE.h"
#include "AssetTypeCategories.h"

USerialPortFactoryNew::USerialPortFactoryNew() {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USerialPort_UE::StaticClass();
}

uint32 USerialPortFactoryNew::GetMenuCategories() const
{
	//Let's place this asset in the Blueprints category in the Editor
	return EAssetTypeCategories::Misc;
}

bool USerialPortFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}

UObject* USerialPortFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USerialPort_UE>(InParent, InClass, InName, Flags);
}
