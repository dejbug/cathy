#pragma once

class Time
{
public:
	int hours;
	int minutes;
	int seconds;
	COLORREF fg, bg;
	HBRUSH brush;
	bool countdown;

public:
	Time () : hours(0), minutes(0), seconds(0), fg(0), bg(0), brush(0),
		countdown(false)
	{
	}

	~Time ()
	{
		if (brush) DeleteObject (brush);
	}

public:
	void Reset ()
	{
		hours = minutes = seconds = 0;
		countdown = false;
	}

	bool Set (unsigned char seconds, unsigned char minutes = 0, unsigned char hours = 0)
	{
		minutes += seconds / 60;
		hours += minutes / 60;
		seconds %= 60;
		minutes %= 60;

		this->seconds = seconds;
		this->minutes = minutes;
		this->hours = hours;

		return NextIncrementWillOverflow () ? Reset (), false : true;
	}

	bool NextIncrementWillOverflow () const
	{
		return !(seconds < 59 || minutes < 59 || hours < 23);
	}

	bool NextDecrementWillOverflow () const
	{
		return !(seconds > 0 || minutes > 0 || hours > 0);
	}

	bool Increment ()
	{
		if (NextIncrementWillOverflow ()) return false;

		++seconds;

		if (seconds > 59)
		{
			seconds = 0;
			++minutes;

			if (minutes > 59)
			{
				minutes = 0;
				++hours;
			}
		}

		return true;
	}

	bool Decrement ()
	{
		if (NextDecrementWillOverflow ()) return false;

		--seconds;

		if (seconds < 0)
		{
			seconds = 59;
			--minutes;

			if (minutes < 0)
			{
				minutes = 59;
				--hours;
			}
		}

		return true;
	}

	bool Tick ()
	{
		return countdown ? Decrement () : Increment ();
	}

public:
	const char * GetHoursText () const
	{
		static char text [8];
		return itoa (hours, text, 10);
	}

	const char * GetMinutesText () const
	{
		static char text [8];
		return itoa (minutes, text, 10);
	}

	const char * GetSecondsText () const
	{
		static char text [8];
		return itoa (seconds, text, 10);
	}

	const char * GetText () const
	{
		static char text [16];
		sprintf (text, "%02d:%02d:%02d", hours, minutes, seconds);
		return text;
	}

public:
	void SetCountdown (bool countdown = true)
	{
		this->countdown = countdown;
	}

	bool SetBackgroundColor (COLORREF color)
	{
		if (color == bg && !!brush) return true;
		if (brush) DeleteObject (brush);
		return brush = CreateSolidBrush (color), !!brush;
	}

	void SetForegroundColor (COLORREF color)
	{
		fg = color;
	}

	bool Paint (HDC hdc, RECT & r)
	{
		HGDIOBJ oldbrush = SelectObject (hdc, brush);
		HGDIOBJ oldpen = SelectObject (hdc, GetStockObject (NULL_PEN));
		Rectangle (hdc, r.left, r.top, r.right, r.bottom);
		SelectObject (hdc, oldbrush);
		SelectObject (hdc, oldpen);

		const char * text = GetText ();

		SetBkMode (hdc, TRANSPARENT);
		SetTextColor (hdc, fg);
		DrawText (hdc, text, strlen (text), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		return true;
	}
};
