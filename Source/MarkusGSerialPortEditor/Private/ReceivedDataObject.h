// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SerialPort_UE.h"
#include "ReceivedDataObject.generated.h"

/**
 * 
 */
UCLASS()
class UReceivedDataObject : public UObject
{
	GENERATED_BODY()
public:
	FString GetData() const {
		return DataRecv;
	}

	void BindToListenThreadDelegate(FListenThreadSignature& inDelegate) {
		ListenThreadDelegate = &inDelegate;
		ListenThreadDelegate->AddDynamic(this, &UReceivedDataObject::RefreshRecvText);
	}

	virtual void BeginDestroy() override {
		if(ListenThreadDelegate && ListenThreadDelegate->Contains(this, FName("RefreshRecvText"))){
			ListenThreadDelegate->RemoveDynamic(this, &UReceivedDataObject::RefreshRecvText);
		}
		Super::BeginDestroy();
	}

	virtual void ClearText() {
		DataRecv = FString();
	}

private:
	FString DataRecv;

	UFUNCTION()
		void RefreshRecvText(const FString& stringData) {
		DataRecv += stringData;
	}

	class FListenThreadSignature* ListenThreadDelegate = nullptr;
};

