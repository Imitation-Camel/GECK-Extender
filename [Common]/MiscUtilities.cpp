#include "MiscUtilities.h"
#include "NativeWrapper.h"

namespace cse
{
	CString::CString(System::String^ Source)
	{
		P = Marshal::StringToHGlobalAnsi(Source);
	}

	CString::~CString()
	{
		this->Free();
	}

	void ToggleFlag(UInt32* Flag, UInt32 Mask, bool State)
	{
		if (State)	*Flag |= Mask;
		else		*Flag &= ~Mask;
	}

	ImageResourceManager::ImageResourceManager(String^ BaseName)
	{
		Manager = gcnew ResourceManager(BaseName, Assembly::GetExecutingAssembly());
	}

	Image^ ImageResourceManager::CreateImage(String^ ResourceIdentifier)
	{
		try {
			return dynamic_cast<Image^>(Manager->GetObject(ResourceIdentifier));
		} catch (...) {
			return nullptr;
		}
	}

	void ImageResourceManager::SetupImageForToolStripButton(ToolStripButton^ Control)
	{
		Control->Image = CreateImage(Control->Name);
	}

	CSEControlDisposer::CSEControlDisposer( Control^ Source )
	{
		for each (Control^ Itr in Source->Controls)
		{
			try
			{
				CSEControlDisposer Disposer(Itr);
			} catch (...) {}

			delete Itr;
		}
	}

	CSEControlDisposer::CSEControlDisposer( Control::ControlCollection^ Source )
	{
		for each (Control^ Itr in Source)
		{
			try
			{
				CSEControlDisposer Disposer(Itr);
			} catch (...) {}

			delete Itr;
		}
	}
	void CopyStringToCharBuffer( String^% Source, char* Buffer, UInt32 Size )
	{
		int i = 0;
		for (i=0; i < Source->Length && i < Size; i++)
			*(Buffer + i) = Source[i];

		if (i < Size)
			*(Buffer + i) = '\0';
		else
			Buffer[Size - 1] = '\0';
	}
}