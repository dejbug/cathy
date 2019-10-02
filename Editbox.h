#pragma once
#include <vector>
#include "lib/Frame.h"

class Editbox : public lib::gui::Frame
{
private:
	

public:
	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "EDIT",id, WS_CHILD|style, exstyle);
	}
};

class Editbox2;

class Editbox2Listener
{
public:
	typedef std::vector <Editbox2Listener *> Vector;

public:
	virtual void OnUserInputDone (Editbox2 & box, const char * text) = 0;
};

class Editbox2Listeners :
	public Editbox2Listener::Vector,
	public Editbox2Listener
{
public:
	virtual void OnUserInputDone (Editbox2 & box, const char * text)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnUserInputDone (box, text);
	}
};

class Editbox2 : public Editbox
{
private:
	Editbox2Listeners listeners;
	char * text;
	char * format;
	char * altformat;
	bool editable;

public:
	Editbox2 () : text(0), format(0), altformat(0), editable(true)
	{
		SetText (&text, "");
		SetText (&format, "%s");
		SetText (&altformat, "");
	}

	~Editbox2 ()
	{
		SetText (&text, 0);
		SetText (&format, 0);
		SetText (&altformat, 0);
	}

	void AddListener (Editbox2Listener * listener)
	{
		listeners.push_back (listener);
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

	void SetText (const char * text)
	{
		SetWindowText (handle, text);
		RememberText ();
		OnUserEditEnd ();
	}

	void SetEditable (bool editable)
	{
		this->editable = editable;
	}

protected:
	void RememberText ()
	{
		char buffer [8];
		GetWindowText (handle, buffer, sizeof (buffer));
		SetText (&text, buffer);
	}

	void RecallText ()
	{
		SetWindowText (handle, text);
	}

	void SetReadOnly (bool on=true)
	{
		Message (EM_SETREADONLY, on ? TRUE : FALSE);
	}

protected:
	virtual void OnUserEditBegin ()
	{
		RecallText ();
		//SetReadOnly (false);
	}

	virtual void OnUserEditEnd ()
	{
		if (editable) RememberText ();
		else RecallText ();

		char buffer [128];
		char * textformat = text && *text ? (!format ? "%s" : format) : (!altformat ? "" : altformat);
		sprintf (buffer, textformat, text);
		SetWindowText (handle, buffer);

		//SetReadOnly (true);
	}

private:
	virtual void OnCreate (HWND handle)
	{
		//SetReadOnly (true);
	}

	virtual void OnKeyDown (int vkey, DWORD)
	{
		switch (vkey)
		{
			case VK_RETURN: SetFocus (GetParent (handle)); break;
		}
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_MOUSEWHEEL: return 1;

			case WM_SETFOCUS:
				OnUserEditBegin ();
				break;
			
			case WM_KILLFOCUS:
				OnUserEditEnd ();
				listeners.OnUserInputDone (*this, text);
				break;
		}
		return Frame::Callback (h,m,w,l);
	}
};
