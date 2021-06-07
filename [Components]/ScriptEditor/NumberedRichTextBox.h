#pragma once

namespace cse
{
	namespace textEditors
	{
		ref class LineChangedEventArgs : public EventArgs
		{
		public:
			property UInt32								CurrentLine;
			property UInt32								PreviousLine;

			LineChangedEventArgs(UInt32 CurrentLine, UInt32 PreviousLine) : EventArgs()
			{
				this->CurrentLine = CurrentLine;
				this->PreviousLine = PreviousLine;
			}
		};

		delegate void											LineChangedEventHandler(Object^ Sender, LineChangedEventArgs^ E);

		ref class NumberedRichTextBox
		{
		public:
			virtual void										UpdateLineNumbers(void);

			event LineChangedEventHandler^						LineChanged;
		protected:
			ref class SimpleScrollRTB : public RichTextBox				// a richtextbox implementation that disables smooth scrolling
			{
			public:
				property UInt32									LinesToScroll;
			protected:
				virtual	void									WndProc(Message% m) override;
			};

			virtual void										OnLineChanged(LineChangedEventArgs^ E);

			void												UpdateLineNumbers_EventHandler(Object^ Sender, EventArgs^ E) { UpdateLineNumbers(); }

			virtual void										LineField_MouseDown(Object^ Sender, MouseEventArgs^ E);

			void												TextField_TextChanged(Object^ Sender, EventArgs^ E);
			void												TextField_KeyDownAndUp(Object^ Sender, KeyEventArgs^ E);
			void												TextField_KeyPress(Object^ Sender, KeyPressEventArgs^ E);
			void												TextField_MouseDownAndUp(Object^ Sender, MouseEventArgs^ E);

			void												NumberedRichTextBox_LineChanged(Object^ Sender, LineChangedEventArgs^ E);

			SplitContainer^										Splitter;
			RichTextBox^										LineField;
			SimpleScrollRTB^									TextField;

			Color												ForegroundColor;
			Color												BackgroundColor;
			Color												HighlightColor;

			int													LineChangeBuffer;

			EventHandler^										UpdateLineNumbersEventHandlerHandler;
			MouseEventHandler^									LineFieldMouseDownHandler;
			EventHandler^										TextFieldTextChangedHandler;
			KeyEventHandler^									TextFieldKeyDownAndUpHandler;
			MouseEventHandler^									TextFieldMouseDownAndUpHandler;
			LineChangedEventHandler^							NumberedRichTextBoxLineChangedHandler;

			virtual int											SelectLineNumberInLineField(UInt32 Line);	// parameter is one-based line number
			virtual void										HighlightLineNumbers(void);
			virtual void										DrawLineNumbers(void);

			virtual void										ValidateLineChange(void);

			void												GotoLine(int Line);
			void												UpdateColors();
		public:
			virtual ~NumberedRichTextBox();

			RichTextBox^										GetTextField() { return TextField; }
			SplitContainer^										GetContainer() { return Splitter; }

			NumberedRichTextBox(UInt32 LinesToScroll, Font^ Font, Color ForegroundColor, Color BackgroundColor, Color HighlightColor);

			virtual void										JumpToLine(String^ Line);
			virtual void										SetFont(Font^ NewFont);
			virtual void										SetForegroundColor(Color Foreground);
			virtual void										SetBackgroundColor(Color Background);

			UInt32												GetLineCount(void) { return TextField->Lines->Length; }
		};

		ref class OffsetRichTextBox : public NumberedRichTextBox
		{
		protected:
			virtual void										LineField_MouseDown(Object^ Sender, MouseEventArgs^ E) override;

			List<UInt16>^										LineOffsets;

			virtual int											SelectLineNumberInLineField(UInt32 Line) override;
			virtual void										HighlightLineNumbers(void) override;
			virtual void										DrawLineNumbers(void) override;

			int													GetIndexOfOffset(UInt16 Offset);
			void												ClearOffsets() { return LineOffsets->Clear(); }
			UInt16												LookupOffsetByIndex(UInt32 Index);
			void												AddOffsetToList(UInt16 Offset) { LineOffsets->Add(Offset); }
			UInt32												GetOffsetCount() { return LineOffsets->Count; }
		public:
			virtual ~OffsetRichTextBox();

			property bool										OffsetFlag;		// line numbering will be rendered in offsets when set to true

			bool												CalculateLineOffsetsForTextField(void* Data, UInt32 Length);
			virtual void										JumpToLine(String^ Line) override;
			UInt16												GetLastOffset(void);

			OffsetRichTextBox(UInt32 LinesToScroll, Font^ Font, Color ForegroundColor, Color BackgroundColor, Color HighlightColor);
		};
	}
}