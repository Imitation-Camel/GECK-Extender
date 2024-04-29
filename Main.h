#pragma once
#include "[Common]\ComponentDLLInterface.h"

namespace gecke
{
	extern bool								shadeMeMode;
	extern bool								IsWarholAGenius;

	class ReleaseNameTable : public bgsee::ReleaseNameTable
	{
	protected:
		ReleaseNameTable();
	public:
		virtual ~ReleaseNameTable();

		static ReleaseNameTable						Instance;
	};

	class InitCallbackQuery : public bgsee::DaemonCallback
	{
		const NVSEInterface*				NVSE;
	public:
		InitCallbackQuery(const NVSEInterface* NVSE);
		virtual ~InitCallbackQuery();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class InitCallbackLoad : public bgsee::DaemonCallback
	{
		const NVSEInterface*				NVSE;
	public:
		InitCallbackLoad(const NVSEInterface* NVSE);
		virtual ~InitCallbackLoad();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class InitCallbackPostMainWindowInit : public bgsee::DaemonCallback
	{
		static int __CRTDECL CrtNewHandler(size_t);
		static void __CRTDECL CrtPureCallHandler(void);
		static void __CRTDECL  CrtInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

		void RegisterCrtExceptionHandlers();
	public:
		virtual ~InitCallbackPostMainWindowInit();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class InitCallbackEpilog : public bgsee::DaemonCallback
	{
	public:
		virtual ~InitCallbackEpilog();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class DeinitCallback : public bgsee::DaemonCallback
	{
	public:
		virtual ~DeinitCallback();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class CrashCallback : public bgsee::DaemonCallback
	{
		bool				HandlerCalled;		// to prevent the handler from being called multiple times (which might happen in case of catastrophic failures)
	public:
		CrashCallback();
		virtual ~CrashCallback();

		virtual bool						Handle(void* Parameter = nullptr);
	};

	class StartupManager
	{
	public:
		static void							LoadStartupWorkspace();
		static void							LoadStartupPlugin();
		static void							LoadStartupScript();
	};

	void GECKSEInteropHandler(NVSEMessagingInterface::Message* Msg);
	void NVSEMessageHandler(NVSEMessagingInterface::Message* Msg);

#define BGSEEMAIN_EXTENDERLONGNAME		"GECK Extender"
#define BGSEEMAIN_EXTENDERSHORTNAME		"GECKE"

// bgsee::ResourceLocation paths
#define GECKE_CODADEPOT					"Coda\\"
#define GECKE_CODABGDEPOT				"Coda\\Background\\"
#define GECKE_SEDEPOT					"Script Editor\\"
#define GECKE_SEPREPROCDEPOT			"Script Editor\\Preprocessor\\"
#define GECKE_SEPREPROCSTDDEPOT			"Script Editor\\Preprocessor\\STD\\"
#define GECKE_SESNIPPETDEPOT			"Script Editor\\Snippets\\"
#define GECKE_SEAUTORECDEPOT			"Script Editor\\Auto-Recovery Cache\\"
#define GECKE_OPALDEPOT					"OPAL\\"
#define GECKE_PREFABDEPOT				"Object Prefabs\\"
#define GECKE_COSAVEDEPOT				"Plugin Cosaves\\"
}

extern "C"
{
	__declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info);
	__declspec(dllexport) bool NVSEPlugin_Load(const NVSEInterface * nvse);
};
