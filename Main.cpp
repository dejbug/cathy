#include <windows.h>

#include "resource.h"
#include "lib/Frame.h"
#include "lib/Font.h"
#include "lib/Surface.h"
#include "lib/Cursor.h"
#include "Chessboard.h"
#include "BrowserControl.h"

/**
	The main window's class.
*/
class Main :
	public lib::gui::Frame,
	public BrowserControlListener,
	public ChessboardListener

{
private:
	Chessboard chessboard;
	BrowserControl control;

	HACCEL haccel;

public:
	Main () : haccel(0) {}

private:
	virtual void OnPreRegisterClass (WNDCLASSEX & wc)
	{
		wc.hIcon = wc.hIconSm = LoadIcon (wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	}

	virtual void OnCreate (HWND handle)
	{
		chessboard.Create (handle, 1000, WS_VISIBLE, WS_EX_STATICEDGE);
		chessboard.SetLocation (0, 0);
		chessboard.AddListener (&control);
		chessboard.AddListener (this);

		control.Create (handle, 1001, WS_VISIBLE, WS_EX_STATICEDGE);
		control.AddListener (this);
		control.SetChessboard (&chessboard);

		control.BrowseDatabase ();

		haccel = LoadAccelerators (GetInstance (handle), MAKEINTRESOURCE (IDR_ACCELERATOR1));

		SetFocus(chessboard.GetHandle());
	}

public:
	/** Resizes the window so it fits the chessboard control.
	*/
	void FitWindowToChessboard ()
	{
		RECT r; ::GetClientRect (handle, &r);
		int min = r.right < r.bottom ? r.right : r.bottom;
		FitToSize (min, min);
	}

	void OnChessboardWheelEvent (short delta)
	{
	}

	void OnChessboardUserMove (unsigned char src, unsigned char dst, bool ok)
	{
	}

	void OnChessboardFlipped (bool inverted)
	{
	}

	void OnChessboardLoaded (const Database2::Entry & entry)
	{
		static bool firstLoaded = true;

		if(firstLoaded)
		{
			firstLoaded = false;
			return;
		}

		PROBLEM_DETAILS details;
		control.GetProblemDetails(details);
		
		int rating = atoi(details.rating);
		char stars[32];
		if(rating > 2000)
			strcpy(stars, "{# # # # # # #}");
		else if(rating > 1600)
			strcpy(stars, "{- # # # # # -}");
		else if(rating > 1400)
			strcpy(stars, "{- - # # # - -}");
		else if(rating > 1100)
			strcpy(stars, "{- - - # - - -}");
		else
			strcpy(stars, "{- - - - - - -}");
		
		char buffer[1024];
		_snprintf(buffer, 1024, "cathy - [% 4s] (%s) p%05s           %s",
			details.rating, details.moves, details.id, stars);
		
		SetTitle(buffer);
	}

	void OnChessboardTurnChange (unsigned char color)
	{
	}

	void OnChessboardUserInput (unsigned int key, unsigned int value)
	{
	}

/*
	bool GetClientRect (RECT & rect)
	{
		RECT wr; GetWindowRect (handle, &wr);
		rect = wr;
		Message (WM_NCCALCSIZE, FALSE, (LONG) &rect);

		rect.left -= wr.left;
		rect.top -= wr.top;
		rect.right -= wr.left;
		rect.bottom -= wr.top;
		return true;
	}
*/

	/**
		Hides or restores the control panel.
	*/
	void ToggleControl ()
	{
		if (control.GetWindowWidth () > 32) FitWindowToChessboard ();
		else InflateBounds (0x1, 150);
	}

private:
	virtual void OnMinimumSizeThreshold ()
	{
		FitWindowToChessboard ();
	}

private:
	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
		/*
		if (cy > cx)
		{
			FitToSize (cx, cx);
		}
		*/

		WORD min = cx < cy ? cx : cy;

		chessboard.SetSize (min, min);
		control.Move (min+3, 0, cx-min-3, cy);
	}

	virtual void OnSizing (DWORD edge, LPRECT rect)
	{
		RECT & r = *rect;

		int cx = r.right-r.left;
		int cy = r.bottom-r.top;
		static const int extra = GetSystemMetrics (SM_CYCAPTION);

		static const int controlSize = 300;

		if (WMSZ_BOTTOM == edge)
		{
			if (cy-extra > cx) r.right += cy-cx-extra;
		}

		else if (WMSZ_RIGHT == edge)
		{
			if (cy-extra > cx) r.bottom -= cy-cx-extra;
		}
	}

	bool DrawDragRect (LPRECT rect, LPRECT old = 0)
	{
		HDC hdc = GetWindowDC (handle);
		SetROP2 (hdc, R2_NOT);
		//HPEN pen = CreatePen (PS_SOLID, 3, 0);
		//SelectObject (hdc, pen);
		SelectObject (hdc, GetStockObject (BLACK_PEN));
		SelectObject (hdc, GetStockObject (NULL_BRUSH));
		if (old) Rectangle (hdc, old->left, old->top, old->right, old->bottom);
		Rectangle (hdc, rect->left, rect->top, rect->right, rect->bottom);
		//SelectObject (hdc, GetStockObject (BLACK_PEN));
		//DeleteObject (pen);
		ReleaseDC (handle, hdc);
		return true;
	}

	virtual void OnCommand (WORD code, WORD id, HWND h)
	{
		switch (id)
		{
			case 1234: ToggleControl (); break;
		}
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		static bool dragging = false;
		static bool hasdr = false;
		static RECT dr = {0, 0, 0, 0};

		switch (m)
		{
			case WM_LBUTTONDOWN:
			{
			}	break;

			case WM_LBUTTONUP:
			{
			}	break;

			case WM_USER + 4711:
			{
				if (0 == w)
				{
					SetCapture (handle);
					dragging = true;
					hasdr = false;
				}
				else
				{
					ReleaseCapture ();
					dragging = false;
				}
			}	break;

			/*
			case WM_MBUTTONUP:
			{
				ReleaseCapture ();
				dragging = false;

				RECT r = {LOWORD(l), HIWORD(l), dr.right-dr.left, dr.bottom-dr.top};
				DrawDragRect (&dr, 0);
			}	break;
			*/

			case WM_MOUSEMOVE:
			{
				if (dragging)
				{
					RECT r = {LOWORD(l), HIWORD(l), dr.right-dr.left, dr.bottom-dr.top};
					DrawDragRect (&r, hasdr ? &dr : 0);
					dr = r;
					hasdr = true;
				}
			}	break;
		}
		return Frame::Callback (h,m,w,l);
	}

	virtual void OnTranslateAccelerator (LPMSG msg)
	{
		TranslateAccelerator (handle, haccel, msg);
	}
};

/**
	Main application entry point.
*/
int WINAPI WinMain (HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	InitCommonControls ();

	Main main;
	main.Create (instance, "cathy v0.8 by dejbug ( a.k.a. plipsticks )          +++ click middle button +++ roll mousewheel +++ press F4 ! +++", WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	main.SetSize (600, 592);  // Side panel is hidden at this size.
	// main.SetSize (740, 592);
	main.CenterToScreen ();
	main.Show ();
	
	return main.Run ();
}
