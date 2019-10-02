#pragma once
#include "Common.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

//#define FRAME_HANDLE(call) do { LRESULT r = call; if (r > 0) return r; } while (0); break
#define FRAME_HANDLE(call) do { call; if (msgResultPresent) return msgResultPresent = false, msgResult; } while (0); break

class Frame
{
protected:
	WNDPROC callback;
	HWND handle;
	MSG msg;
	LRESULT msgResult;
	bool msgResultPresent;

public:
	Frame () :
		callback(DefWindowProc), handle(0),
		msgResult(0), msgResultPresent(false)
	{
	}

	operator HWND () { return handle; }

public:
	static bool OutputLastErrorMessage ()
	{
		LPTSTR lpMsgBuf;
		FormatMessage (
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		OutputDebugString (lpMsgBuf);
		LocalFree (lpMsgBuf);
		return true;
	}

	static HINSTANCE GetInstance (HWND hwnd)
	{
		return (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE);
	}

	static HGDIOBJ SelectStockObject (HDC hdc, int id)
	{
		return SelectObject (hdc, GetStockObject (id));
	}

	static void CreateDefaultWindowClass (WNDCLASSEX & wc)
	{
		wc.cbSize = sizeof (WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = 0;
		wc.hIcon = LoadIcon (0, IDI_APPLICATION);
		wc.hCursor = LoadCursor (0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
		wc.lpszMenuName = 0;
		wc.lpszClassName = 0; // Add some random string.
		wc.hIconSm = wc.hIcon;
	}

	static void CreateDefaultCreateStruct (CREATESTRUCT & cs)
	{
		cs.lpCreateParams = 0;
		cs.hInstance = 0;
		cs.hMenu = 0;
		cs.hwndParent = 0;
		cs.cx = CW_USEDEFAULT;
		cs.cy = CW_USEDEFAULT;
		cs.x = CW_USEDEFAULT;
		cs.y = CW_USEDEFAULT;
		cs.style = 0;
		cs.lpszName = 0;
		cs.lpszClass = 0;
		cs.dwExStyle = 0;
	}

protected:
	static LRESULT CALLBACK Main (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		Frame * frame = (Frame *) GetWindowLong (h, GWL_USERDATA);
		if (!frame) return DefWindowProc (h,m,w,l);
		return frame->Callback (h,m,w,l);
	}

public:
	void Subclass (WNDPROC callback)
	{
		//this->callback = (WNDPROC) SetWindowLong (handle, GWL_WNDPROC, (LONG) callback);
		WNDPROC old = (WNDPROC) SetWindowLong (handle, GWL_WNDPROC, (LONG) callback);
		if (DefWindowProc == this->callback) this->callback = old;
		SendMessage (handle, WM_CREATE, (LONG) this, 0);
	}

	LRESULT Dispatch (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		return CallWindowProcA (callback,h,m,w,l);
	}

	LRESULT DispatchDefault ()
	{
		//return CallWindowProcA (callback,handle,msg.message,msg.wParam,msg.lParam);
		return DefWindowProc (handle,msg.message,msg.wParam,msg.lParam);
	}

	LRESULT Default ()
	{
		return DefWindowProc (handle,msg.message,msg.wParam,msg.lParam);
	}

	bool Create (CREATESTRUCT * cs,WNDCLASSEX * wc = 0)
	{
		if (!cs) return false;
		if (wc)
		{
			OnPreRegisterClass (*wc);
			if (!RegisterClassEx (wc)) return false;
		}
		OnPreCreate (*cs);
		handle = CreateWindowEx (cs->dwExStyle, cs->lpszClass, cs->lpszName, cs->style,
			cs->x, cs->y, cs->cx, cs->cy, cs->hwndParent, cs->hMenu, cs->hInstance, cs->lpCreateParams);
		if (!handle) return false;
		SetWindowLong (handle, GWL_USERDATA, (LONG) this);
		Subclass (Main);
		return true;
	}

	bool Create (HINSTANCE instance, LPCSTR title = 0, DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE, DWORD exstyle = 0)
	{
		WNDCLASSEX wc;
		CreateDefaultWindowClass (wc);
		wc.hInstance = instance;
		wc.lpszClassName = "Frame";
		
		CREATESTRUCT cs;		
		CreateDefaultCreateStruct (cs);
		cs.hInstance = wc.hInstance;
		cs.lpszClass = wc.lpszClassName;
		cs.lpszName = title;
		cs.style = style;
		cs.dwExStyle = exstyle;

		return Create (&cs, &wc);
	}

	bool Create (HWND parent, LPCSTR type, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0, bool registered = true)
	{
		WNDCLASSEX wc;
		CreateDefaultWindowClass (wc);

		if (!registered)
		{
			CreateDefaultWindowClass (wc);
			wc.hInstance = GetInstance (parent);
			wc.lpszClassName = type;
		}

		CREATESTRUCT cs;
		CreateDefaultCreateStruct (cs);
		cs.hInstance = Frame::GetInstance (parent);
		cs.hMenu = (HMENU) id;
		cs.hwndParent = parent;
		cs.lpszClass = type;
		cs.style = WS_CHILD | style;
		cs.dwExStyle = exstyle;

		return registered ? Create (&cs) : Create (&cs, &wc);
	}

	virtual LRESULT Run ()
	{
		for (MSG msg; true; ) switch (GetMessage (&msg, 0, 0, 0))
		{
		case 0: return msg.wParam;
		case -1: Sleep (10); break;
		default:
			OnTranslateAccelerator (&msg);
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			break;
		}
		return 0;
	}

	bool Attach (HWND handle)
	{
		this->handle = handle;
		return true;
	}

	HWND GetHandle ()
	{
		return handle;
	}

	Frame GetParentFrame () const
	{
		Frame frame;
		frame.Attach (GetParent (handle));
		return frame;
	}

	LRESULT Message (UINT m, WPARAM w = 0, LPARAM l = 0, bool force = true)
	{
		WNDPROC proc = force ? SendMessage : (WNDPROC) PostMessage;
		return proc (handle, m, w, l);
	}

	bool SetLocation (int x, int y)
	{
		return TRUE == SetWindowPos (handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	bool SetSize (int cx, int cy)
	{
		return TRUE == SetWindowPos (handle, 0, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
	}

	void GetWindowSize (int * width, int * height) const
	{
		RECT r; GetWindowRect (handle, &r);
		if (width) *width = r.right-r.left;
		if (height) *height = r.bottom-r.top;
	}

	void GetClientSize (int * width, int * height) const
	{
		RECT r; GetClientRect (handle, &r);
		if (width) *width = r.right-r.left;
		if (height) *height = r.bottom-r.top;
	}

	int GetWindowWidth () const
	{
		RECT r; GetWindowRect (handle, &r); return r.right-r.left;
	}

	int GetWindowHeight () const
	{
		RECT r; GetWindowRect (handle, &r); return r.bottom-r.top;
	}

	int GetClientWidth () const
	{
		RECT r; GetClientRect (handle, &r); return r.right-r.left;
	}

	int GetClientHeight () const
	{
		RECT r; GetClientRect (handle, &r); return r.bottom-r.top;
	}

	bool InflateBounds (int side, int value)
	{
		RECT r; GetWindowRect (handle, &r);
		if (side & 0x8) r.left -= value;
		if (side & 0x4) r.top -= value;
		if (side & 0x2) r.bottom += value;
		if (side & 0x1) r.right += value;
		SetWindowPos (handle, 0, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOZORDER);
		return true;
	}

	bool Move (int x, int y, int cx, int cy, bool repaint = true)
	{
		return TRUE == MoveWindow (handle, x, y, cx, cy, repaint ? TRUE : FALSE);
	}

	bool Show (int cmd = SW_SHOW)
	{
		return TRUE == ShowWindow (handle, cmd);
	}

	bool Update ()
	{
		return TRUE == UpdateWindow (handle);
	}

	bool Invalidate (LPRECT rect = 0, bool erase = false)
	{
		return TRUE == InvalidateRect (handle, rect, erase ? TRUE : FALSE);
	}

	bool CenterToScreen ()
	{
		int cx = GetSystemMetrics (SM_CXSCREEN);
		int cy = GetSystemMetrics (SM_CYSCREEN);
		RECT r;
		GetWindowRect (handle, &r);
		r.left = (cx - (r.right - r.left)) >> 1;
		r.top = (cy - (r.bottom - r.top)) >> 1;
		return SetLocation (r.left, r.top);
	}

	bool SetTitle (LPCSTR text)
	{
		return TRUE == SetWindowText (handle, text);
	}

	bool SetText (LPCSTR text)
	{
		return TRUE == SetWindowText (handle, text);
	}

	bool SetFormattedText (LPCSTR format, ...)
	{
		va_list list;
		char buffer [1024];
		va_start (list, format);
		_vsnprintf (buffer, sizeof (buffer)-1, format, list);
		buffer [sizeof (buffer)-1] = 0;
		va_end (list);
		return TRUE == SetWindowText (handle, buffer);
	}

	bool HasMenu () const
	{
		return 0 != GetMenu (handle);
	}

	DWORD GetStyle () const
	{
		return GetWindowLong (handle, GWL_STYLE);
	}

	DWORD GetStyleEx () const
	{
		return GetWindowLong (handle, GWL_EXSTYLE);
	}
	
	void FitToSize (int cx, int cy)
	{
		RECT r;
		SetRect (&r, 0, 0, cx, cy);
		AdjustWindowRectEx (&r, GetStyle (), HasMenu () ? TRUE : FALSE, GetStyleEx ());
		SetSize (r.right-r.left, r.bottom-r.top);
	}

	bool SetEnabled (bool enabled)
	{
		return TRUE == EnableWindow (handle, enabled ? TRUE : FALSE);
	}
	
protected:
	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		msg.message = m;
		msg.wParam = w;
		msg.lParam = l;

		msgResultPresent = false;

		switch (m)
		{
			//case WM_MOUSEMOVE: if (GetFocus () != h) SetFocus (h); break;

			case WM_CREATE: OnCreate (handle); return 0;
			case WM_DESTROY: PostQuitMessage (0); break;
			case WM_CLOSE: DestroyWindow (h); break;
			case WM_SIZE: FRAME_HANDLE (OnSize (w, LOWORD(l), HIWORD(l)));
			case WM_SIZING: FRAME_HANDLE (OnSizing (w, (LPRECT) l));
			case WM_KEYDOWN: FRAME_HANDLE (OnKeyDown ((int) w, l));
			case WM_ERASEBKGND: FRAME_HANDLE (OnEraseBkgnd ((HDC) w));
			case WM_PAINT: FRAME_HANDLE (OnPaint ((HDC) w));
			case WM_COMMAND: FRAME_HANDLE (OnCommand (HIWORD(w),LOWORD(w),(HWND)l));
		}
		return Dispatch (h,m,w,l);
	}

public:
	void SetMessageHandlerResult (LRESULT result)
	{
		msgResult = result;
		msgResultPresent = true;
	}

	virtual void OnPreRegisterClass (WNDCLASSEX & wc)
	{
	}

	virtual void OnPreCreate (CREATESTRUCT & cs)
	{
		cs.style |= WS_CLIPCHILDREN;
	}

	virtual void OnCreate (HWND handle)
	{
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
	}

	virtual void OnSizing (DWORD edge, LPRECT rect)
	{
	}

	virtual void OnKeyDown (int vkey, DWORD data)
	{
		if (VK_ESCAPE == vkey) Message (WM_CLOSE);
	}

	virtual void OnPaint (HDC hdc)
	{
	}

	virtual void OnCommand (WORD code, WORD id, HWND h)
	{
	}

	virtual void OnTranslateAccelerator (LPMSG msg)
	{
	}

	virtual void OnEraseBkgnd (HDC hdc)
	{
	}
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
