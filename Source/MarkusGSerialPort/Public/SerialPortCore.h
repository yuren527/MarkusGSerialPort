// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <process.h>    
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <Windows.h>  

using namespace std;

class MARKUSGSERIALPORT_API SerialPort {

private:
	/** 串口句柄 */
	HANDLE  hComm;
	/** 线程退出标志变量 */
	bool bExit = false;
	/** 线程句柄 */
	volatile HANDLE hListenThread;
	/** 同步互斥,临界区保护 */
	CRITICAL_SECTION csCommunicationSync;       //!< 互斥操作串口  

	UINT SleepTimeInterval = 0;

	void* DelegateObject = nullptr;
	void(*DelegateFunc)(void*, string) = nullptr;

private:
	UINT GetBytesInCOM();
	bool ReadChar(unsigned char& cRecved);
	bool OpenPort(UINT portNo);
	void ClosePort();
	static UINT ListenThread(void* pParam);

public:
	SerialPort();
	bool InitPort(UINT portNo, UINT baud = CBR_19200, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);
	~SerialPort();

	SerialPort& operator= (SerialPort&) = delete;
	SerialPort(SerialPort&) = delete;
	SerialPort(SerialPort&&) = delete;

	bool WriteData(unsigned char* pData, int length);

	bool OpenListenThread(UINT sleepTimeInterval);

	bool CloseListenThread();

	void BindToListenThread(void*, void(*func)(void*, string));

	void ClearListenThreadDelegate();

	bool IsPortOpened() const;
	bool IsListenThreadOpened() const;
};