//////////////////////////////////////////////////////////////////////////////////////////////////////////
//		            testCOM
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define STRICT

#include "CKAll.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <afx.h>

#include <windows.h>

//�ĸ�ǰ����������
CKObjectDeclaration	*FillBehaviortestCOMDecl();			//BBע�ắ��
CKERROR CreatetestCOMProto(CKBehaviorPrototype **);		
int testCOM(const CKBehaviorContext& BehContext);
int testCOMCallBack(const CKBehaviorContext& BehContext);

CKObjectDeclaration	*FillBehaviortestCOMDecl()			//BBע�ắ��
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("testCOM");	
	
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetVersion(0x00000001);
	od->SetCreationFunction(CreatetestCOMProto);
	od->SetDescription("Enter your description here");
	od->SetCategory("UserBBs");
	od->SetGuid(CKGUID(0x8fce3152, 0xfcc191cd));
	od->SetAuthorGuid(CKGUID(0x56495254,0x4f4f4c53));
	od->SetAuthorName("Virtools");
	od->SetCompatibleClassId(CKCID_BEOBJECT);

	return od;
}

CKERROR CreatetestCOMProto(CKBehaviorPrototype** pproto)		
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("testCOM");	//��������BB��������Ϊ���󣬴�����������
	if (!proto) 
	{
		return CKERR_OUTOFMEMORY;
	}

//---     Inputs declaration
	proto->DeclareInput("In0");			//��������˿�

//---     Outputs declaration
	proto->DeclareOutput("Out0");		//��������˿�

//---     Input Parameters declaration
	proto->DeclareInParameter("nPort", CKPGUID_STRING,"COM2");	//�����������

//---     Output Parameters declaration
	proto->DeclareOutParameter("out", CKPGUID_STRING,"0");		//�����������

//----	Local Parameters Declaration

//----	Settings Declaration
	//�������BB�Ļص�������
	proto->SetBehaviorCallbackFct(testCOMCallBack, CKCB_BEHAVIORATTACH|CKCB_BEHAVIORDETACH|CKCB_BEHAVIORDELETE|CKCB_BEHAVIOREDITED|CKCB_BEHAVIORSETTINGSEDITED|CKCB_BEHAVIORLOAD|CKCB_BEHAVIORPRESAVE|CKCB_BEHAVIORPOSTSAVE|CKCB_BEHAVIORRESUME|CKCB_BEHAVIORPAUSE|CKCB_BEHAVIORRESET|CKCB_BEHAVIORRESET|CKCB_BEHAVIORDEACTIVATESCRIPT|CKCB_BEHAVIORACTIVATESCRIPT|CKCB_BEHAVIORREADSTATE, NULL);
	
	proto->SetFunction(testCOM);	//����BBʵ���Լ����ܵĺ������

	*pproto = proto;
	return CK_OK;
}

  HANDLE hComm;			//���ھ��
  OVERLAPPED m_ov;		//�첽
  COMSTAT comstat;
	
  BOOL m_once=TRUE;

  BOOL  bRead = TRUE;
  BOOL  bResult = TRUE;
  DWORD dwError = 0;
  DWORD BytesRead = 0;
  char RXBuff;
  CString out;
  char nPort[4];

