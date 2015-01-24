#pragma once

// BGSEERenderWindowPainter - Render window text painter

namespace BGSEditorExtender
{
	class BGSEERenderWindowPainter;
	class BGSEERenderWindowPainterOperator;
	class BGSEEDynamicRenderChannel;

	class BGSEERenderChannelBase
	{
		friend class					BGSEERenderWindowPainter;

		virtual void					Render(LPD3DXSPRITE RenderToSprite) = 0;
	protected:
		struct Parameters
		{
			INT								FontHeight;
			INT								FontWidth;
			UINT							FontWeight;
			char							FontFace[0x100];
			D3DCOLOR						Color;
			RECT							DrawArea;
			UInt32							DrawAreaFlags;
			DWORD							DrawFormat;

			Parameters(INT FontHeight,
				INT FontWidth,
				UINT FontWeight,
				const char* FontFace,
				D3DCOLOR Color,
				RECT* DrawArea,
				DWORD DrawFormat,
				UInt32 DrawAreaFlags);
		};

		LPD3DXFONT						Font;
		RECT							RenderArea;
		Parameters						InputParams;
		bool							Valid;

		bool							CreateD3D(LPDIRECT3DDEVICE9 Device, HWND Window);
		void							ReleaseD3D();
		bool							GetIsValid() const;
	public:
		enum
		{
			kDrawAreaFlags_Default = 0,
			kDrawAreaFlags_RightAligned = 1 << 0,
			kDrawAreaFlags_BottomAligned = 1 << 1,
		};

		BGSEERenderChannelBase(INT FontHeight,
							   INT FontWidth,
							   UINT FontWeight,
							   const char* FontFace,
							   D3DCOLOR Color,
							   RECT* DrawArea,
							   DWORD DrawFormat,
							   UInt32 DrawAreaFlags = kDrawAreaFlags_Default);

		virtual ~BGSEERenderChannelBase() = 0;
	};

	class BGSEEStaticRenderChannel : public BGSEERenderChannelBase
	{
		friend class					BGSEERenderWindowPainter;

		std::string						LastRenderedText;

		virtual void					Render(LPD3DXSPRITE RenderToSprite);
	protected:
		virtual bool					DrawText(std::string& OutText) = 0;				// return false to skip rendering
	public:
		BGSEEStaticRenderChannel(INT FontHeight,
			INT FontWidth,
			UINT FontWeight,
			const char* FontFace,
			D3DCOLOR Color,
			RECT* DrawArea,
			DWORD DrawFormat,
			UInt32 DrawAreaFlags = kDrawAreaFlags_Default);

		virtual ~BGSEEStaticRenderChannel();
	};

	class BGSEEDynamicRenderChannelScheduler
	{
		static const UInt32				kTimerPeriod = 50;		// in ms

		static VOID CALLBACK			UpdateTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

		typedef std::vector<BGSEEDynamicRenderChannel*>		UpdateCallbackListT;

		UpdateCallbackListT				TaskRegistry;
		UINT_PTR						TimerID;

		void							UpdateTasks(DWORD CurrentTickCount);
	public:
		BGSEEDynamicRenderChannelScheduler();
		~BGSEEDynamicRenderChannelScheduler();

		bool							Register(BGSEEDynamicRenderChannel* Channel);
		void							Unregister(BGSEEDynamicRenderChannel* Channel);

		static BGSEEDynamicRenderChannelScheduler*				GetSingleton(void);
	};

	class BGSEEDynamicRenderChannel : public BGSEERenderChannelBase
	{
		friend class					BGSEERenderWindowPainter;
		friend class					BGSEEDynamicRenderChannelScheduler;

		virtual void					Render(LPD3DXSPRITE RenderToSprite);

		struct Task
		{
			std::string					Drawable;
			DWORD						Duration;			// in ms
			DWORD						StartTickCount;

			Task(const char* Text, DWORD Duration);
		};

		typedef std::queue<Task>		TaskQueueT;

		TaskQueueT						QueuedItems;

		void							UpdateTasks(DWORD CurrentTickCount);
	public:
		BGSEEDynamicRenderChannel(INT FontHeight,
			INT FontWidth,
			UINT FontWeight,
			const char* FontFace,
			D3DCOLOR Color,
			RECT* DrawArea,
			DWORD DrawFormat,
			UInt32 DrawAreaFlags = kDrawAreaFlags_Default);

		virtual ~BGSEEDynamicRenderChannel();

		bool							Queue(float DurationInSeconds, const char* Format, ...);		// duration must be b'ween 1 and 10 seconds
	};

	class BGSEERenderWindowPainterOperator
	{
	public:
		virtual ~BGSEERenderWindowPainterOperator() = 0
		{
			;//
		}

		virtual LPDIRECT3DDEVICE9				GetD3DDevice(void) = 0;
		virtual HWND							GetD3DWindow(void) = 0;
		virtual void							RedrawRenderWindow(void) = 0;
	};

	class BGSEERenderWindowPainter
	{
		static BGSEERenderWindowPainter*		Singleton;

		BGSEERenderWindowPainter();

		typedef std::list<BGSEERenderChannelBase*>	RenderChannelListT;

		RenderChannelListT						RegisteredChannels;
		BGSEERenderWindowPainterOperator*		Operator;
		LPD3DXSPRITE							OutputSprite;
		bool									Enabled;
		bool									Initialized;

		bool									CreateD3D(void);
		void									ReleaseD3D(void);
	public:
		~BGSEERenderWindowPainter();

		static BGSEERenderWindowPainter*		GetSingleton(void);

		bool									Initialize(BGSEERenderWindowPainterOperator* Operator);		// takes ownership of pointer

		bool									RegisterRenderChannel(BGSEERenderChannelBase* Channel);		// caller retains ownership of pointer; returns true if successful
		void									UnregisterRenderChannel(BGSEERenderChannelBase* Channel);

		void									Render(void);
		bool									HandleReset(bool Release, bool Renew);
		void									Redraw(void) const;											// force a redraw of current render window scene

		void									SetEnabled(bool State);
		bool									GetEnabled(void) const;
	};

#define BGSEERWPAINTER							BGSEditorExtender::BGSEERenderWindowPainter::GetSingleton()
}