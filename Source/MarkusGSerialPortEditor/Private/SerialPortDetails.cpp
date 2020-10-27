// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialPortDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SmultiLineEditableTextBox.h"

#include "SerialPort_UE.h"
#include "PortDataObject.h"

TSharedRef<IDetailCustomization> FSerialPortDetails::MakeInstance()
{
	return MakeShareable(new FSerialPortDetails);
}

void FSerialPortDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() != 1) {
		return;
	}

	TWeakObjectPtr<UObject> obj = Objects[0];
	TWeakObjectPtr<USerialPort_UE> objPtr_Cast = CastChecked<USerialPort_UE>(obj);
	if (!objPtr_Cast.IsValid()) {
		return;
	}

	IDetailCategoryBuilder& portCategory = DetailLayout.EditCategory("Serial Port Actions");
	portCategory.AddCustomRow(FText::FromString("Actions")).WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).AutoWidth()
		[
			SNew(SBox).WidthOverride(150.f)
			[
				SNew(SButton)
				.HAlign(HAlign_Fill)
		.OnClicked<FSerialPortDetails, TWeakObjectPtr<USerialPort_UE>>(this, &FSerialPortDetails::OnInitButtonClicked, objPtr_Cast)
		.IsEnabled_Lambda(
			[objPtr_Cast]() {
				return !objPtr_Cast->IsInitialized();
			}
		)
		[
			SNew(STextBlock).Text(FText::FromString("Initialize Port")).Justification(ETextJustify::Center)
		]
			]
		]
	+ SHorizontalBox::Slot().HAlign(HAlign_Left).AutoWidth()
		[
			SNew(SBox).WidthOverride(150.f)
			[
				SNew(SButton)
				.OnClicked<FSerialPortDetails, TWeakObjectPtr<USerialPort_UE>>(this, &FSerialPortDetails::OnFinalizeButtonClicked, objPtr_Cast)
		.IsEnabled_Lambda(
			[objPtr_Cast]() {
				return objPtr_Cast->IsInitialized();
			}
		)
		[
			SNew(STextBlock).Text(FText::FromString("Finalize Port")).Justification(ETextJustify::Center)
		]
			]
		]
	+ SHorizontalBox::Slot().HAlign(HAlign_Left).AutoWidth()
		[
			SNew(SBox).WidthOverride(150.f)
			[
				SNew(SButton)
				.OnClicked<FSerialPortDetails, TWeakObjectPtr<USerialPort_UE>>(this, &FSerialPortDetails::OnOpenListenThreadClicked, objPtr_Cast)
		.IsEnabled_Lambda(
			[objPtr_Cast]() {
				return objPtr_Cast->IsInitialized() && !objPtr_Cast->IsListenThreadOpened();
			}
		)
		[
			SNew(STextBlock).Text(FText::FromString("Open Listen Thread")).Justification(ETextJustify::Center)
		]
			]
		]
	+ SHorizontalBox::Slot().HAlign(HAlign_Left).AutoWidth()
		[
			SNew(SBox).WidthOverride(150.f)
			[
				SNew(SButton)
				.OnClicked<FSerialPortDetails, TWeakObjectPtr<USerialPort_UE>>(this, &FSerialPortDetails::OnCloseListenThreadClicked, objPtr_Cast)
		.IsEnabled_Lambda(
			[objPtr_Cast]() {
				return objPtr_Cast->IsInitialized() && objPtr_Cast->IsListenThreadOpened();
			}
		)
		[
			SNew(STextBlock).Text(FText::FromString("CLose Listen Thread")).Justification(ETextJustify::Center)
		]
			]
		]
		];

	//Send data category
	IDetailCategoryBuilder& sendDataCategory = DetailLayout.EditCategory("Send Data");
	sendDataCategory.AddCustomRow(FText::FromString("SendData")).WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SCheckBox).OnCheckStateChanged<FSerialPortDetails>(this, &FSerialPortDetails::OnSendHexChanged)
			.ToolTipText(FText::FromString("If check, send hex, otherwise, use ASCII"))
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(STextBlock).Text(FText::FromString("Send Hex"))
		]
		];

	sendDataCategory.AddCustomRow(FText::FromString("SendData")).WholeRowContent()
		[
			SNew(SHorizontalBox)
			//Data text to send
		+ SHorizontalBox::Slot().HAlign(HAlign_Fill)
		[
			SNew(SBox).HeightOverride(100.f)
			[
				SAssignNew(TextToSend, SMultiLineEditableTextBox)
				.BackgroundColor(FColor(230.f, 255.f, 230.f))
			]
		]
	//Send button
	+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth()
		[
			SNew(SBox).WidthOverride(100.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
		.OnClicked<FSerialPortDetails, TWeakObjectPtr<USerialPort_UE>>(this, &FSerialPortDetails::OnSendDataClicked, objPtr_Cast)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Send"))
		.Justification(ETextJustify::Center)
		]
			]
		]
	//Clear button
	+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth()
		[
			SNew(SBox).WidthOverride(100.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
		.OnClicked<FSerialPortDetails>(this, &FSerialPortDetails::OnClearSendDataClicked)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Clear"))
		.Justification(ETextJustify::Center)
		.AutoWrapText(true)
		]
			]
		]
		];

	ReceivedDataObj = NewObject<UPortDataObject>();
	ReceivedDataObj->BindToListenThreadDelegate(objPtr_Cast->ListenThreadDelegate);

	//Receive data category
	IDetailCategoryBuilder& receiveDataCategory = DetailLayout.EditCategory("Receive Data");
	receiveDataCategory.AddCustomRow(FText::FromString("ReceiveData")).WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SCheckBox).OnCheckStateChanged<FSerialPortDetails>(this, &FSerialPortDetails::OnRecvHexChanged)
			.ToolTipText(FText::FromString("Receive hex data by default, if checked, convert hex data to string"))
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(STextBlock).Text(FText::FromString("To String"))
		]
		];

	receiveDataCategory.AddCustomRow(FText::FromString("ReceiveData")).WholeRowContent()
		[
			SNew(SHorizontalBox)
			//Data text received
		+ SHorizontalBox::Slot().HAlign(HAlign_Fill)
		[
			SNew(SBox).HeightOverride(100.f)
			[
				SNew(SMultiLineEditableTextBox)
				.BackgroundColor(FColor(255.f, 230.f, 230.f))
		.AutoWrapText(true)
		.IsReadOnly(true)
		.Text_Lambda(
			[this]() {	
				if(ReceivedDataObj.IsValid())
				{
					return FText::FromString(ReceivedDataObj->GetData());
				}
				else {
					return FText();
				}
			}
		)
			]
		]
	//Clear button recv
	+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth()
		[
			SNew(SBox).WidthOverride(100.f)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
		.OnClicked<FSerialPortDetails>(this, &FSerialPortDetails::OnClearRecvDataClicked)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Clear"))
		.Justification(ETextJustify::Center)
		]
			]
		]
		];
}

