#pragma once

namespace cse
{
	namespace objectPalette
	{
		// palette data is serialized to INI files
		typedef std::string		SerializedData;

		class ObjectPaletteManager;

		class PaletteObject
		{
			friend class ObjectPaletteManager;

			std::string			Name;
			TESForm*			BaseObject;

			Vector3				AngleBase;
			Vector3				AngleOffset;

			Vector2				Sink;			// x = base, y = offset
			Vector2				Scale;			// x = base, y = offset

			bool				ConformToSlope;

			void				Serialize(SerializedData& OutData) const;
			void				Deserialize(const SerializedData& Data);

			void				GetFromDialog(HWND Dialog);
			void				SetInDialog(HWND Dialog) const;

			void				Reset();
		public:
			PaletteObject();
			PaletteObject(TESForm* Base);
			PaletteObject(const SerializedData& Data);
			~PaletteObject();

			TESObjectREFR*		Instantiate(const Vector3& Position, bool MarkAsTemporary) const;
		};

		typedef std::shared_ptr<PaletteObject>	PaletteObjectHandleT;

		class ObjectPaletteManager
		{
			static INT_PTR CALLBACK				DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static int CALLBACK					SortComparator(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

			typedef std::vector<PaletteObjectHandleT>	PaletteObjectArrayT;

			static const bgsee::ResourceLocation		kRepositoryPath;
			static const char*							kPaletteFileExtension;

			PaletteObjectArrayT		LoadedObjects;
			PaletteObjectArrayT		CurrentSelection;

			TESObjectREFR*			PreviewRef;

			HWND					MainDialog;
			std::string				CurrentPaletteFilename;
			UInt32					TimeCounter;
			BaseExtraList*			ExtraDataList;
			TESPreviewControl*		Renderer;
			PaletteObject*			ActiveObject;
			bool					RefreshingList;

			void					InitializeDialog(HWND Dialog);
			void					DeinitializeDialog(HWND Dialog);

			void					RefreshObjectList();
			void					UpdateSelectionList();
			void					RemoveSelection();

			void					SaveObjects(const PaletteObjectArrayT& Objects, const char* Path) const;
			bool					LoadObjects(PaletteObjectArrayT& OutObjects, const char* Path) const;			// returns false if an error was encountered

			void					NewPalette();
			void					SavePalette(bool NewFile);
			void					LoadPalette();
			void					MergePalette();

			void					UpdatePreview();
			void					HandleDragDrop();

			void					EnableControls(bool State) const;
			bool					GetFormTypeAllowed(UInt8 Type) const;
			bool					ShowFileDialog(bool Save, std::string& OutPath, std::string& OutName);
			bool					GetBaseHandle(PaletteObject* Data, PaletteObjectHandleT& Out) const;
			bool					RemoveLoadedObject(PaletteObject* Data);
		public:
			ObjectPaletteManager();
			~ObjectPaletteManager();

			bool					PlaceObject(int X, int Y) const;			// render window coords, returns true if successful

			void					Show();
			void					Close();

			static ObjectPaletteManager			Instance;
		};
	}
}