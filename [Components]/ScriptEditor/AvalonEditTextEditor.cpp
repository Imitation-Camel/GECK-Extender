#include "AvalonEditTextEditor.h"
#include "ScriptParser.h"
#include "Globals.h"
#include "ScriptEditorPreferences.h"

using namespace IntelliSense;
using namespace ICSharpCode::AvalonEdit::Rendering;
using namespace ICSharpCode::AvalonEdit::Document;
using namespace ICSharpCode::AvalonEdit::Editing;

#pragma region Interface Methods
void AvalonEditTextEditor::SetFont(Font^ FontObject)
{
	TextField->FontFamily = gcnew Windows::Media::FontFamily(FontObject->FontFamily->Name);
	TextField->FontSize = FontObject->Size + 5;
	if (FontObject->Style == Drawing::FontStyle::Bold)
		TextField->FontWeight = Windows::FontWeights::Bold;
}

void AvalonEditTextEditor::SetTabCharacterSize(int PixelWidth)
{
	TextField->Options->IndentationSize = PixelWidth;
}

void AvalonEditTextEditor::SetContextMenu(ContextMenuStrip^% Strip)
{
	Container->ContextMenuStrip = Strip;
}

void AvalonEditTextEditor::AddControl(Control^ ControlObject)
{
	Container->Controls->Add(ControlObject);
}

String^ AvalonEditTextEditor::GetText(void)
{
	return TextField->Text;
}

UInt32 AvalonEditTextEditor::GetTextLength(void)
{
	return TextField->Text->Length;
}

void AvalonEditTextEditor::SetText(String^ Text, bool PreventTextChangedEventHandling)
{
	if (PreventTextChangedEventHandling)
		SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);

	if (SetTextAnimating)
	{
		TextField->Text = Text;
		UpdateCodeFoldings();
	}
	else
	{
		SetTextAnimating = true;

 		TextFieldPanel->Children->Add(AnimationPrimitive);

		AnimationPrimitive->Fill =  gcnew System::Windows::Media::VisualBrush(TextField);
		AnimationPrimitive->Height = TextField->ActualHeight;
		AnimationPrimitive->Width = TextField->ActualWidth;

 		TextFieldPanel->Children->Remove(TextField);

		System::Windows::Media::Animation::DoubleAnimation^ FadeOutAnimation = gcnew System::Windows::Media::Animation::DoubleAnimation(1.0,
																						0.0,
																						System::Windows::Duration(System::TimeSpan::FromSeconds(SetTextFadeAnimationDuration)),
																						System::Windows::Media::Animation::FillBehavior::Stop);
		FadeOutAnimation->Completed += gcnew EventHandler(this, &AvalonEditTextEditor::SetTextAnimation_Completed);

		System::Windows::Media::Animation::Storyboard^ FadeOutStoryBoard = gcnew System::Windows::Media::Animation::Storyboard();
		FadeOutStoryBoard->Children->Add(FadeOutAnimation);
		FadeOutStoryBoard->SetTargetName(FadeOutAnimation, AnimationPrimitive->Name);
		FadeOutStoryBoard->SetTargetProperty(FadeOutAnimation, gcnew System::Windows::PropertyPath(AnimationPrimitive->OpacityProperty));
		FadeOutStoryBoard->Begin(TextFieldPanel);

		TextField->Text = Text;
		UpdateCodeFoldings();
	}
}

void AvalonEditTextEditor::InsertText( String^ Text, int Index )
{
	if (Index > GetTextLength())
		Index = GetTextLength();

	TextField->Document->Insert(Index, Text);
}

String^ AvalonEditTextEditor::GetSelectedText(void)
{
	return TextField->SelectedText;
}

void AvalonEditTextEditor::SetSelectedText(String^ Text, bool PreventTextChangedEventHandling)
{
	if (PreventTextChangedEventHandling)
		SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);

	TextField->SelectedText = Text;
}

void AvalonEditTextEditor::SetSelectionStart(int Index)
{
	TextField->SelectionStart = Index;
}

void AvalonEditTextEditor::SetSelectionLength(int Length)
{
	TextField->SelectionLength = Length;
}

int AvalonEditTextEditor::GetCharIndexFromPosition(Point Position)
{
	Nullable<AvalonEdit::TextViewPosition> TextPos = TextField->TextArea->TextView->GetPosition(Windows::Point(Position.X, Position.Y));
	if (TextPos.HasValue)
		return TextField->Document->GetOffset(TextPos.Value.Line, TextPos.Value.Column);
	else
		return GetTextLength() + 1;
}

Point AvalonEditTextEditor::GetPositionFromCharIndex(int Index)
{
	AvalonEdit::Document::TextLocation Location = TextField->Document->GetLocation(Index);
	Windows::Point Result = TextField->TextArea->TextView->GetVisualPosition(AvalonEdit::TextViewPosition(Location), AvalonEdit::Rendering::VisualYPosition::TextTop) - TextField->TextArea->TextView->ScrollOffset;
	return Point(Result.X, Result.Y);
}

int AvalonEditTextEditor::GetLineNumberFromCharIndex(int Index)
{
	if (Index >= TextField->Text->Length)
		Index = TextField->Text->Length - 1;
	return TextField->Document->GetLocation(Index).Line - 1;
}

bool AvalonEditTextEditor::GetCharIndexInsideCommentSegment(int Index)
{
	bool Result = true;

	if (Index < TextField->Text->Length)
	{
		AvalonEdit::Document::DocumentLine^ Line = TextField->Document->GetLineByOffset(Index);
		ScriptParser^ LocalParser = gcnew ScriptParser();
		LocalParser->Tokenize(TextField->Document->GetText(Line), false);
		if (LocalParser->GetCommentTokenIndex(LocalParser->GetTokenIndex(GetTextAtLocation(Index, false))) == -1)
			Result = false;
	}

	return Result;
}

int AvalonEditTextEditor::GetCurrentLineNumber(void)
{
	return TextField->TextArea->Caret->Line - 1;
}

String^ AvalonEditTextEditor::GetTokenAtCaretPos()
{
	return GetTextAtLocation(GetCaretPos() - 1, false)->Replace("\n", "");
}

