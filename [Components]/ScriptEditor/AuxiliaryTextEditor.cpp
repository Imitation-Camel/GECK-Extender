#include "AuxiliaryTextEditor.h"
#include "NumberedRichTextBox.h"

namespace cse
{
	namespace textEditors
	{
		ScriptOffsetViewer::ScriptOffsetViewer(Font^ FontData, Color ForegroundColor, Color BackgroundColor, Color HighlightColor, Control^% Parent)
		{
			Viewer = gcnew OffsetRichTextBox(6, gcnew Font(FontData->FontFamily, FontData->Size - 3, FontStyle::Regular), ForegroundColor, BackgroundColor, HighlightColor);
			InitializationState = false;

			Viewer->OffsetFlag = true;
			Viewer->GetTextField()->ReadOnly = true;
			Viewer->GetTextField()->Enabled = true;
			Viewer->GetContainer()->Visible = false;

			Parent->Controls->Add(Viewer->GetContainer());
		}

		bool ScriptOffsetViewer::InitializeViewer(String^ ScriptText, void* Data, UInt32 Length)
		{
			Viewer->GetTextField()->Text = ScriptText;
			if (Viewer->CalculateLineOffsetsForTextField(Data, Length))
				InitializationState = true;
			else
				InitializationState = false;

			return InitializationState;
		}

		int ScriptOffsetViewer::Hide()
		{
			Viewer->GetContainer()->Hide();
			return Viewer->GetTextField()->SelectionStart;
		}

		bool ScriptOffsetViewer::Show( int CaretPosition )
		{
			if (InitializationState == false)
				return false;

			Viewer->GetContainer()->BringToFront();
			Viewer->GetContainer()->Show();

			if (Viewer->GetTextField()->TextLength > CaretPosition)
			{
				Viewer->GetTextField()->SelectionStart = CaretPosition;
				Viewer->GetTextField()->SelectionLength = 0;
				Viewer->GetTextField()->ScrollToCaret();
			}
			return true;
		}

		void ScriptOffsetViewer::Reset( void )
		{
			InitializationState = false;
		}

		void ScriptOffsetViewer::SetFont( Font^ NewFont )
		{
			Viewer->SetFont(gcnew Font(NewFont->FontFamily, NewFont->Size - 3, FontStyle::Regular));
		}

		UInt16 ScriptOffsetViewer::GetLastOffset()
		{
			if (InitializationState == false)
				return 0;
			else
				return Viewer->GetLastOffset();
		}

		void ScriptOffsetViewer::SetForegroundColor(Color Foreground)
		{
			Viewer->SetForegroundColor(Foreground);
		}

		void ScriptOffsetViewer::SetBackgroundColor(Color Background)
		{
			Viewer->SetBackgroundColor(Background);
		}

		SimpleTextViewer::SimpleTextViewer(Font^ FontData, Color ForegroundColor, Color BackgroundColor, Color HighlightColor, Control^% Parent)
		{
			Viewer = gcnew NumberedRichTextBox(6, gcnew Font(FontData->FontFamily, FontData->Size - 3, FontStyle::Regular), ForegroundColor, BackgroundColor, HighlightColor);

			Viewer->GetTextField()->ReadOnly = true;
			Viewer->GetTextField()->Enabled = true;
			Viewer->GetContainer()->Visible = false;

			Parent->Controls->Add(Viewer->GetContainer());
		}

		int SimpleTextViewer::Hide()
		{
			Viewer->GetContainer()->Hide();
			return Viewer->GetTextField()->SelectionStart;
		}

		void SimpleTextViewer::Show( String^ Text, int CaretPosition )
		{
			Viewer->GetTextField()->Text = Text;

			Viewer->GetContainer()->BringToFront();
			Viewer->GetContainer()->Show();

			if (Text->Length > CaretPosition)
			{
				Viewer->GetTextField()->SelectionStart = CaretPosition;
				Viewer->GetTextField()->SelectionLength = 0;
				Viewer->GetTextField()->ScrollToCaret();
			}
		}

		void SimpleTextViewer::SetFont( Font^ NewFont )
		{
			Viewer->SetFont(gcnew Font(NewFont->FontFamily, NewFont->Size - 3, FontStyle::Regular));
		}

		void SimpleTextViewer::SetForegroundColor(Color Foreground)
		{
			Viewer->SetForegroundColor(Foreground);
		}

		void SimpleTextViewer::SetBackgroundColor(Color Background)
		{
			Viewer->SetBackgroundColor(Background);
		}
	}
}