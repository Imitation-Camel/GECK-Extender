#include "Serialization.h"

namespace cse
{
	namespace serialization
	{
		PluginCosaveManager				PluginCosaveManager::Instance;
		const bgsee::ResourceLocation	PluginCosaveManager::kRepositoryPath(CSE_COSAVEDEPOT);


		PluginCosaveManager::GlobalEventSink::GlobalEventSink(PluginCosaveManager* Parent) :
			Parent(Parent)
		{
			SME_ASSERT(Parent);
		}

		void PluginCosaveManager::GlobalEventSink::Handle(SME::MiscGunk::IEventData* Data)
		{
			SME_ASSERT(Parent->Initialized);
			const events::TypedEventSource* Source = dynamic_cast<const events::TypedEventSource*>(Data->Source);
			SME_ASSERT(Source);

			switch (Source->GetTypeID())
			{
			case events::TypedEventSource::kType_Shutdown:
				Parent->HandleShutdown();
				break;
			case events::TypedEventSource::kType_Plugin_PostLoad:
				Parent->HandleLoad();
				break;
			case events::TypedEventSource::kType_Plugin_PostSave:
				Parent->HandleSave();
				break;
			}
		}

		void PluginCosaveManager::CleanupEmptyDirectories() const
		{
			try
			{
				std::filesystem::path CosaveDir(kRepositoryPath());

				if (std::filesystem::exists(CosaveDir) == false)
					return;

				for (std::filesystem::directory_iterator Itr(CosaveDir); Itr != std::filesystem::directory_iterator(); ++Itr)
				{
					if (std::filesystem::is_directory(Itr->path()) && GetFileCount(Itr->path()) == 0)
					{
						if (std::filesystem::remove(Itr->path()) == false)
							BGSEECONSOLE_MESSAGE("Couldn't remove empty cosave directory %s", Itr->path().string().c_str());
					}
				}
			}
			catch (std::exception& e)
			{
				BGSEECONSOLE_MESSAGE("Couldn't get cleanup cosave directories. Exception - %s", e.what());
			}
		}

		int PluginCosaveManager::GetFileCount(const std::filesystem::path& DirectoryPath) const
		{
			try
			{
				if (std::filesystem::exists(DirectoryPath) == false || std::filesystem::is_directory(DirectoryPath) == false)
					return 0;

				int Count = 0;
				for (std::filesystem::directory_iterator Itr(DirectoryPath); Itr != std::filesystem::directory_iterator(); ++Itr)
					Count++;

				return Count;
			}
			catch (std::exception& e)
			{
				BGSEECONSOLE_MESSAGE("Couldn't get file count for %s. Exception - %s", DirectoryPath.string().c_str(), e.what());
				return 0;
			}
		}

		bool PluginCosaveManager::CreateCosaveDirectory(TESFile* File) const
		{
			SME_ASSERT(File);

			try
			{
				std::string OutDir(kRepositoryPath());
				OutDir += "\\" + std::string(File->fileName) + "\\";

				std::filesystem::path CosavePath(OutDir);
				if (std::filesystem::exists(CosavePath))
				{
					if (std::filesystem::is_directory(CosavePath) == false)
					{
						BGSEECONSOLE_MESSAGE("Couldn't create cosave directory at %s - Another file with the same name exists", OutDir.c_str());
						return false;
					}
					else
						return true;
				}

				if (std::filesystem::create_directory(CosavePath) == false)
				{
					BGSEECONSOLE_MESSAGE("Couldn't create cosave directory at %s", OutDir.c_str());
					return false;
				}

				return true;
			}
			catch (std::exception& e)
			{
				BGSEECONSOLE_MESSAGE("Couldn't create cosave directory. Exception - %s", e.what());
				return false;
			}
		}