void AvalonEditTextEditor::SetTokenAtCaretPos(String^ Replacement)
{
	GetTextAtLocation(GetCaretPos() - 1, true);
	TextField->SelectedText	= Replacement;
	SetCaretPos(TextField->SelectionStart + TextField->SelectionLength);
}

String^ AvalonEditTextEditor::GetTokenAtMouseLocation()
{
	return GetTextAtLocation(LastKnownMouseClickLocation, false)->Replace("\n", "");
}

array<String^>^ AvalonEditTextEditor::GetTokensAtMouseLocation()
{
	return GetTextAtLocation(GetCharIndexFromPosition(LastKnownMouseClickLocation));
}

int AvalonEditTextEditor::GetCaretPos()
{
	return TextField->TextArea->Caret->Offset;
}

void AvalonEditTextEditor::SetCaretPos(int Index)
{
	TextField->SelectionLength = 0;

	if (Index > -1)
		TextField->TextArea->Caret->Offset = Index;
	else
		TextField->TextArea->Caret->Offset = 0;

	ScrollToCaret();
}

void AvalonEditTextEditor::ScrollToCaret()
{
	TextField->TextArea->Caret->BringCaretToView();
}

IntPtr AvalonEditTextEditor::GetHandle()
{
	return Container->Handle;
}

void AvalonEditTextEditor::FocusTextArea()
{
	TextField->Focus();
}

void AvalonEditTextEditor::LoadFileFromDisk(String^ Path, UInt32 AllocatedIndex)
{
	try
	{
		SetPreventTextChangedFlag(PreventTextChangeFlagState::e_ManualReset);
		TextField->Load(Path);
		SetPreventTextChangedFlag(PreventTextChangeFlagState::e_Disabled);
		DebugPrint("Loaded text from " + Path + " to editor " + AllocatedIndex);
	}
	catch (Exception^ E)
	{
		DebugPrint("Error encountered when opening file for read operation!\n\tError Message: " + E->Message);
	}
}

void AvalonEditTextEditor::SaveScriptToDisk(String^ Path, bool PathIncludesFileName, String^% DefaultName, UInt32 AllocatedIndex)
{
	if (PathIncludesFileName == false)
		Path += "\\" + DefaultName + ".txt";

	try
	{
		TextField->Save(Path);
		DebugPrint("Dumped editor " + AllocatedIndex + "'s text to " + Path);
	}
	catch (Exception^ E)
	{
		DebugPrint("Error encountered when opening file for write operation!\n\tError Message: " + E->Message);
	}
}

bool AvalonEditTextEditor::GetModifiedStatus()
{
	return ModifiedFlag;
}

void AvalonEditTextEditor::SetModifiedStatus(bool Modified)
{
	ModifiedFlag = Modified;

	switch (Modified)
	{
	case true:
		ClearFindResultIndicators();
		ErrorColorizer->ClearLines();
		break;
	case false:
		break;
	}

	OnScriptModified(gcnew ScriptModifiedEventArgs(Modified));
}

bool AvalonEditTextEditor::GetInitializingStatus()
{
	return InitializingFlag;
}

void AvalonEditTextEditor::SetInitializingStatus(bool Initializing)
{
	InitializingFlag = Initializing;
}

Point AvalonEditTextEditor::GetLastKnownMouseClickLocation()
{
	return LastKnownMouseClickLocation;
}

UInt32 AvalonEditTextEditor::FindReplace(IScriptTextEditor::FindReplaceOperation Operation, String^ Query, String^ Replacement, IScriptTextEditor::FindReplaceOutput^ Output)
{
	UInt32 Hits = 0;
	ClearFindResultIndicators();

	AvalonEdit::Editing::Selection^ TextSelection = TextField->TextArea->Selection;
	if (TextSelection->IsEmpty)
	{
		for each (DocumentLine^ Line in TextField->Document->Lines)
			Hits += PerformReplaceOnSegment(Operation, Line, Query, Replacement, Output);
	}
	else
	{
		AvalonEdit::Document::DocumentLine ^FirstLine = nullptr, ^LastLine = nullptr;

		for each (AvalonEdit::Document::ISegment^ Itr in TextSelection->Segments)
		{
			FirstLine = TextField->TextArea->Document->GetLineByOffset(Itr->Offset);
			LastLine = TextField->TextArea->Document->GetLineByOffset(Itr->EndOffset);

			for (AvalonEdit::Document::DocumentLine^ Itr = FirstLine; Itr != LastLine->NextLine && Itr != nullptr; Itr = Itr->NextLine)
				Hits += PerformReplaceOnSegment(Operation, Itr, Query, Replacement, Output);
		}
	}

	if (Operation == IScriptTextEditor::FindReplaceOperation::e_Replace)
		FindReplaceColorizer->SetMatch(Replacement);
	else
		FindReplaceColorizer->SetMatch(Query);

	SetSelectionLength(0);

	RefreshUI();

	return Hits;
}

