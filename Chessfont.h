#pragma once
#include "webparser/FileParser.h"

class Chessfont
{
private:
	HFONT handle;
	char * face;
	char * fontpath;

public:
	Chessfont () : handle(0), face(0), fontpath(0) {}
	Chessfont (const char * face, int size) : handle(0), face(0), fontpath(0)
	{
		Create (face, size);		
	}
	Chessfont (const char * face) : handle(0), face(0), fontpath(0)
	{
		SetFace (face);
	}
	
	operator HFONT () const {return handle;}
	HFONT GetHandle () const {return handle;}

protected:
	void SetFace (const char * text)
	{
		if (face == text) return;
		if (face) delete face;
		face = text ? strdup (text) : 0;
	}

	void SetFontPath (const char * text)
	{
		if (fontpath == text) return;
		if (fontpath) delete fontpath;
		fontpath = text ? strdup (text) : 0;
	}

public:
	bool ExtractFontFaceName (const char * fotpath)
	{
		if (!fotpath || !*fotpath) return false;
		
		FileParser file;
		if (!file.Open (fotpath, "rb")) return false;

		char buffer [128];

		if (!file.Find ("FONTRES:")) return false;
		fgets (buffer, sizeof (buffer), file);

		SetFace (buffer);
		
		return true;
	}

	bool Install (const char * fontpath, bool notify = true)
	{
		SetFontPath (fontpath);
		CreateScalableFontResource (1,"TEMP.FOT",fontpath,0);
		if (0 == AddFontResource (fontpath)) return false;
		
		// This seems to be causeing trouble after the current Windows
		// System Updates (2011-12-15).
		//if (notify) SendMessage (HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

		ExtractFontFaceName ("TEMP.FOT");
		return true;
	}

	bool Uninstall ()
	{
		RemoveFontResource (fontpath);
		DeleteFile ("TEMP.FOT");
		SetFontPath (0);
		return true;
	}


public:
	bool Create (const char * face, int size)
	{
		if (!face || !*face || size < 1) return false;
		
		SetFace (face);

		if (handle) DeleteObject (handle);
		handle = CreateFont (size,0,0,0,0,0,0,0,
			SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			//DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY, FF_DONTCARE, face);
		return !!handle;
	}
	
	bool Delete ()
	{
		if (handle) DeleteObject (handle), handle = 0;
		return true;
	}

	bool Resize (int size)
	{
		Delete ();
		return Create (face, size);
	}
};
