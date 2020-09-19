// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SerialPortFactoryNew.generated.h"

/**
 * 
 */
UCLASS()
class USerialPortFactoryNew : public UFactory
{
	GENERATED_BODY()
	
public:
	USerialPortFactoryNew();

	virtual uint32 GetMenuCategories() const override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