void AvalonEditTextEditor::ToggleComment(int StartIndex)
{
	SetPreventTextChangedFlag(PreventTextChangeFlagState::e_ManualReset);

	AvalonEdit::Editing::Selection^ TextSelection = TextField->TextArea->Selection;
	if (TextSelection->IsEmpty)
	{
		AvalonEdit::Document::DocumentLine^ Line = TextField->TextArea->Document->GetLineByOffset(StartIndex);
		if (Line != nullptr)
		{
			int FirstOffset = -1;
			for (int i = Line->Offset; i <= Line->EndOffset; i++)
			{
				char FirstChar = TextField->TextArea->Document->GetCharAt(i);
				if (AvalonEdit::Document::TextUtilities::GetCharacterClass(FirstChar) != AvalonEdit::Document::CharacterClass::Whitespace &&
					AvalonEdit::Document::TextUtilities::GetCharacterClass(FirstChar) != AvalonEdit::Document::CharacterClass::LineTerminator)
				{
					FirstOffset = i;
					break;
				}
			}

			if (FirstOffset != -1)
			{
				char FirstChar = TextField->TextArea->Document->GetCharAt(FirstOffset);
				if (FirstChar == ';')
					TextField->TextArea->Document->Replace(FirstOffset, 1, "");
				else
					TextField->TextArea->Document->Insert(FirstOffset, ";");
			}
		}
	}
	else
	{
		int Count = 0, ToggleType = 9;								// 0 - off, 1 - on
		AvalonEdit::Document::DocumentLine ^FirstLine = nullptr, ^LastLine = nullptr;

		for each (AvalonEdit::Document::ISegment^ Itr in TextSelection->Segments)
		{
			FirstLine = TextField->TextArea->Document->GetLineByOffset(Itr->Offset);
			LastLine = TextField->TextArea->Document->GetLineByOffset(Itr->EndOffset);

			for (AvalonEdit::Document::DocumentLine^ Itr = FirstLine; Itr != LastLine->NextLine && Itr != nullptr; Itr = Itr->NextLine)
			{
				int FirstOffset = -1;
				for (int i = Itr->Offset; i < TextField->Text->Length && i <= Itr->EndOffset; i++)
				{
					char FirstChar = TextField->TextArea->Document->GetCharAt(i);
					if (AvalonEdit::Document::TextUtilities::GetCharacterClass(FirstChar) != AvalonEdit::Document::CharacterClass::Whitespace &&
						AvalonEdit::Document::TextUtilities::GetCharacterClass(FirstChar) != AvalonEdit::Document::CharacterClass::LineTerminator)
					{
						FirstOffset = i;
						break;
					}
				}

				if (FirstOffset != -1)
				{
					char FirstChar = TextField->TextArea->Document->GetCharAt(FirstOffset);
					if (FirstChar == ';' && (!Count || !ToggleType))
					{
						if (!Count)		ToggleType = 0;

						AvalonEdit::Document::DocumentLine^ Line = TextField->TextArea->Document->GetLineByOffset(FirstOffset);
						TextField->TextArea->Document->Replace(Line->Offset, 1, "");
					}
					else if (FirstChar != ';' && (!Count || ToggleType))
					{
						if (!Count)		ToggleType = 1;

						AvalonEdit::Document::DocumentLine^ Line = TextField->TextArea->Document->GetLineByOffset(FirstOffset);
						TextField->TextArea->Document->Insert(Line->Offset, ";");
					}
				}

				Count++;
			}
		}
	}

	SetPreventTextChangedFlag(PreventTextChangeFlagState::e_Disabled);
}

void AvalonEditTextEditor::UpdateIntelliSenseLocalDatabase(void)
{
	IntelliSenseBox->UpdateLocalVariableDatabase();
}

void AvalonEditTextEditor::ClearFindResultIndicators()
{
	FindReplaceColorizer->SetMatch("");
}

void AvalonEditTextEditor::ScrollToLine(String^ LineNumber)
{
	int LineNo = 0;
	try { LineNo = Int32::Parse(LineNumber); } catch (...) { return; }

	GotoLine(LineNo);
}

void AvalonEditTextEditor::OnGotFocus(void)
{
	FocusTextArea();

	IsFocused = true;
	FoldingTimer->Start();
	ScrollBarSyncTimer->Start();
}

void AvalonEditTextEditor::HighlightScriptError(int Line)
{
	ErrorColorizer->AddLine(Line);
	RefreshUI();
}

void AvalonEditTextEditor::OnLostFocus( void )
{
	IsFocused = false;
	FoldingTimer->Stop();
	ScrollBarSyncTimer->Stop();
}

void AvalonEditTextEditor::ClearScriptErrorHighlights(void)
{
	ErrorColorizer->ClearLines();
	RefreshUI();
}

Point AvalonEditTextEditor::PointToScreen(Point Location)
{
	return Container->PointToScreen(Location);
}

void AvalonEditTextEditor::SetEnabledState(bool State)
{
	WPFHost->Enabled = State;
}

void AvalonEditTextEditor::OnPositionSizeChange(void)
{
	IntelliSenseBox->HideInterface();
}
#pragma endregion

#pragma region Methods
void AvalonEditTextEditor::Destroy()
{
	MiddleMouseScrollTimer->Stop();
	FoldingTimer->Stop();
	ScrollBarSyncTimer->Stop();
	AvalonEdit::Folding::FoldingManager::Uninstall(CodeFoldingManager);

	TextField->TextArea->IndentationStrategy = nullptr;
	TextField->TextArea->TextView->BackgroundRenderers->Clear();
	TextField->SyntaxHighlighting = nullptr;

	WPFHost->Child = nullptr;

	delete TextField;
	delete IntelliSenseBox;
	delete MiddleMouseScrollTimer;
	delete FoldingTimer;
	delete WPFHost;
	delete Container;
}

UInt32 AvalonEditTextEditor::PerformReplaceOnSegment(IScriptTextEditor::FindReplaceOperation Operation, AvalonEdit::Document::DocumentLine^ Line, String^ Query, String^ Replacement, IScriptTextEditor::FindReplaceOutput^ Output)
{
	UInt32 Hits = 0;
	String^ CurrentLine = TextField->Document->GetText(Line);

	int Index = 0, Start = 0;
	while ((Index = CurrentLine->IndexOf(Query, Start, System::StringComparison::CurrentCultureIgnoreCase)) != -1)
	{
		Hits++;
		int EndIndex = Index + Query->Length;

		if (Operation == IScriptTextEditor::FindReplaceOperation::e_Replace)
		{
			TextField->Document->Replace(Line->Offset + Index, Query->Length, Replacement);
		}

		Output(Line->LineNumber.ToString(), TextField->Document->GetText(Line));
		Start = Index + 1;
	}
	return Hits;
}