		bool PluginCosaveManager::GetCosaveDirectory(TESFile* File, std::string& OutPath, bool Create /*= true*/) const
		{
			SME_ASSERT(File);
			bool Result = false;
			try
			{
				std::string OutDir(kRepositoryPath());
				OutDir += "\\" + std::string(File->fileName) + "\\";

				std::filesystem::path CosavePath(OutDir);
				if (std::filesystem::exists(CosavePath) == false)
				{
					if (Create)
					{
						Result = CreateCosaveDirectory(File);
						if (Result)
							OutPath = OutDir;
					}
				}
				else
				{
					if (std::filesystem::is_directory(CosavePath) == false)
						BGSEECONSOLE_MESSAGE("Couldn't find cosave directory at %s - Found a file with the same name instead", OutDir.c_str());
					else
					{
						Result = true;
						OutPath = OutDir;
					}
				}
			}
			catch (std::exception& e)
			{
				BGSEECONSOLE_MESSAGE("Couldn't get cosave directory. Exception - %s", e.what());
			}

			return Result;
		}

		void PluginCosaveManager::HandleLoad() const
		{
			if (_DATAHANDLER->activeFile == nullptr)
				return;
			else if (Handlers.size() == 0)
				return;

			std::string CosavePath;
			if (GetCosaveDirectory(_DATAHANDLER->activeFile, CosavePath))
			{
				SME::MiscGunk::ScopedSetter<bool> Sentry(InvokingHandlers, true);
				for (auto Itr : Handlers)
					Itr->HandleLoad(_DATAHANDLER->activeFile->fileName, CosavePath.c_str());
			}
		}

		void PluginCosaveManager::HandleSave() const
		{
			SME_ASSERT(_DATAHANDLER->activeFile);
			if (Handlers.size() == 0)
				return;
			else if (strstr(_DATAHANDLER->activeFile->fileName, "AutoSave") == _DATAHANDLER->activeFile->fileName)
				return;		// don't bother when autosaving

			std::string CosavePath;
			if (GetCosaveDirectory(_DATAHANDLER->activeFile, CosavePath))
			{
				SME::MiscGunk::ScopedSetter<bool> Sentry(InvokingHandlers, true);
				for (auto Itr : Handlers)
					Itr->HandleSave(_DATAHANDLER->activeFile->fileName, CosavePath.c_str());
			}
		}

		void PluginCosaveManager::HandleShutdown() const
		{
			if (_DATAHANDLER->activeFile == nullptr)
				return;
			else if (Handlers.size() == 0)
				return;

			std::string CosavePath;
			if (GetCosaveDirectory(_DATAHANDLER->activeFile, CosavePath))
			{
				SME::MiscGunk::ScopedSetter<bool> Sentry(InvokingHandlers, true);
				for (auto Itr : Handlers)
					Itr->HandleShutdown(_DATAHANDLER->activeFile->fileName, CosavePath.c_str());
			}
		}

		PluginCosaveManager::PluginCosaveManager() :
			Handlers()
		{
			EventSink = new GlobalEventSink(this);
			Initialized = false;
			InvokingHandlers = false;
		}

		PluginCosaveManager::~PluginCosaveManager()
		{
			DEBUG_ASSERT(Initialized == false);

			SAFEDELETE(EventSink);
		}

		void PluginCosaveManager::Initialize()
		{
			SME_ASSERT(Initialized == false);

			events::general::kShutdown.AddSink(EventSink);
			events::plugin::kPostLoad.AddSink(EventSink);
			events::plugin::kPostSave.AddSink(EventSink);

			CleanupEmptyDirectories();

			Initialized = true;
		}

		void PluginCosaveManager::Deinitialize()
		{
			SME_ASSERT(Initialized && InvokingHandlers == false);
			SME_ASSERT(Handlers.size() == 0);

			CleanupEmptyDirectories();

			events::general::kShutdown.RemoveSink(EventSink);
			events::plugin::kPostLoad.RemoveSink(EventSink);
			events::plugin::kPostSave.RemoveSink(EventSink);

			Initialized = false;
		}

		void PluginCosaveManager::Register(IEventHandler* Handler)
		{
			SME_ASSERT(Initialized);
			SME_ASSERT(Handler);
			SME_ASSERT(InvokingHandlers == false);

			if (std::find(Handlers.begin(), Handlers.end(), Handler) == Handlers.end())
				Handlers.push_back(Handler);
		}


		void PluginCosaveManager::Unregister(IEventHandler* Handler)
		{
			SME_ASSERT(Initialized);
			SME_ASSERT(Handler);
			SME_ASSERT(InvokingHandlers == false);

			HandlerArrayT::iterator Match = std::find(Handlers.begin(), Handlers.end(), Handler);
			if (Match != Handlers.end())
				Handlers.erase(Match);
		}

