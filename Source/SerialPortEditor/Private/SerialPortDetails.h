// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * 
 */
class SERIALPORTEDITOR_API FSerialPortDetails : public IDetailCustomization
{
public:
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance();

    /** IDetailCustomization interface */
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

protected:
    FReply OnInitButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnFinalizeButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnOpenListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnCloseListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj);

    FReply OnSendDataClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnClearSendDataClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnClearRecvDataClicked(TWeakObjectPtr<class USerialPort_UE> obj);

    TSharedPtr<class SMultiLineEditableTextBox> TextToSend;

    //Object to store the data recieved from serial port
    class UReceivedDataObject* ReceivedDataObj;
};