int testCOM(const CKBehaviorContext& BehContext)		//����ʵ�ֺ���,ʹ��BBʱ����ѭ��ִ��
{


	CKBehavior *P=BehContext.Behavior;		//�õ�����Virtools��Ϊ�Ķ���

	CKContext* ctx=BehContext.Context;
	//////////////////////////////////////////////

	CKSTRING nPort = (CKSTRING)P->GetInputParameterReadDataPtr(0);		//���մ��ںţ��ܹ��������øı�

	if (m_once)		//ִֻ��һ��
	{	
		hComm = CreateFile(//_T("nPort"),			
                   nPort,			//���ں�
				   GENERIC_READ | GENERIC_WRITE, //�����д
                   0, //ͨѶ�豸�����Զ�ռ��ʽ��
                   0, //�ް�ȫ����
                   OPEN_EXISTING, //ͨѶ�豸�Ѵ���
                   FILE_FLAG_OVERLAPPED, //�첽I/O
                   0); //ͨѶ�豸������ģ���
		if(hComm == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
			 ctx->OutputToConsole("open comport failed");
		}
	


	DCB  dcb;
	int rate= 9600;			//���ò�����
	memset(&dcb,0,sizeof(dcb));

	if(!GetCommState(hComm,&dcb))//��ȡ��ǰDCB����
	{
		return 0;
	}


 // set DCB to configure the serial port
        dcb.DCBlength       = sizeof(dcb);
 // ---------- Serial Port Config -------
        dcb.BaudRate        = rate;
        dcb.Parity      = NOPARITY;
        dcb.fParity     = 0;
        dcb.StopBits        = ONESTOPBIT;
        dcb.ByteSize        = 8;
        dcb.fOutxCtsFlow    = 0;
        dcb.fOutxDsrFlow    = 0;
        dcb.fDtrControl     = DTR_CONTROL_DISABLE;
        dcb.fDsrSensitivity = 0;
        dcb.fRtsControl     = RTS_CONTROL_DISABLE;
        dcb.fOutX           = 0;
        dcb.fInX            = 0;
       //* ----------------- misc parameters ----- 
        dcb.fErrorChar      = 0;
        dcb.fBinary         = 1;
        dcb.fNull           = 0;
        dcb.fAbortOnError   = 0;
        dcb.wReserved       = 0;
        dcb.XonLim          = 2;
        dcb.XoffLim         = 4;
        dcb.XonChar         = 0x13;
        dcb.XoffChar        = 0x19;
        dcb.EvtChar         = 0;
  //* --------------------------------------------------------------------
        // set DCB
     if(!SetCommState(hComm,&dcb))
        {
        return 0;
        }
	
		ctx->OutputToConsole("setupDCB success");


//*---------------------------------------------------------------------------
		DWORD ReadInterval=0;
		DWORD ReadTotalMultiplier=0;
		DWORD ReadTotalconstant=0;
		DWORD WriteTotalMultiplier=0;
		DWORD WriteTotalconstant=0;

		 COMMTIMEOUTS timeouts;
       timeouts.ReadIntervalTimeout=ReadInterval;
       timeouts.ReadTotalTimeoutConstant=ReadTotalconstant;
       timeouts.ReadTotalTimeoutMultiplier=ReadTotalMultiplier;
       timeouts.WriteTotalTimeoutConstant=WriteTotalconstant;
       timeouts.WriteTotalTimeoutMultiplier=WriteTotalMultiplier;

     if(!SetCommTimeouts(hComm, &timeouts))
       {
	    return 0;
       }	

	   ctx->OutputToConsole("setuptimeout success");
		
	   PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	   ctx->OutputToConsole("open comport success");
	
		m_once=FALSE;
}


//����������



 	bResult = ClearCommError(hComm, &dwError, &comstat);	//���������ȡ��������״̬
//    if (comstat.cbInQue == 0)	//�ж����뻺�����ֽ����Ƿ�Ϊ0
//	{
       if (bRead)
        {
			bResult = ReadFile(hComm,		//���ھ�� Handle to COMM port
				  		     &RXBuff,			//�洢�����������׵�ַ RX Buffer Pointer
						           1,			//׼�������ֽڸ��� Read one byte
						  &BytesRead,			//ʵ�ʶ����ֽڸ��� Stores number of bytes read
						      &m_ov);   		//ָ���첽�ṹ pointer to the m_ov structure
		  //  printf("%c",RXBuff);	//��ӡ����������

			ctx->OutputToConsoleEx("%c",RXBuff);

			out.Format("%c",RXBuff);
			P->SetOutputParameterValue(0,out);


			if (!bResult)			//������Ĺ����г��ִ���
			{
				switch (dwError = GetLastError())		//����������
				{
					case ERROR_IO_PENDING:			//�ص�������δ��ɣ��̵߳ȴ��������
					{
						bRead = FALSE;
						ctx->OutputToConsole("failed ERROR_IO_PENDING");
						break;
					}
					default:
					{
						break;
					}
				}
			}	//close if (!bResult)
			else
			{
				bRead = TRUE;

				ctx->OutputToConsole("succsed ERROR_IO_PENDING");
			}
		}  // close if (bRead)

        if (!bRead)		//���bRead=FALSE���ص�����δ���
        {
			bRead = TRUE;
			bResult = GetOverlappedResult(hComm,	// Handle to COMM port
				                          &m_ov,	// Overlapped structure
									 &BytesRead,	// Stores number of bytes read
										  TRUE); 	// Wait flag

			ctx->OutputToConsole("succsed !bRead");
        }	//close if (!bRead)

//	}


	P->ActivateOutput(0,TRUE);
	//////////////////////////////////////

	return CKBR_ACTIVATENEXTFRAME;
}



int testCOMCallBack(const CKBehaviorContext& BehContext)
{
	CKBehavior *P=BehContext.Behavior;		//�õ�����Virtools��Ϊ�Ķ���
	CKContext* ctx=BehContext.Context;

	switch (BehContext.CallbackMessage)
	{
		case CKM_BEHAVIORATTACH:
			break;
		case CKM_BEHAVIORDETACH:
			break;
		case CKM_BEHAVIORDELETE:
			break;
		case CKM_BEHAVIOREDITED:
			break;
		case CKM_BEHAVIORSETTINGSEDITED:
			break;
		case CKM_BEHAVIORLOAD:
			break;
		case CKM_BEHAVIORPRESAVE:
			break;
		case CKM_BEHAVIORPOSTSAVE:
			break;
		case CKM_BEHAVIORRESUME:
			break;
		case CKM_BEHAVIORPAUSE:
			break;
		case CKM_BEHAVIORRESET:	//�����򱻸�λʱ����
			
			ctx->OutputToConsole("close comport");
			break;
		case CKM_BEHAVIORNEWSCENE:
			break;
		case CKM_BEHAVIORDEACTIVATESCRIPT:
			break;
		case CKM_BEHAVIORACTIVATESCRIPT:
			break;
		case CKM_BEHAVIORREADSTATE:
			break;

	}
	return CKBR_OK;
}


