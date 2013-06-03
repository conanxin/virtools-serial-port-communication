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

//四个前向声明函数
CKObjectDeclaration	*FillBehaviortestCOMDecl();			//BB注册函数
CKERROR CreatetestCOMProto(CKBehaviorPrototype **);		
int testCOM(const CKBehaviorContext& BehContext);
int testCOMCallBack(const CKBehaviorContext& BehContext);

CKObjectDeclaration	*FillBehaviortestCOMDecl()			//BB注册函数
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
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("testCOM");	//建立描述BB特征的行为对象，传入对象的名字
	if (!proto) 
	{
		return CKERR_OUTOFMEMORY;
	}

//---     Inputs declaration
	proto->DeclareInput("In0");			//声明输入端口

//---     Outputs declaration
	proto->DeclareOutput("Out0");		//声明输出端口

//---     Input Parameters declaration
	proto->DeclareInParameter("nPort", CKPGUID_STRING,"COM2");	//输入参数声明

//---     Output Parameters declaration
	proto->DeclareOutParameter("out", CKPGUID_STRING,"0");		//输出参数声明

//----	Local Parameters Declaration

//----	Settings Declaration
	//设置这个BB的回调处理函数
	proto->SetBehaviorCallbackFct(testCOMCallBack, CKCB_BEHAVIORATTACH|CKCB_BEHAVIORDETACH|CKCB_BEHAVIORDELETE|CKCB_BEHAVIOREDITED|CKCB_BEHAVIORSETTINGSEDITED|CKCB_BEHAVIORLOAD|CKCB_BEHAVIORPRESAVE|CKCB_BEHAVIORPOSTSAVE|CKCB_BEHAVIORRESUME|CKCB_BEHAVIORPAUSE|CKCB_BEHAVIORRESET|CKCB_BEHAVIORRESET|CKCB_BEHAVIORDEACTIVATESCRIPT|CKCB_BEHAVIORACTIVATESCRIPT|CKCB_BEHAVIORREADSTATE, NULL);
	
	proto->SetFunction(testCOM);	//设置BB实现自己功能的函数入口

	*pproto = proto;
	return CK_OK;
}

  HANDLE hComm;			//串口句柄
  OVERLAPPED m_ov;		//异步
  COMSTAT comstat;
	
  BOOL m_once=TRUE;

  BOOL  bRead = TRUE;
  BOOL  bResult = TRUE;
  DWORD dwError = 0;
  DWORD BytesRead = 0;
  char RXBuff;
  CString out;
  char nPort[4];

int testCOM(const CKBehaviorContext& BehContext)		//功能实现函数,使用BB时函数循环执行
{


	CKBehavior *P=BehContext.Behavior;		//得到描述Virtools行为的对象

	CKContext* ctx=BehContext.Context;
	//////////////////////////////////////////////

	CKSTRING nPort = (CKSTRING)P->GetInputParameterReadDataPtr(0);		//接收串口号，能够进行设置改变

	if (m_once)		//只执行一次
	{	
		hComm = CreateFile(//_T("nPort"),			
                   nPort,			//串口号
				   GENERIC_READ | GENERIC_WRITE, //允许读写
                   0, //通讯设备必须以独占方式打开
                   0, //无安全属性
                   OPEN_EXISTING, //通讯设备已存在
                   FILE_FLAG_OVERLAPPED, //异步I/O
                   0); //通讯设备不能用模板打开
		if(hComm == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
			 ctx->OutputToConsole("open comport failed");
		}
	


	DCB  dcb;
	int rate= 9600;			//设置波特率
	memset(&dcb,0,sizeof(dcb));

	if(!GetCommState(hComm,&dcb))//获取当前DCB配置
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


//读串口数据



 	bResult = ClearCommError(hComm, &dwError, &comstat);	//清除错误或读取串口现在状态
//    if (comstat.cbInQue == 0)	//判断输入缓冲区字节数是否为0
//	{
       if (bRead)
        {
			bResult = ReadFile(hComm,		//串口句柄 Handle to COMM port
				  		     &RXBuff,			//存储被读出数据首地址 RX Buffer Pointer
						           1,			//准备读出字节个数 Read one byte
						  &BytesRead,			//实际读出字节个数 Stores number of bytes read
						      &m_ov);   		//指向异步结构 pointer to the m_ov structure
		  //  printf("%c",RXBuff);	//打印出被读数据

			ctx->OutputToConsoleEx("%c",RXBuff);

			out.Format("%c",RXBuff);
			P->SetOutputParameterValue(0,out);


			if (!bResult)			//如果读的过程中出现错误
			{
				switch (dwError = GetLastError())		//分析错误结果
				{
					case ERROR_IO_PENDING:			//重叠操作还未完成，线程等待操作完成
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

        if (!bRead)		//如果bRead=FALSE，重叠操作未完成
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
	CKBehavior *P=BehContext.Behavior;		//得到描述Virtools行为的对象
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
		case CKM_BEHAVIORRESET:	//当程序被复位时调用
			
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