String^ AvalonEditTextEditor::GetTokenAtIndex(int Index, bool SelectText, int% StartIndexOut, int% EndIndexOut)
{
	String^% Source = TextField->Text;
	int SearchIndex = Source->Length, SubStrStart = 0, SubStrEnd = SearchIndex;
	StartIndexOut = -1; EndIndexOut = -1;

	if (Index < SearchIndex && Index >= 0)
	{
		for (int i = Index; i > 0; i--)
		{
			if (Globals::ScriptTextDelimiters->IndexOf(Source[i]) != -1)
			{
				SubStrStart = i + 1;
				break;
			}
		}

		for (int i = Index; i < SearchIndex; i++)
		{
			if (Globals::ScriptTextDelimiters->IndexOf(Source[i]) != -1)
			{
				SubStrEnd = i;
				break;
			}
		}
	}
	else
		return "";

	if (SubStrStart > SubStrEnd)
		return "";
	else
	{
		if (SelectText)
		{
			TextField->SelectionStart = SubStrStart;
			TextField->SelectionLength = SubStrEnd - SubStrStart;
		}

		StartIndexOut = SubStrStart; EndIndexOut = SubStrEnd;
		return Source->Substring(SubStrStart, SubStrEnd - SubStrStart);
	}
}

String^ AvalonEditTextEditor::GetTextAtLocation(Point Location, bool SelectText)
{
	int Index =	GetCharIndexFromPosition(Location), OffsetA = 0, OffsetB = 0;
	return GetTokenAtIndex(Index, SelectText, OffsetA, OffsetB);
}

String^ AvalonEditTextEditor::GetTextAtLocation(int Index, bool SelectText)
{
	int OffsetA = 0, OffsetB = 0;
	return GetTokenAtIndex(Index, SelectText, OffsetA, OffsetB);
}

array<String^>^ AvalonEditTextEditor::GetTextAtLocation( int Index )
{
	int OffsetA = 0, OffsetB = 0, Throwaway = 0;
	array<String^>^ Result = gcnew array<String^>(3);
	Result[1] = GetTokenAtIndex(Index, false, OffsetA, OffsetB);
	Result[0] = GetTokenAtIndex(OffsetA - 2, false, Throwaway, Throwaway);
	Result[2] = GetTokenAtIndex(OffsetB + 2, false, Throwaway, Throwaway);

	return Result;
}

void AvalonEditTextEditor::GotoLine(int Line)
{
	if (Line > 0 && Line <= TextField->LineCount)
	{
		TextField->TextArea->Caret->Line = Line;
		TextField->TextArea->Caret->Column = 0;
		TextField->Select(TextField->Document->GetLineByNumber(Line)->Offset, TextField->Document->GetLineByNumber(Line)->Length);
		ScrollToCaret();
	}
	else
	{
		MessageBox::Show("Invalid line number/offset", "Goto Line - CSE Script Editor");
	}
}

void AvalonEditTextEditor::HandleTextChangeEvent()
{
	if (InitializingFlag)
	{
		InitializingFlag = false;
		SetModifiedStatus(false);
		ClearFindResultIndicators();
	}
	else
	{
		SetModifiedStatus(true);
		if (PreventTextChangedEventFlag == PreventTextChangeFlagState::e_AutoReset)
			PreventTextChangedEventFlag = PreventTextChangeFlagState::e_Disabled;
		else if (PreventTextChangedEventFlag == PreventTextChangeFlagState::e_Disabled)
		{
			if (TextField->SelectionStart - 1 >= 0 && !GetCharIndexInsideCommentSegment(TextField->SelectionStart - 1))
			{
				if (LastKeyThatWentDown != System::Windows::Input::Key::Back || GetTokenAtCaretPos() != "")
					IntelliSenseBox->ShowInterface(IntelliSenseBox->LastOperation, false, false);
				else
					IntelliSenseBox->HideInterface();
			}
		}
	}
}

void AvalonEditTextEditor::StartMiddleMouseScroll(System::Windows::Input::MouseButtonEventArgs^ E)
{
	IsMiddleMouseScrolling = true;

	ScrollStartPoint = E->GetPosition(TextField);

	TextField->Cursor = (TextField->ExtentWidth > TextField->ViewportWidth) || (TextField->ExtentHeight > TextField->ViewportHeight) ? System::Windows::Input::Cursors::ScrollAll : System::Windows::Input::Cursors::IBeam;
	TextField->CaptureMouse();
	MiddleMouseScrollTimer->Start();
}

void AvalonEditTextEditor::StopMiddleMouseScroll()
{
	TextField->Cursor = System::Windows::Input::Cursors::IBeam;
	TextField->ReleaseMouseCapture();
	MiddleMouseScrollTimer->Stop();
	IsMiddleMouseScrolling = false;
}

void AvalonEditTextEditor::UpdateCodeFoldings()
{
	if (IsFocused && CodeFoldingStrategy != nullptr)
		CodeFoldingStrategy->UpdateFoldings(CodeFoldingManager, TextField->Document);
}

void AvalonEditTextEditor::SynchronizeExternalScrollBars()
{
	SynchronizingExternalScrollBars = true;

	int ScrollBarHeight = TextField->ExtentHeight - TextField->ViewportHeight + 155;
	int ScrollBarWidth = TextField->ExtentWidth - TextField->ViewportWidth + 155;
	int VerticalOffset = TextField->VerticalOffset;
	int HorizontalOffset = TextField->HorizontalOffset;

	if (ScrollBarHeight <= 0)
		ExternalVerticalScrollBar->Enabled = false;
	else if (!ExternalVerticalScrollBar->Enabled)
		ExternalVerticalScrollBar->Enabled = true;

	if (ScrollBarWidth <= 0)
		ExternalHorizontalScrollBar->Enabled = false;
	else if (!ExternalHorizontalScrollBar->Enabled)
		ExternalHorizontalScrollBar->Enabled = true;

	ExternalVerticalScrollBar->Maximum = ScrollBarHeight;
	ExternalVerticalScrollBar->Minimum = 0;
	if (VerticalOffset >= 0 && VerticalOffset <= ScrollBarHeight)
		ExternalVerticalScrollBar->Value = VerticalOffset;

	ExternalHorizontalScrollBar->Maximum = ScrollBarWidth;
	ExternalHorizontalScrollBar->Minimum = 0;
	if (HorizontalOffset >= 0 && HorizontalOffset <= ScrollBarHeight)
		ExternalHorizontalScrollBar->Value = HorizontalOffset;

	SynchronizingExternalScrollBars = false;
}