FSerialPortDetails::~FSerialPortDetails()
{
	ReceivedDataObj->ConditionalBeginDestroy();
}

FReply FSerialPortDetails::OnInitButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj)
{
	obj->InitPort();
	return FReply::Handled();
}

FReply FSerialPortDetails::OnFinalizeButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj)
{
	obj->Finalize();
	return FReply::Handled();
}

FReply FSerialPortDetails::OnOpenListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj)
{
	obj->OpenListenThread(0);
	return FReply::Handled();
}

FReply FSerialPortDetails::OnCloseListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj)
{
	obj->CloseListenThread();
	return FReply::Handled();
}

FReply FSerialPortDetails::OnSendDataClicked(TWeakObjectPtr<class USerialPort_UE> obj) {
	if (bSendHex) {
		obj->WriteDataHex(TextToSend->GetText().ToString());
	}
	else {
		obj->WriteDataString(TextToSend->GetText().ToString());
	}
	TextToSend->SetText(FText());
	return FReply::Handled();
}

FReply FSerialPortDetails::OnClearSendDataClicked() {
	TextToSend->SetText(FText());
	return FReply::Handled();
}

FReply FSerialPortDetails::OnClearRecvDataClicked() {
	ReceivedDataObj->ClearText();
	return FReply::Handled();
}

void FSerialPortDetails::OnSendHexChanged(ECheckBoxState newState)
{
	bSendHex = (bool)newState;
}

void FSerialPortDetails::OnRecvHexChanged(ECheckBoxState newState)
{
	ReceivedDataObj->bToString = (bool)newState;
}




