#pragma once
#include <vector>
#include "lib/Frame.h"
#include "lib/Surface.h"
#include "Time.h"

class ClockListener
{
public:
	typedef std::vector <ClockListener *> Vector;

public:
	virtual void OnClockTick () = 0;
};

class ClockListeners :
	public ClockListener::Vector,
	public ClockListener
{
public:
	virtual void OnClockTick ()
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnClockTick ();
	}
};

class Clock : public lib::gui::Frame
{
private:
	ClockListeners listeners;
	lib::gdi::Surface surface;
	Time black, white;
	bool flipped;
	bool toggled;
	unsigned int timerid;
	bool ticking;

public:
	Clock () : flipped(false), toggled(false), timerid(1001), ticking(false)
	{
		COLORREF cwhite = RGB (255, 255, 255);
		COLORREF cblack = RGB (0, 0, 0);
		white.SetBackgroundColor (cwhite);
		white.SetForegroundColor (cblack);
		black.SetBackgroundColor (cblack);
		black.SetForegroundColor (cwhite);
	}

	bool Create (HWND parent, UINT id, DWORD style = 0, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "CLOCK", id, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|style, exstyle, false);
	}

	void AddListener (ClockListener * listener)
	{
		listeners.push_back (listener);
	}

public:
	Time & GetWhiteTime () { return white; }
	Time & GetBlackTime () { return black; }
	Time & GetTopTime () { return flipped ? white : black; }
	Time & GetBottomTime () { return flipped ? black : white; }

public:
	void Reset ()
	{
		white.Reset ();
		black.Reset ();
		SetFlipped (false);
	}

	bool SetWhiteTime (unsigned char seconds, unsigned char minutes = 0, unsigned char hours = 0)
	{
		return white.Set (seconds, minutes, hours);
	}

	bool SetBlackTime (unsigned char seconds, unsigned char minutes = 0, unsigned char hours = 0)
	{
		return black.Set (seconds, minutes, hours);
	}

	bool Tick ()
	{
		/*
		if (flipped) {if (!black.Increment ()) return false;}
		else if (!white.Increment ()) return false;
		return Invalidate ();
		*/
		if (toggled) black.Tick ();
		else white.Tick ();
		return Invalidate ();
	}

	void Flip ()
	{
		SetFlipped (!flipped);
	}

	void SetFlipped (bool flipped)
	{
		this->flipped = flipped;
		SetToggled (!flipped);
		Invalidate ();
	}

	void Toggle ()
	{
		SetToggled (!toggled);
	}

	void SetToggled (bool toggled)
	{
		this->toggled = toggled;
	}

	bool Resume ()
	{
		if (ticking) return true;
		timerid = SetTimer (handle, timerid, 1000, 0);
		return ticking = !!timerid;
	}

	bool Pause ()
	{
		if (!ticking) return true;
		ticking = !KillTimer (handle, timerid);
		return !ticking;
	}

protected:
	void MakeTopRectangle (RECT & r)
	{
		r.bottom -= (r.bottom - r.top) >> 1;
	}

	void MakeBottomRectangle (RECT & r)
	{
		r.top += (r.bottom - r.top) >> 1;
	}

	bool PaintBlackTime (HDC hdc, RECT & rect)
	{
		RECT r = rect;
		if (flipped) MakeBottomRectangle (r);
		else MakeTopRectangle (r);
		return black.Paint (hdc, r);
	}

	bool PaintWhiteTime (HDC hdc, RECT & rect)
	{
		RECT r = rect;
		if (flipped) MakeTopRectangle (r);
		else MakeBottomRectangle (r);
		return white.Paint (hdc, r);
	}

private:
	virtual void OnCreate (HWND handle)
	{
		surface.Create (handle);
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
		surface.Resize (cx, cy);
	}

	virtual void OnPaint (HDC hdc)
	{
		RECT r; GetClientRect (handle, &r);
		
		PaintWhiteTime (surface, r);
		PaintBlackTime (surface, r);

		surface.Flip ();
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			default: break;

			case WM_ERASEBKGND: return 1;

			case WM_TIMER:
			{
				if (w == timerid) Tick ();
				listeners.OnClockTick ();
			}	break;
		}

		return Frame::Callback (h,m,w,l);
	}
};
