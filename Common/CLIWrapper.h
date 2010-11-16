#pragma once
#include "ExtenderInternals.h"
#include "SEHooks.h"
#include "Common\HandShakeStructs.h"

namespace CLIWrapper
{
	namespace ScriptEditor
	{
		typedef void							(*_AllocateNewEditor)(UInt32, UInt32, UInt32, UInt32);
		typedef void							(*_InitializeScript)(UInt32, ScriptData*);
		typedef void							(*_SendMessagePingback)(UInt32, UInt16);

		typedef void							(*_InitializeComponents)(CommandTableData*);
		typedef void							(*_AddToURLMap)(const char*, const char*);
		typedef void							(*_SetScriptListItemData)(UInt32, ScriptData*);

		typedef void							(*_SetVariableListItemData)(UInt32, ScriptVarIndexData::ScriptVarInfo*);
		typedef void							(*_InitializeDatabaseUpdateTimer)();

		extern _AllocateNewEditor				AllocateNewEditor;
		extern _InitializeScript				InitializeScript;
		extern _SendMessagePingback				SendMessagePingback;


		extern _InitializeComponents			InitializeComponents;
		extern _AddToURLMap						AddToURLMap;
		extern _SetScriptListItemData			SetScriptListItemData;

		extern _SetVariableListItemData			SetVariableListItemData;
		extern _InitializeDatabaseUpdateTimer	InitializeDatabaseUpdateTimer;
	}
	namespace UseInfoList
	{
		typedef void							(*_OpenUseInfoBox)(void);
		typedef void							(*_SetFormListItemData)(FormData*);
		typedef void							(*_SetUseListObjectItemData)(FormData*);
		typedef void							(*_SetUseListCellItemData)(UseListCellItemData*);

		extern _OpenUseInfoBox					OpenUseInfoBox;
		extern _SetFormListItemData				SetFormListItemData;
		extern _SetUseListObjectItemData		SetUseListObjectItemData;
		extern _SetUseListCellItemData			SetUseListCellItemData;
	}
	namespace BSAViewer
	{
		typedef const char*						(*_InitializeViewer)(const char*, const char*);

		extern _InitializeViewer				InitializeViewer;
	}
	namespace BatchEditor
	{
		typedef bool							(*_InitializeRefBatchEditor)(BatchRefData*);
		typedef void							(*_AddFormListItem)(FormData*, UInt8);

		extern _InitializeRefBatchEditor		InitializeRefBatchEditor;
		extern _AddFormListItem					AddFormListItem;
	}
	bool										Import(const OBSEInterface * obse);
}