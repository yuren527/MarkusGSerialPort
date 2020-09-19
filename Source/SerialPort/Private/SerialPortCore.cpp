// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialPortCore.h"

using namespace std;

UINT SerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** ������ */
	COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */
	if (ClearCommError(hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}

	//��ȡ���ڽ��ջ�������һ���ֽڵ�����
bool SerialPort::ReadChar(unsigned char& cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&csCommunicationSync);

	return (BytesRead == 1);
}

bool SerialPort::OpenPort(UINT portNo)
{
	/** �����ٽ�� */
	EnterCriticalSection(&csCommunicationSync);

	/** �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** ��ָ���Ĵ��� */
	hComm = CreateFileA(szPort,  /** �豸��,COM1,COM2�� */
		GENERIC_READ | GENERIC_WRITE, /** ����ģʽ,��ͬʱ��д */
		0,                            /** ����ģʽ,0��ʾ������ */
		NULL,                         /** ��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,                /** �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);

	/** �����ʧ�ܣ��ͷ���Դ������ */
	if (hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */
	LeaveCriticalSection(&csCommunicationSync);

	return true;
}
void SerialPort::ClosePort() {
	/** ����д��ڱ��򿪣��ر��� */
	if (hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hComm);
		hComm = INVALID_HANDLE_VALUE;
	}
}
UINT SerialPort::ListenThread(void* pParam)
{
	SerialPort* port = reinterpret_cast<SerialPort*>(pParam);
	// �߳�ѭ��,��ѯ��ʽ��ȡ��������  
	while (!port->bExit)
	{
		UINT BytesInQue = port->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(port->SleepTimeInterval);
			continue;
		}

		/** ��ȡ���뻺�����е����ݲ������ʾ */
		unsigned char cRecved = 0x00;
		do
		{
			cRecved = 0x00;
			if (port->ReadChar(cRecved) == true)
			{

				std::stringstream  ss;
				int tm = cRecved;
				ss << std::hex << std::setw(2) << std::setfill('0') << tm;
				ss << " ";
				string a = ss.str();
				string b;
				transform(a.begin(), a.end(), back_inserter(b), ::toupper);
				//ִ�а󶨵�ί��
				if (port->DelegateObject && port->DelegateFunc)
					port->DelegateFunc(port->DelegateObject, b);
				continue;
			}
		} while (--BytesInQue);
	}

	return 0;
}
bool SerialPort::OpenListenThread(UINT sleepTimeInterval)
{
	SleepTimeInterval = sleepTimeInterval;
	/** ����߳��Ƿ��Ѿ������� */
	if (hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}
	bExit = false;

	UINT threadId;
	/** �����������ݼ����߳� */
	hListenThread = (HANDLE)_beginthreadex(NULL, 0, SerialPort::ListenThread, this, 0, &threadId);
	if (!hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool SerialPort::CloseListenThread()
{
	SleepTimeInterval = 0;
	if (hListenThread != INVALID_HANDLE_VALUE)
	{
		/** ֪ͨ�߳��˳� */
		bExit = true;

		/** �ȴ��߳��˳� */
		Sleep(10);

		/** ���߳̾����Ч */
		CloseHandle(hListenThread);
		hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

SerialPort::SerialPort() {
	hComm = INVALID_HANDLE_VALUE;
	hListenThread = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&csCommunicationSync);
}

bool SerialPort::InitPort(UINT  portNo, UINT  baud, char  parity, UINT  databits, UINT  stopsbits, DWORD dwCommEvents) {

	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!OpenPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���  
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t* pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(hComm, &dcb);
	}

	/**  ��մ��ڻ����� */
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&csCommunicationSync);

	return bIsSuccess == TRUE;

}

bool SerialPort::WriteData(unsigned char* pData, int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&csCommunicationSync);

	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(hComm,/*�ļ����*/pData,/*���ڱ���������ݵ�һ��������*/ length,/*Ҫ������ַ���*/ &BytesToSend,/*ָ��ʵ�ʶ�ȡ�ֽ�����ָ��*/ NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&csCommunicationSync);

	return true;
}

SerialPort::~SerialPort()
{
	CloseListenThread();
	ClosePort();
	DeleteCriticalSection(&csCommunicationSync);
}

void SerialPort::BindToListenThread(void* obj, void(*func)(void*, string)) {
	DelegateFunc = func;
	DelegateObject = obj;
}

void SerialPort::ClearListenThreadDelegate() {
	DelegateFunc = nullptr;
	DelegateObject = nullptr;
}

bool SerialPort::IsPortOpened() const {
	return !(hComm == INVALID_HANDLE_VALUE);
}

bool SerialPort::IsListenThreadOpened() const {
	return !(hListenThread == INVALID_HANDLE_VALUE);
}
