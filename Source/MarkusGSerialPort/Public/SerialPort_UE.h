// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <string>
#include "SerialPort_UE.generated.h"

using namespace std;
/**
 * 
 */

UENUM()
enum ECommEvents_UE {
	EV_RXCHAR = 0x0001,  // Any Character received
	EV_RXFLAG = 0x0002, // Received certain character
	EV_TXEMPTY = 0x0004, // Transmitt Queue Empty
	EV_CTS = 0x0008,// CTS changed state
	EV_DSR = 0x0010,// DSR changed state
	EV_RLSD = 0x0020, // RLSD changed state
	EV_BREAK = 0x0040, // BREAK received
	EV_ERR = 0x0080,// Line status error occurred
	EV_RING = 0x0100, // Ring signal detected
	EV_PERR = 0x0200, // Printer error occured
	EV_RX80FULL = 0x0400, // Receive buffer is 80 percent full
	EV_EVENT1 = 0x0800,// Provider specific event 1
	EV_EVENT2 = 0x1000,// Provider specific event 2
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListenThreadSignature, const FString&, data);

FString MARKUSGSERIALPORT_API HexToStr(const FString& hex);

UCLASS(BlueprintType)
class MARKUSGSERIALPORT_API USerialPort_UE : public UObject
{
	GENERATED_BODY()
private:
	class SerialPort* SerialPort_internal;

	static void ExecuteListenThreadDelegate(void* obj, string data);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		uint8 PortNo = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		int Baud = 19200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		bool Parity = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		uint8 Databits = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		uint8 Stopbits = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SerialPort")
		TEnumAsByte<ECommEvents_UE> DwCommEvents = EV_RXCHAR;
	UPROPERTY(BlueprintAssignable, Category = "SerialPort")
		FListenThreadSignature ListenThreadDelegate;

public:
	USerialPort_UE();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool InitPort();
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool InitPortWithParam(uint8 _portNo, int _baud = 19200, bool _parity = false, uint8 _databits = 8, uint8 _stopsbits = 1, ECommEvents_UE _dwCommEvents = EV_RXCHAR);
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool WriteDataString(FString data);
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool WriteDataHex(FString data);
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool WriteData(TArray<uint8> data);
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		bool OpenListenThread(int sleepTimeInterval);
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		void CloseListenThread();
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		void ClearListenThreadDelegate();
	UFUNCTION(BlueprintCallable, Category = "SerialPort")
		void Finalize();

	UFUNCTION(BlueprintCallable, Category = "SerialPort", BlueprintPure)
		bool IsInitialized() const;
	UFUNCTION(BlueprintCallable, Category = "SerialPort", BlueprintPure)
		bool IsListenThreadOpened() const;

	UFUNCTION(BlueprintCallable, Category = "SerialPort", BlueprintPure)
		FString HexToString(const FString& hex) const;
};
