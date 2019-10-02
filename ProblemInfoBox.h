#pragma once
#include "lib/Frame.h"

class ProblemInfoBox : public lib::gui::Frame
{
public:
	ProblemInfoBox ()
	{
	}

	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "LISTBOX", id,
			WS_VSCROLL|WS_BORDER|LBS_DISABLENOSCROLL|LBS_HASSTRINGS|LBS_NOINTEGRALHEIGHT|style,
			exstyle, true);
	}

public:
	void AppendText (const char * text)
	{
		if (text) Message (LB_ADDSTRING, -1, (LONG) text);
	}

protected:

private:
	virtual void OnCreate (HWND handle)
	{
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_LBUTTONDOWN: SetFocus (h); break;
			case WM_RBUTTONDOWN: AppendText ("hi"); break;
		}
		return Dispatch (h,m,w,l);
	}
};

class ProblemInfoBox2 : public lib::gui::Frame
{
private:
	int lineCount;
	int firstLine;
	int lineHeight;
	int linesPerPage;

	int g_yOrigin;

public:
	ProblemInfoBox2 () :
		lineCount(20), firstLine(0), lineHeight(23), linesPerPage(0),
		g_yOrigin(0)
	{
	}

	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "PROBLEMINFOBOX", id, WS_VSCROLL|style, exstyle, false);
	}

protected:
	void DrawGrid (HDC hdc, RECT & r)
	{
		SetBkMode (hdc, TRANSPARENT);

		for (int i = 0; i < lineCount; i++) {
			char szLine[256];
			int cch = wsprintf(szLine, "This is line %d", i);
			TextOut(hdc, 3+0, 2+i * lineHeight, szLine, cch);
		}
	}

	void SetScrollPos (int pos, bool redraw = true)
	{
		::SetScrollPos (handle, SB_VERT, pos, redraw ? TRUE : FALSE);
	}

	int GetScrollPos () const
	{
		return ::GetScrollPos (handle, SB_VERT);
	}

private:
	virtual void OnCreate (HWND handle)
	{
		SetScrollRange (handle, SB_VERT, 0, 6, FALSE);

		HDC hdc = GetDC(handle);
		SIZE siz;
		GetTextExtentPoint(hdc, TEXT("0"), 1, &siz);
		lineHeight = siz.cy;
		ReleaseDC(handle, hdc);
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
		linesPerPage = cy / lineHeight;
		ScrollDelta (0);
	}

	virtual void OnEraseBkgnd (HDC hdc)
	{
		RECT r; GetClientRect (handle, &r);
		SelectStockObject (hdc, WHITE_BRUSH);
		SelectStockObject (hdc, NULL_PEN);
		Rectangle (hdc, 0, 0, r.right, r.bottom);
		SetMessageHandlerResult (1);
	}

	virtual void OnPaint (HDC hdc)
	{
		PAINTSTRUCT ps = {0};

		if (!hdc) hdc = BeginPaint (handle, &ps);

		POINT ptOrgPrev;
		OffsetRect(&ps.rcPaint, 0, g_yOrigin * lineHeight);
		GetWindowOrgEx(ps.hdc, &ptOrgPrev);
		SetWindowOrgEx(ps.hdc, ptOrgPrev.x, ptOrgPrev.y + g_yOrigin * lineHeight, 0);

		DrawGrid (hdc, ps.rcPaint);

		SetWindowOrgEx(ps.hdc, ptOrgPrev.x, ptOrgPrev.y, 0);

		if (ps.hdc) EndPaint (handle, &ps);
	}

	virtual void OnKeyDown (int vkey, DWORD data)
	{
		ScrollWindowEx(handle, 0, -lineHeight,
			   0,0,0,0, SW_ERASE | SW_INVALIDATE);
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_LBUTTONDOWN: SetFocus (h); break;

			case WM_VSCROLL:
			{
				int nScrollCode = LOWORD(w);
				short nPos = HIWORD(w);
				HWND hwndScrollBar = (HWND) l;

				switch (nScrollCode)
				{
					case SB_TOP: ScrollTo (0); break;
					case SB_BOTTOM: ScrollTo (MAXLONG); break;

					case SB_LINEDOWN: ScrollDelta (+1); break;
					case SB_LINEUP: ScrollDelta(-1); break;

					case SB_PAGEDOWN: ScrollDelta (+linesPerPage); break;
					case SB_PAGEUP: ScrollDelta (-linesPerPage); break;

					case SB_THUMBTRACK: ScrollTo (nPos); break;
					case SB_THUMBPOSITION: ScrollTo (nPos); break;
				}

			}	return 0;
		}
		return Frame::Callback (h,m,w,l);
	}


protected:
	void ScrollTo(int pos)
	{
		/*
		 *  Keep the value in the range 0 .. (g_cItems - g_cLinesPerPage).
		 */
		pos = __max(pos, 0);
		pos = __min(pos, lineCount - linesPerPage);

		/*
		 *  Scroll the window contents accordingly.
		 */
		ScrollWindowEx(handle, 0, (g_yOrigin - pos) * lineHeight,
					   NULL, NULL, NULL, NULL,
					   SW_ERASE | SW_INVALIDATE);

		/*
		 *  Now that the window has scrolled, a new item is at the top.
		 */
		g_yOrigin = pos;

		/*
		 *  And make the scrollbar look like what we think it is.
		 */
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nPage = linesPerPage;
		si.nMin = 0;
		si.nMax = lineCount - 1;     /* endpoint is inclusive */
		si.nPos = g_yOrigin;
		SetScrollInfo(handle, SB_VERT, &si, TRUE);

		RedrawWindow (handle, 0, 0, RDW_ERASENOW);
	}

	void ScrollDelta(int dpos)
	{
		ScrollTo(g_yOrigin + dpos);
	}
};
