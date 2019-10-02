#pragma once
#include "lib/Frame.h"

class Static : public lib::gui::Frame
{
private:
	char * text;
	char * format;
	char * altformat;

public:
	Static () : text(0), format(0), altformat(0)
	{
		SetText (&text, "");
		SetText (&format, "%s");
		SetText (&altformat, "");
	}

	~Static ()
	{
		SetText (&text, 0);
		SetText (&format, 0);
		SetText (&altformat, 0);
	}

public:
	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "STATIC", id, style, exstyle, true);
	}

protected:
	static bool SetText (char ** var, const char * text)
	{
		if (!var) return false;
		if (*var) if (*var == text) return true; else delete[] *var;
		*var = text ? strdup (text) : 0;
		return !!*var;
	}

public:
	void SetFormat (const char * text)
	{
		SetText (&format, text);
	}

	void SetAlternateFormat (const char * text)
	{
		SetText (&altformat, text);
	}

	const char * GetText () const
	{
		return text ? text : "";
	}

	void SetText (const char * format, ...)
	{
		char buffer [1024];
		va_list list;
		va_start (list, format);
		_vsnprintf (buffer, sizeof (buffer), format, list);
		va_end (list);

		SetText (&text, buffer);
		Update ();
	}

	void Update ()
	{
		char buffer [128];
		char * textformat = text && *text ? (!format ? "%s" : format) : (!altformat ? "" : altformat);
		sprintf (buffer, textformat, text);
		SetWindowText (handle, buffer);
	}

private:
	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_PAINT: Update (); break;
		}
		return Frame::Callback (h,m,w,l);
	}
};
