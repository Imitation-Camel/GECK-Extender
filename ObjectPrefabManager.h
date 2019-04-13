#pragma once
#include "Wrappers.h"

namespace cse
{
	namespace objectPrefabs
	{
		class ObjectPrefabManager;

		// all forms are temporary
		struct PrefabObjectPreviewData
		{
			TESPreviewControl*		Parent;

			TESFormArrayT			BaseForms;
			TESObjectREFRArrayT		References;
			NiNode*					RootNode;

			PrefabObjectPreviewData();
			~PrefabObjectPreviewData();

			void					Attach(TESPreviewControl* To);
			void					Detach();
		};

		class PrefabObject
		{
			friend class ObjectPrefabManager;

			enum
			{
				kState_None = 0,		// operation not performed (Serialize/Deserialize)
				kState_Good,			// operation completed successfully
				kState_Bad,				// errors encountered during operation/operation couldn't be performed
			};

			TESFileWrapper*								SourceFile;
			ObjectRefCollectionSerializer*				Serializer;
			ObjectRefCollectionInstantiator*			Instantiator;

			UInt8										SerializationState;
			UInt8										DeserializationState;
			std::string									FileName;
			std::string									FilePath;			// as passed to the c'tor

			void										SetInDialog(HWND Dialog);

			UInt8										Deserialize(bool Force = false);				// returns the state
			UInt8										Serialize(bgsee::FormListT& Forms, bool Force = false);
		public:
			PrefabObject(const char* SourceFilePath, const char* RepositoryPath, bool OverwriteExisting);
			~PrefabObject();

			PrefabObjectPreviewData*					GeneratePreviewData(TESPreviewControl* PreviewControl);		// caller takes ownership of pointer
			bool										Instantiate();
		};

		typedef std::shared_ptr<PrefabObject>			PrefabObjectHandleT;

		class ObjectPrefabManager
		{
			static INT_PTR CALLBACK				DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static int CALLBACK					SortComparator(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

			typedef std::vector<PrefabObjectHandleT>	PrefabObjectArrayT;

			static const bgsee::ResourceLocation		kRepositoryPath;
			static const char*							kPrefabFileExtension;

			PrefabObjectArrayT			LoadedPrefabs;

			PrefabObject*				CurrentSelection;
			PrefabObjectPreviewData*	PreviewData;

			HWND						MainDialog;
			UInt32						TimeCounter;
			BaseExtraList*				ExtraDataList;
			TESPreviewControl*			Renderer;
			bool						RefreshingList;

			void						InitializeDialog(HWND Dialog);
			void						DeinitializeDialog(HWND Dialog);

			void						RefreshPrefabList();
			void						LoadPrefabsInDirectory(const char* DirectoryPath);

			void						ReloadPrefabs();
			void						NewPrefab();
			void						InstatiateSelection();
			void						RemoveSelection();

			bool						ShowFileDialog(bool Save, std::string& OutPath, std::string& OutName);
			bool						GetExistingPrefab(const char* FilePath, PrefabObjectHandleT& Out);
			bool						RemoveLoadedPrefab(PrefabObject* Data);
		public:
			ObjectPrefabManager();
			~ObjectPrefabManager();

			void							Show();
			void							Close();

			static ObjectPrefabManager		Instance;
		};
	}
}