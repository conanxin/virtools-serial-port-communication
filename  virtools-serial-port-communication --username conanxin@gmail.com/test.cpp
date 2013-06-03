// test.cpp : Defines the initialization routines for the plugin DLL.
//
#include "CKAll.h"


CKERROR InitInstance(CKContext* context);
CKERROR ExitInstance(CKContext* context);

#define PLUGIN_COUNT 1

CKPluginInfo g_PluginInfo[PLUGIN_COUNT];

int CKGetPluginInfoCount() {
	return PLUGIN_COUNT;
}

CKPluginInfo* CKGetPluginInfo(int Index)
{
	int Plugin = 0;
	g_PluginInfo[Plugin].m_Author			= "Virtools";
	g_PluginInfo[Plugin].m_Description		= "Enter your description here";
	g_PluginInfo[Plugin].m_Extension		= "";
	g_PluginInfo[Plugin].m_Type				= CKPLUGIN_BEHAVIOR_DLL;
	g_PluginInfo[Plugin].m_Version			= 0x00010000;
	g_PluginInfo[Plugin].m_InitInstanceFct	= NULL;
	g_PluginInfo[Plugin].m_GUID				= CKGUID(0x8fce3152, 0xfcc191cd);
	g_PluginInfo[Plugin].m_Summary			= "Enter your summary here";

	return &g_PluginInfo[Index];
}

//  If no manager is used in the plugin 
//	these functions are optional and can be exported.
//  Virtools will call 'InitInstance' when loading the behavior library
//  and 'ExitInstance' when unloading it.
//	It is a good place to perform Attributes Types declaration, 
//  registering new enums or new parameter types.

CKERROR InitInstance(CKContext* context)
{
  
	return CK_OK;
}

CKERROR ExitInstance(CKContext* context)
{
	// This function will only be called if the dll is unloaded explicitely 
	// by a user in Virtools Dev interface
	// Otherwise the manager destructor will be called by Virtools runtime directly
  
	return CK_OK;
}

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
	RegisterBehavior(reg, FillBehaviortestCOMDecl);
}