RTBitmap^ AvalonEditTextEditor::RenderFrameworkElement( System::Windows::FrameworkElement^ Element )
{
	double TopLeft = 0;
	double TopRight = 0;
	int Width = (int)Element->ActualWidth;
	int Height = (int)Element->ActualHeight;
	double DpiX = 96; // this is the magic number
	double DpiY = 96; // this is the magic number

	System::Windows::Media::PixelFormat ReturnFormat = System::Windows::Media::PixelFormats::Default;
	System::Windows::Media::VisualBrush^ ElementBrush = gcnew System::Windows::Media::VisualBrush(Element);
	System::Windows::Media::DrawingVisual^ Visual = gcnew System::Windows::Media::DrawingVisual();

	System::Windows::Media::DrawingContext^ Context = Visual->RenderOpen();
	Context->DrawRectangle(ElementBrush, nullptr, System::Windows::Rect(TopLeft, TopRight, Width, Height));
	Context->Close();

	System::Windows::Media::Imaging::RenderTargetBitmap^ Bitmap = gcnew System::Windows::Media::Imaging::RenderTargetBitmap(Width, Height, DpiX, DpiY, ReturnFormat);
	Bitmap->Render(Visual);

	return Bitmap;
}
#pragma endregion

#pragma region Events
void AvalonEditTextEditor::OnScriptModified(ScriptModifiedEventArgs^ E)
{
	ScriptModified(this, E);
}

void AvalonEditTextEditor::OnKeyDown(KeyEventArgs^ E)
{
	KeyDown(this, E);
}
#pragma endregion

#pragma region Event Handlers
void AvalonEditTextEditor::TextField_TextChanged(Object^ Sender, EventArgs^ E)
{
	HandleTextChangeEvent();
}

void AvalonEditTextEditor::TextField_CaretPositionChanged(Object^ Sender, EventArgs^ E)
{
	static UInt32 LineBuffer = 1;

	if (TextField->TextArea->Caret->Line != LineBuffer)
	{
		IntelliSenseBox->Enabled = true;
		IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;
		LineBuffer = TextField->TextArea->Caret->Line;
		RefreshUI();
	}
}

void AvalonEditTextEditor::TextField_ScrollOffsetChanged(Object^ Sender, EventArgs^ E)
{
	if (SynchronizingInternalScrollBars == false)
		SynchronizeExternalScrollBars();
}

