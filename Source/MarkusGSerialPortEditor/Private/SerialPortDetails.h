// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * 
 */
class FSerialPortDetails : public IDetailCustomization
{
public:
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance();

    /** IDetailCustomization interface */
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    virtual ~FSerialPortDetails() override;

protected:
    FReply OnInitButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnFinalizeButtonClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnOpenListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnCloseListenThreadClicked(TWeakObjectPtr<class USerialPort_UE> obj);

    FReply OnSendDataClicked(TWeakObjectPtr<class USerialPort_UE> obj);
    FReply OnClearSendDataClicked();
    FReply OnClearRecvDataClicked();

    void OnSendHexChanged(ECheckBoxState newState);
    void OnRecvHexChanged(ECheckBoxState newState);

    TSharedPtr<class SMultiLineEditableTextBox> TextToSend;

    TWeakObjectPtr<class UPortDataObject> ReceivedDataObj;

    bool bSendHex;
};


