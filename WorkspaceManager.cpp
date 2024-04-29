#include "WorkspaceManager.h"
#include "Main.h"

namespace gecke
{
	namespace workspaceManager
	{
		WorkspaceManagerOperator::~WorkspaceManagerOperator()
		{
			;//
		}

		void WorkspaceManagerOperator::ResetCurrentWorkspace()
		{
			NOT_IMPLEMENTED;
		}

		void WorkspaceManagerOperator::ReloadPlugins( const char* WorkspacePath, bool ResetPluginList, bool LoadESPs )
		{
			NOT_IMPLEMENTED;
		}

		void Initialize()
		{
			static bgsee::WorkspaceManager::DefaultDirectoryArrayT kDefaultDirectories;
			if (kDefaultDirectories.size() == 0)
			{
				kDefaultDirectories.push_back("Data\\");
				kDefaultDirectories.push_back("Data\\Meshes\\");
				kDefaultDirectories.push_back("Data\\Textures\\");
				kDefaultDirectories.push_back("Data\\Textures\\menus\\");
				kDefaultDirectories.push_back("Data\\Textures\\menus\\icons\\");
				kDefaultDirectories.push_back("Data\\Sound\\");
				kDefaultDirectories.push_back("Data\\Sound\\fx\\");
				kDefaultDirectories.push_back("Data\\Sound\\Voice\\");
				kDefaultDirectories.push_back("Data\\Trees\\");
				kDefaultDirectories.push_back("Data\\Backup\\");

				kDefaultDirectories.push_back(bgsee::ResourceLocation::GetBasePath());
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_CODADEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_CODABGDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_SEDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_SEPREPROCDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_SEPREPROCSTDDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_SESNIPPETDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_SEAUTORECDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_OPALDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_PREFABDEPOT)()));
				kDefaultDirectories.push_back((bgsee::ResourceLocation(GECKE_COSAVEDEPOT)()));
			}

			bool ComponentInitialized = bgsee::WorkspaceManager::Initialize(BGSEEMAIN->GetAPPPath(), new WorkspaceManagerOperator(), kDefaultDirectories);
			SME_ASSERT(ComponentInitialized);

			// _FILEFINDER->AddSearchPath((std::string(std::string(BGSEEWORKSPACE->GetCurrentWorkspace()) + "Data")).c_str());

		}

		void Deinitialize()
		{
			bgsee::WorkspaceManager::Deinitialize();
		}
	}
}