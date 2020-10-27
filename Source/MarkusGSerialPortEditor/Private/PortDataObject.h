// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SerialPort_UE.h"
#include "PortDataObject.generated.h"

/**
 * 
 */
UCLASS()
class UPortDataObject : public UObject
{
	GENERATED_BODY()
public:
	FString GetData() const {
		return DataRecv;
	}

	bool bToString = false;

	void BindToListenThreadDelegate(FListenThreadSignature& inDelegate) {
		ListenThreadDelegate = &inDelegate;
		ListenThreadDelegate->AddDynamic(this, &UPortDataObject::RefreshRecvText);
	}

	virtual void BeginDestroy() override {
		if(ListenThreadDelegate && ListenThreadDelegate->Contains(this, FName("RefreshRecvText"))){
			ListenThreadDelegate->RemoveDynamic(this, &UPortDataObject::RefreshRecvText);
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
		if(!bToString)
			DataRecv += stringData;
		else 
			DataRecv += HexToStr(stringData);
		}

	class FListenThreadSignature* ListenThreadDelegate = nullptr;
};