		const char*			TESForm2Text::kSigil			= "TESForm";
		const char*			TESForm2Text::kNullEditorID		= "<null>";

		TESForm2Text::~TESForm2Text()
		{
			;//
		}

		bool TESForm2Text::Serialize(const TESForm* In, std::string& Out)
		{
			SME_ASSERT(In);

			bool Result = true;
			char Buffer[0x100] = { 0 };
			TESFile* Parent = In->GetOverrideFile(-1);
			FORMAT_STR(Buffer, "%s{%08X|%d|%s|%s}", kSigil, In->formID, (UInt32)In->formType,
					   (In->GetEditorID() == nullptr ? kNullEditorID : In->GetEditorID()),
					   (Parent == nullptr ? "Oblivion.esm" : Parent->fileName));
			Out = Buffer;
			return Result;
		}

		bool TESForm2Text::Deserialize(std::string In, TESForm** Out)
		{
			SME_ASSERT(Out);
			bool Result = false;

			if (In.length() < 0x100)
			{
				char String[0x100] = { 0 };
				UInt32 FormID = 0;
				UInt32 TypeID = 0;
				std::string EditorID, Plugin;

				if (In.find(kSigil) == 0)
				{
					std::string Data(In.substr(strlen(kSigil)));
					if (Data.length() > 1)
					{
						SME::StringHelpers::Tokenizer Extractor(Data.c_str(), "{}|");
						bool HasError = false;
						int ArgCount = 0;
						while (true)
						{
							std::string CurrentArg;
							if (Extractor.NextToken(CurrentArg) != -1)
							{
								if (CurrentArg.empty())
									continue;

								switch (ArgCount + 1)
								{
								case 1:
									if (sscanf_s(CurrentArg.c_str(), "%08X", &FormID) != 1)
									{
										HasError = true;
										BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Invalid FormID (%s)", CurrentArg.c_str());
									}
									break;
								case 2:
									if (sscanf_s(CurrentArg.c_str(), "%d", &TypeID) != 1)
									{
										HasError = true;
										BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Invalid TypeID (%s)", CurrentArg.c_str());
									}
									break;
								case 3:
									EditorID = CurrentArg;
									break;
								case 4:
									Plugin = CurrentArg;
									break;
								}

								if (HasError)
									break;

								ArgCount++;
							}
							else
								break;
						}

						if (HasError == false && ArgCount == 4)
						{
							TESFile* ParentPlugin = _DATAHANDLER->LookupPluginByName(Plugin.c_str());
							if (ParentPlugin && _DATAHANDLER->IsPluginLoaded(ParentPlugin))
							{
								FormID = (ParentPlugin->fileIndex << 24) | (FormID & 0x00FFFFFF);
								TESForm* Form = TESForm::LookupByFormID(FormID);
								if (Form)
								{
									bool GoodEditorID = (Form->GetEditorID() == nullptr && strcmp(EditorID.c_str(), kNullEditorID) == 0) || Form->editorID.Compare(EditorID.c_str()) == 0;
									if (Form && Form->formType == TypeID && GoodEditorID)
									{
										Result = true;
										*Out = Form;
									}
									else
									{
										BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - TypeID/EditorID mismatch! Found (%d, %s), Expected (%d, %s)",
															 TypeID, EditorID.c_str(), Form->formType, Form->GetEditorID());
									}
								}
								else
									BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Couldn't find form with formID %08X", FormID);
							}
							else
								BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Parent plugin %s was not loaded", Plugin.c_str());
						}
						else
							BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Couldn't extract data completely (%s)", In.c_str());
					}
					else
						BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Input too short");
				}
				else
					BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Invalid sigil (%s)", In.c_str());
			}
			else
				BGSEECONSOLE_MESSAGE("TESForm2Text::Deserialize - Input too long (length = %d)", In.size());

			return Result;
		}



		void Initialize()
		{
			PluginCosaveManager::Instance.Initialize();
		}

		void Deinitialize()
		{
			PluginCosaveManager::Instance.Deinitialize();
		}
	}
}