void AvalonEditTextEditor::TextField_KeyDown(Object^ Sender, System::Windows::Input::KeyEventArgs^ E)
{
	LastKeyThatWentDown = E->Key;

	if (IsMiddleMouseScrolling)
	{
		StopMiddleMouseScroll();
	}

	int SelStart = TextField->SelectionStart, SelLength = TextField->SelectionLength;

	if (Globals::GetIsDelimiterKey(E->Key))
	{
		IntelliSenseBox->UpdateLocalVariableDatabase();
		IntelliSenseBox->Enabled = true;

		if (TextField->SelectionStart - 1 >= 0 && !GetCharIndexInsideCommentSegment(TextField->SelectionStart - 1))
		{
			try
			{
				switch (E->Key)
				{
				case System::Windows::Input::Key::OemPeriod:
					{
						IntelliSenseBox->ShowInterface(IntelliSenseInterface::Operation::e_Dot, false, true);
						SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);
						break;
					}
				case System::Windows::Input::Key::Space:
					{
						String^ CommandName = GetTextAtLocation(TextField->SelectionStart - 1, false)->Replace("\n", "");

						if (!String::Compare(CommandName, "call", true))
						{
							IntelliSenseBox->ShowInterface(IntelliSenseInterface::Operation::e_Call, false, true);
							SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);
						}
						else if (!String::Compare(CommandName, "let", true) || !String::Compare(CommandName, "set", true))
						{
							IntelliSenseBox->ShowInterface(IntelliSenseInterface::Operation::e_Assign, false, true);
							SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);
						}
						else
							IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;

						break;
					}
				default:
					{
						IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;
						break;
					}
				}
			}
			catch (Exception^ E)
			{
				DebugPrint("IntelliSense raised an exception while initializing.\n\tException: " + E->Message, true);
			}
		}
	}

	switch (E->Key)
	{
	case System::Windows::Input::Key::Q:
		if (E->KeyboardDevice->Modifiers == System::Windows::Input::ModifierKeys::Control)
		{
			ToggleComment(TextField->SelectionStart);

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Enter:
		if (E->KeyboardDevice->Modifiers == System::Windows::Input::ModifierKeys::Control)
		{
			if (!IntelliSenseBox->Visible)
				IntelliSenseBox->ShowInterface(IntelliSenseInterface::Operation::e_Default, true, false);

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Escape:
		if (IntelliSenseBox->Visible)
		{
			IntelliSenseBox->HideInterface();
			IntelliSenseBox->Enabled = false;
			IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Tab:
		if (IntelliSenseBox->Visible)
		{
			SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);
			IntelliSenseBox->PickSelection();
			FocusTextArea();

			IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Up:
		if (IntelliSenseBox->Visible)
		{
			IntelliSenseBox->ChangeCurrentSelection(IntelliSenseInterface::MoveDirection::e_Up);

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Down:
		if (IntelliSenseBox->Visible)
		{
			IntelliSenseBox->ChangeCurrentSelection(IntelliSenseInterface::MoveDirection::e_Down);

			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	case System::Windows::Input::Key::Z:
	case System::Windows::Input::Key::Y:
		if (E->KeyboardDevice->Modifiers == System::Windows::Input::ModifierKeys::Control)
			SetPreventTextChangedFlag(PreventTextChangeFlagState::e_AutoReset);
		break;
	case System::Windows::Input::Key::PageUp:
	case System::Windows::Input::Key::PageDown:
		if (IntelliSenseBox->Visible)
		{
			HandleKeyEventForKey(E->Key);
			E->Handled = true;
		}
		break;
	}

	Int32 KeyState = System::Windows::Input::KeyInterop::VirtualKeyFromKey(E->Key);
	if ((E->KeyboardDevice->Modifiers & System::Windows::Input::ModifierKeys::Control) == System::Windows::Input::ModifierKeys::Control)
		KeyState |= (int)Keys::Control;
 	if ((E->KeyboardDevice->Modifiers & System::Windows::Input::ModifierKeys::Alt) == System::Windows::Input::ModifierKeys::Alt)
 		KeyState |= (int)Keys::Alt;
 	if ((E->KeyboardDevice->Modifiers & System::Windows::Input::ModifierKeys::Shift) == System::Windows::Input::ModifierKeys::Shift)
 		KeyState |= (int)Keys::Shift;

	KeyEventArgs^ TunneledArgs = gcnew KeyEventArgs((Keys)KeyState);
	OnKeyDown(TunneledArgs);
}

void AvalonEditTextEditor::TextField_KeyUp(Object^ Sender, System::Windows::Input::KeyEventArgs^ E)
{
	if (E->Key == KeyToPreventHandling)
	{
		E->Handled = true;
		KeyToPreventHandling = System::Windows::Input::Key::None;
		return;
	}
}

void AvalonEditTextEditor::TextField_MouseDown(Object^ Sender, System::Windows::Input::MouseButtonEventArgs^ E)
{
	Nullable<AvalonEdit::TextViewPosition> Location = TextField->GetPositionFromPoint(E->GetPosition(TextField));
	if (Location.HasValue)
	{
		int Offset = TextField->Document->GetOffset(Location.Value.Line, Location.Value.Column);
		LastKnownMouseClickLocation = GetPositionFromCharIndex(Offset);
		System::Windows::Point ScrollCorrectedLocation = System::Windows::Point(LastKnownMouseClickLocation.X, LastKnownMouseClickLocation.Y);
		ScrollCorrectedLocation += TextField->TextArea->TextView->ScrollOffset;
		LastKnownMouseClickLocation = Point(ScrollCorrectedLocation.X, ScrollCorrectedLocation.Y);
	}

	if (IntelliSenseBox->Visible)
	{
		IntelliSenseBox->HideInterface();
		IntelliSenseBox->Enabled = false;
		IntelliSenseBox->LastOperation = IntelliSenseInterface::Operation::e_Default;
	}

	IntelliSenseBox->HideInfoToolTip();
}

void AvalonEditTextEditor::TextField_MouseWheel(Object^ Sender, System::Windows::Input::MouseWheelEventArgs^ E)
{
	if (IntelliSenseBox->Visible)
	{
		if (E->Delta < 0)
			IntelliSenseBox->ChangeCurrentSelection(IntelliSenseInterface::MoveDirection::e_Down);
		else
			IntelliSenseBox->ChangeCurrentSelection(IntelliSenseInterface::MoveDirection::e_Up);

		E->Handled = true;
	}
	else
		IntelliSenseBox->HideInfoToolTip();
}

void AvalonEditTextEditor::TextField_MouseHover(Object^ Sender, System::Windows::Input::MouseEventArgs^ E)
{
	Nullable<AvalonEdit::TextViewPosition> ViewLocation = TextField->GetPositionFromPoint(E->GetPosition(TextField));
	if (ViewLocation.HasValue)
	{
		int Offset = TextField->Document->GetOffset(ViewLocation.Value.Line, ViewLocation.Value.Column);
		Point Location = GetPositionFromCharIndex(Offset);

		if (TextField->Text->Length > 0)
		{
			array<String^>^ Tokens = GetTextAtLocation(Offset);
			if (!GetCharIndexInsideCommentSegment(Offset))
				IntelliSenseBox->ShowQuickViewTooltip(Tokens[1], Tokens[0], Location);
		}
	}
}

void AvalonEditTextEditor::TextField_MouseHoverStopped(Object^ Sender, System::Windows::Input::MouseEventArgs^ E)
{
	IntelliSenseBox->HideInfoToolTip();
}

void AvalonEditTextEditor::TextField_SelectionChanged(Object^ Sender, EventArgs^ E)
{
	RefreshUI();
}

void AvalonEditTextEditor::TextField_LostFocus(Object^ Sender, System::Windows::RoutedEventArgs^ E)
{
	;//
}

void AvalonEditTextEditor::TextField_MiddleMouseScrollMove(Object^ Sender, System::Windows::Input::MouseEventArgs^ E)
{
	static double SlowScrollFactor = 20;

	if (TextField->IsMouseCaptured)
	{
		System::Windows::Point CurrentPosition = E->GetPosition(TextField);

		System::Windows::Vector Delta = CurrentPosition - ScrollStartPoint;
		Delta.Y /= SlowScrollFactor;
		Delta.X /= SlowScrollFactor;

		CurrentScrollOffset = Delta;
	}
}

void AvalonEditTextEditor::TextField_MiddleMouseScrollDown(Object^ Sender, System::Windows::Input::MouseButtonEventArgs^ E)
{
	if (!IsMiddleMouseScrolling && E->ChangedButton ==  System::Windows::Input::MouseButton::Middle)
	{
		StartMiddleMouseScroll(E);
	}
	else if (IsMiddleMouseScrolling)
	{
		StopMiddleMouseScroll();
	}
}

void AvalonEditTextEditor::MiddleMouseScrollTimer_Tick(Object^ Sender, EventArgs^ E)
{
	static double AccelerateScrollFactor = 0.0456;

	if (IsMiddleMouseScrolling)
	{
		TextField->ScrollToVerticalOffset(TextField->VerticalOffset + CurrentScrollOffset.Y);
		TextField->ScrollToHorizontalOffset(TextField->HorizontalOffset + CurrentScrollOffset.X);

		CurrentScrollOffset += CurrentScrollOffset * AccelerateScrollFactor;
	}
}

void AvalonEditTextEditor::FoldingTimer_Tick( Object^ Sender, EventArgs^ E )
{
	UpdateCodeFoldings();
}

void AvalonEditTextEditor::ScrollBarSyncTimer_Tick( Object^ Sender, EventArgs^ E )
{
	SynchronizingInternalScrollBars = false;
	SynchronizeExternalScrollBars();
}

void AvalonEditTextEditor::ExternalScrollBar_ValueChanged( Object^ Sender, EventArgs^ E )
{
	if (SynchronizingExternalScrollBars == false)
	{
		SynchronizingInternalScrollBars = true;

		int VerticalOffset = ExternalVerticalScrollBar->Value;
		int HorizontalOffset = ExternalHorizontalScrollBar->Value;

		VScrollBar^ VertSender = dynamic_cast<VScrollBar^>(Sender);
		HScrollBar^ HortSender = dynamic_cast<HScrollBar^>(Sender);

		if (VertSender != nullptr)
		{
			TextField->ScrollToVerticalOffset(VerticalOffset);
		}
		else if (HortSender != nullptr)
		{
			TextField->ScrollToHorizontalOffset(HorizontalOffset);
		}
	}
}

void AvalonEditTextEditor::SetTextAnimation_Completed( Object^ Sender, EventArgs^ E )
{
	TextFieldPanel->Children->Remove(AnimationPrimitive);
	TextFieldPanel->Children->Add(TextField);

	System::Windows::Media::Animation::DoubleAnimation^ FadeInAnimation = gcnew System::Windows::Media::Animation::DoubleAnimation(0.0,
																					1.0,
																					System::Windows::Duration(System::TimeSpan::FromSeconds(SetTextFadeAnimationDuration)),
																					System::Windows::Media::Animation::FillBehavior::Stop);
	System::Windows::Media::Animation::Storyboard^ FadeInStoryBoard = gcnew System::Windows::Media::Animation::Storyboard();
	FadeInStoryBoard->Children->Add(FadeInAnimation);
	FadeInStoryBoard->SetTargetName(FadeInAnimation, TextField->Name);
	FadeInStoryBoard->SetTargetProperty(FadeInAnimation, gcnew System::Windows::PropertyPath(TextField->OpacityProperty));
	FadeInStoryBoard->Begin(TextFieldPanel);

	SetTextAnimating = false;
}
#pragma endregion

AvalonEditTextEditor::AvalonEditTextEditor(Font^ Font, Object^% Parent)
{
	Container = gcnew Panel();
	WPFHost = gcnew ElementHost();
	TextFieldPanel = gcnew System::Windows::Controls::DockPanel();
	TextField = gcnew AvalonEdit::TextEditor();
	AnimationPrimitive = gcnew System::Windows::Shapes::Rectangle();
	ErrorColorizer = gcnew AvalonEditScriptErrorBGColorizer(TextField, KnownLayer::Background);
	FindReplaceColorizer = gcnew AvalonEditFindReplaceBGColorizer(TextField, KnownLayer::Background);
	CodeFoldingManager = AvalonEdit::Folding::FoldingManager::Install(TextField->TextArea);
	CodeFoldingStrategy = nullptr;
	if (OPTIONS->FetchSettingAsInt("CodeFolding"))		CodeFoldingStrategy = gcnew AvalonEditObScriptCodeFoldingStrategy();
	MiddleMouseScrollTimer = gcnew Timer();
	FoldingTimer = gcnew Timer();
	ExternalVerticalScrollBar = gcnew VScrollBar();
	ExternalHorizontalScrollBar = gcnew HScrollBar();
	ScrollBarSyncTimer = gcnew Timer();

	System::Windows::NameScope::SetNameScope(TextFieldPanel, gcnew System::Windows::NameScope());
	TextFieldPanel->Background = gcnew Windows::Media::SolidColorBrush(Windows::Media::Color::FromArgb(255, 255, 255, 255));
	TextFieldPanel->VerticalAlignment = System::Windows::VerticalAlignment::Stretch;

	TextField->Name = "AvalonEditTextEditorInstance";
	TextField->Options->AllowScrollBelowDocument = false;
	TextField->Options->EnableEmailHyperlinks = false;
	TextField->Options->EnableHyperlinks = true;
	TextField->Options->RequireControlModifierForHyperlinkClick = true;
	TextField->Options->CutCopyWholeLine = OPTIONS->FetchSettingAsInt("CutCopyEntireLine");
	TextField->Options->ShowSpaces = OPTIONS->FetchSettingAsInt("ShowSpaces");
	TextField->Options->ShowTabs = OPTIONS->FetchSettingAsInt("ShowTabs");
	TextField->WordWrap = OPTIONS->FetchSettingAsInt("WordWrap");
	TextField->ShowLineNumbers = true;
	TextField->HorizontalScrollBarVisibility = System::Windows::Controls::ScrollBarVisibility::Hidden;
	TextField->VerticalScrollBarVisibility = System::Windows::Controls::ScrollBarVisibility::Hidden;

	InitializeSyntaxHighlightingManager(false);

	TextField->SyntaxHighlighting = AvalonEdit::Highlighting::HighlightingManager::Instance->GetDefinition("ObScript");

	TextField->TextChanged += gcnew EventHandler(this, &AvalonEditTextEditor::TextField_TextChanged);
	TextField->TextArea->Caret->PositionChanged += gcnew EventHandler(this, &AvalonEditTextEditor::TextField_CaretPositionChanged);
	TextField->TextArea->SelectionChanged += gcnew EventHandler(this, &AvalonEditTextEditor::TextField_SelectionChanged);
	TextField->LostFocus += gcnew System::Windows::RoutedEventHandler(this, &AvalonEditTextEditor::TextField_LostFocus);
	TextField->TextArea->TextView->ScrollOffsetChanged += gcnew EventHandler(this, &AvalonEditTextEditor::TextField_ScrollOffsetChanged);

	TextField->PreviewKeyUp += gcnew System::Windows::Input::KeyEventHandler(this, &AvalonEditTextEditor::TextField_KeyUp);
	TextField->PreviewKeyDown += gcnew System::Windows::Input::KeyEventHandler(this, &AvalonEditTextEditor::TextField_KeyDown);
	TextField->PreviewMouseDown += gcnew System::Windows::Input::MouseButtonEventHandler(this, &AvalonEditTextEditor::TextField_MouseDown);
	TextField->PreviewMouseWheel += gcnew System::Windows::Input::MouseWheelEventHandler(this, &AvalonEditTextEditor::TextField_MouseWheel);

	TextField->PreviewMouseHover += gcnew System::Windows::Input::MouseEventHandler(this, &AvalonEditTextEditor::TextField_MouseHover);
	TextField->PreviewMouseHoverStopped += gcnew System::Windows::Input::MouseEventHandler(this, &AvalonEditTextEditor::TextField_MouseHoverStopped);

	TextField->PreviewMouseMove += gcnew System::Windows::Input::MouseEventHandler(this, &AvalonEditTextEditor::TextField_MiddleMouseScrollMove);
	TextField->PreviewMouseDown += gcnew System::Windows::Input::MouseButtonEventHandler(this, &AvalonEditTextEditor::TextField_MiddleMouseScrollDown);

	TextField->TextArea->TextView->ScrollOffsetChanged += gcnew EventHandler(this, &AvalonEditTextEditor::TextField_ScrollOffsetChanged);

	TextField->TextArea->TextView->BackgroundRenderers->Add(ErrorColorizer);
	TextField->TextArea->TextView->BackgroundRenderers->Add(FindReplaceColorizer);
	TextField->TextArea->TextView->BackgroundRenderers->Add(gcnew AvalonEditSelectionBGColorizer(TextField, KnownLayer::Background));
	TextField->TextArea->TextView->BackgroundRenderers->Add(gcnew AvalonEditLineLimitBGColorizer(TextField, KnownLayer::Background));
	TextField->TextArea->TextView->BackgroundRenderers->Add(gcnew AvalonEditCurrentLineBGColorizer(TextField, KnownLayer::Background));

	TextField->TextArea->IndentationStrategy = gcnew AvalonEditObScriptIndentStrategy(true, true);

	AnimationPrimitive->Name = "AnimationPrimitive";

	TextFieldPanel->RegisterName(AnimationPrimitive->Name, AnimationPrimitive);
	TextFieldPanel->RegisterName(TextField->Name, TextField);

	TextFieldPanel->Children->Add(TextField);

	InitializingFlag = false;
	ModifiedFlag = false;
	PreventTextChangedEventFlag = PreventTextChangeFlagState::e_Disabled;
	KeyToPreventHandling = System::Windows::Input::Key::None;
	LastKeyThatWentDown = System::Windows::Input::Key::None;
	IsMiddleMouseScrolling = false;

	MiddleMouseScrollTimer->Interval = 16;
	MiddleMouseScrollTimer->Tick += gcnew EventHandler(this, &AvalonEditTextEditor::MiddleMouseScrollTimer_Tick);

	IsFocused = true;
	FoldingTimer->Interval = 2000;
	FoldingTimer->Tick += gcnew EventHandler(this, &AvalonEditTextEditor::FoldingTimer_Tick);
	FoldingTimer->Start();

	IntelliSenseBox = gcnew IntelliSenseInterface(Parent);
	LastKnownMouseClickLocation = Point(0, 0);

	ScrollBarSyncTimer->Interval = 200;
	ScrollBarSyncTimer->Tick += gcnew EventHandler(this, &AvalonEditTextEditor::ScrollBarSyncTimer_Tick);
	ScrollBarSyncTimer->Start();

	ExternalVerticalScrollBar->Dock = DockStyle::Right;
	ExternalVerticalScrollBar->ValueChanged += gcnew EventHandler(this, &AvalonEditTextEditor::ExternalScrollBar_ValueChanged);
	ExternalVerticalScrollBar->SmallChange = 30;
	ExternalVerticalScrollBar->LargeChange = 150;

	ExternalHorizontalScrollBar->Dock = DockStyle::Bottom;
	ExternalHorizontalScrollBar->ValueChanged += gcnew EventHandler(this, &AvalonEditTextEditor::ExternalScrollBar_ValueChanged);
	ExternalHorizontalScrollBar->SmallChange = 30;
	ExternalHorizontalScrollBar->LargeChange = 155;

	SynchronizingInternalScrollBars = false;
	SynchronizingExternalScrollBars = false;

	SetTextAnimating = false;

	Container->Dock = DockStyle::Fill;
	Container->BorderStyle = BorderStyle::FixedSingle;
	Container->Controls->Add(WPFHost);
 	Container->Controls->Add(ExternalVerticalScrollBar);
 	Container->Controls->Add(ExternalHorizontalScrollBar);

	WPFHost->Dock = DockStyle::Fill;
	WPFHost->Child = TextFieldPanel;

	SetFont(Font);
}

void AvalonEditTextEditor::InitializeSyntaxHighlightingManager(bool Reset)
{
	if (SyntaxHighlightingManager->GetInitialized() && !Reset)
		return;
	else
		SyntaxHighlightingManager->Reset();

	SyntaxHighlightingManager->CreateCommentPreprocessorRuleset(OPTIONS->LookupColorByKey("SyntaxCommentsColor"), Color::GhostWhite, true, OPTIONS->LookupColorByKey("SyntaxPreprocessorColor"));
	SyntaxHighlightingManager->CreateRuleset(AvalonEditXSHDManager::Rulesets::e_Keywords, OPTIONS->LookupColorByKey("SyntaxKeywordsColor"), Color::GhostWhite, true);
	SyntaxHighlightingManager->CreateRuleset(AvalonEditXSHDManager::Rulesets::e_BlockTypes, OPTIONS->LookupColorByKey("SyntaxScriptBlocksColor"), Color::GhostWhite, true);
	SyntaxHighlightingManager->CreateRuleset(AvalonEditXSHDManager::Rulesets::e_Delimiter, OPTIONS->LookupColorByKey("SyntaxDelimitersColor"), Color::GhostWhite, true);
	SyntaxHighlightingManager->CreateRuleset(AvalonEditXSHDManager::Rulesets::e_Digit, OPTIONS->LookupColorByKey("SyntaxDigitsColor"), Color::GhostWhite, true);
	SyntaxHighlightingManager->CreateRuleset(AvalonEditXSHDManager::Rulesets::e_String, OPTIONS->LookupColorByKey("SyntaxStringsColor"), Color::GhostWhite, true);

	SyntaxHighlightingManager->RegisterDefinitions("ObScript");

	DebugPrint("Initialized syntax highlighting definitions");
}