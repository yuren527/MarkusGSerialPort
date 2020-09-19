// Copyright Epic Games, Inc. All Rights Reserved.

#include "Containers/Array.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Styling/SlateStyle.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "SerialPortActions.h"
#include "SerialPort_UE.h"
#include "SerialPortDetails.h"

#define LOCTEXT_NAMESPACE "FMarkusGSerialPortEditorModule"


/**
 * Implements the TextAssetEditor module.
 */
class FMarkusGSerialPortEditorModule
	: public IModuleInterface
{

public:

	//~ IModuleInterface interface

	virtual void StartupModule() override
	{
		Style = MakeShareable(new FSlateStyleSet("SerialPortStyle"));	
		RegisterAssetTools();
		RegisterDetailsCustomization();
	}

	virtual void ShutdownModule() override
	{
		UnregisterAssetTools();
		UnregisterDetailsCustomization();
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

protected:

	void RegisterAssetTools()
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		auto Action = MakeShareable(new FSerialPortActions(Style.ToSharedRef()));
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}

	void UnregisterAssetTools()
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");
		if (AssetToolsModule != nullptr)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();
			for (auto Action : RegisteredAssetTypeActions)
			{
				AssetTools.UnregisterAssetTypeActions(Action);
			}
		}
	}

	void RegisterDetailsCustomization() {
		static FName PropertyEditor("PropertyEditor");
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
		PropertyModule.RegisterCustomClassLayout(USerialPort_UE::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FSerialPortDetails::MakeInstance));
	}

	void UnregisterDetailsCustomization() {
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
			PropertyModule.UnregisterCustomClassLayout(USerialPort_UE::StaticClass()->GetFName());
		}
	}
private:

	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

	/** Holds the plug-ins style set. */
	TSharedPtr<ISlateStyle> Style;
};


IMPLEMENT_MODULE(FMarkusGSerialPortEditorModule, MarkusGSerialPortEditor);


#undef LOCTEXT_NAMESPACE

