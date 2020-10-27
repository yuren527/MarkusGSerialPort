// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialPort_UE.h"
#include "SerialPortCore.h"

USerialPort_UE::USerialPort_UE()
{
	SerialPort_internal = nullptr;
}

void USerialPort_UE::ExecuteListenThreadDelegate(void* obj, string data) {
	USerialPort_UE* port_ue = reinterpret_cast<USerialPort_UE*>(obj);
	port_ue->ListenThreadDelegate.Broadcast(UTF8_TO_TCHAR(data.c_str()));
}

void USerialPort_UE::BeginDestroy()
{
	Super::BeginDestroy();
	Finalize();
}

bool USerialPort_UE::InitPort()
{
	return InitPortWithParam(PortNo, Baud, Parity, Databits, Stopbits, DwCommEvents);
}

bool USerialPort_UE::InitPortWithParam(uint8 _portNo, int _baud, bool _parity, uint8 _databits, uint8 _stopsbits, ECommEvents_UE _dwCommEvents)
{
	if (SerialPort_internal) {
		return false;
	}

	SerialPort_internal = new SerialPort;
	bool bSuccess = SerialPort_internal->InitPort(_portNo, _baud, _parity ? 'Y' : 'N', _databits, _stopsbits, _dwCommEvents);
	if (bSuccess) {
		UE_LOG(LogTemp, Log, TEXT("Serial port initialized successfully"));
	}
	return bSuccess;
}

bool USerialPort_UE::WriteDataString(FString data)
{
	if (!SerialPort_internal) {
		return false;
	}
	
	string dataString = TCHAR_TO_UTF8(*data);
	const char* char_array = dataString.c_str();
	TArray<uint8> hexArray;
	for (int i = 0; i < dataString.length(); i++) {
		hexArray.Add(char_array[i]);
	}
	return WriteData(hexArray);
}

const string hexString = "0123456789ABCDEF";

bool USerialPort_UE::WriteDataHex(FString data)
{
	if (!SerialPort_internal) {
		return false;
	}
	
	string dataString = TCHAR_TO_UTF8(*data);
	if (dataString.length() % 2 != 0) {
		UE_LOG(LogTemp,Warning, TEXT("Wrong hex data!"));
		return false;
	}

	TArray<uint8> hexArray;
	for (int i = 0; i < dataString.length(); i += 2) {
		size_t a = hexString.find(toupper(dataString[i]), 0);
		size_t b = hexString.find(toupper(dataString[i + 1]), 0);
		if (a == string::npos || b == string::npos) {
			UE_LOG(LogTemp,Warning, TEXT("Wrong hex data!"));
			return false;
		}

		hexArray.Add(a * 16 + b);
	}
	return WriteData(hexArray);
}

bool USerialPort_UE::WriteData(TArray<uint8> data) {
	if (!SerialPort_internal) {
		return false;
	}

	return SerialPort_internal->WriteData(&data[0], data.Num());
}

bool USerialPort_UE::OpenListenThread(int sleepTimeInterval)
{
	if (!SerialPort_internal) {
		return false;
	}

	bool bSuccess = SerialPort_internal->OpenListenThread(sleepTimeInterval);
	if (bSuccess) {
		SerialPort_internal->BindToListenThread(this, &USerialPort_UE::ExecuteListenThreadDelegate);
		UE_LOG(LogTemp, Log, TEXT("Serial port listen thread opened"));
	}
	return bSuccess;
}

void USerialPort_UE::CloseListenThread()
{
	if (!SerialPort_internal) {
		return;
	}

	SerialPort_internal->CloseListenThread();
	UE_LOG(LogTemp, Log, TEXT("Serial port listen thread closed"));
}

void USerialPort_UE::ClearListenThreadDelegate()
{
	if (!SerialPort_internal) {
		return;
	}

	SerialPort_internal->ClearListenThreadDelegate();
}

void USerialPort_UE::Finalize()
{
	if (SerialPort_internal) {
		delete SerialPort_internal;
		SerialPort_internal = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Serial port finalized"));
	}
}

bool USerialPort_UE::IsInitialized() const
{
	return SerialPort_internal && SerialPort_internal->IsPortOpened();
}

bool USerialPort_UE::IsListenThreadOpened() const
{
	return SerialPort_internal && SerialPort_internal->IsListenThreadOpened();
}

FString USerialPort_UE::HexToString(const FString& hex) const
{
	return HexToStr(hex);
}

FString HexToStr(const FString& hex)
{
	string dataString = TCHAR_TO_UTF8(*hex);
	
	int index = 0;
    if( !dataString.empty())
    {
        while( (index = dataString.find(' ',index)) != string::npos)
        {
            dataString.erase(index,1);
        }
    }

	if (dataString.length() % 2 != 0) {
		UE_LOG(LogTemp,Warning, TEXT("Wrong hex data!"));
		return FString();
	}

	TArray<uint8> hexArray;
	for (int i = 0; i < dataString.length(); i += 2) {
		size_t a = hexString.find(toupper(dataString[i]), 0);
		size_t b = hexString.find(toupper(dataString[i + 1]), 0);
		if (a == string::npos || b == string::npos) {
			UE_LOG(LogTemp,Warning, TEXT("Wrong hex data!"));
			return FString();
		}

		hexArray.Add(a * 16 + b);
	}
	
	std::stringstream s;
	for(auto i : hexArray){
		s << std::hex << i;
	}

	return FString(s.str().c_str());
}
