#pragma once
#include <string.h>

class File
{
private:
	FILE * handle;
	long mark;

public:
	File () : handle(0), mark(0) {}
	~File () { Close (); }
	operator FILE * () const { return handle; }
	FILE * Handle () const { return handle; }

public:
	bool Open (const char * path, const char * mode)
	{
		Close ();
		return handle = fopen (path, mode), !!handle;
	}

	void Close () { if (handle) fclose (handle), handle = 0; }

	bool Exists (const char * path) const
	{
		FILE * file = fopen (path, "r");
		if (!file) return false;
		return fclose (file), true;
	}

	bool Create (const char * path) const
	{
		FILE * file = fopen (path, "wb");
		if (!file) return false;
		return fclose (file), true;
	}

	bool Ready () const { return !!handle; }
	bool More () const { return !!handle && !feof (handle); }

	bool Tell (long & offset) const
	{
		if (!Ready ()) return false;
		offset = ftell (handle);
		return -1L != offset;
	}

	bool Seek (long pos) const
	{
		if (!Ready ()) return false;
		int mode = pos >= 0 ? SEEK_SET : SEEK_END;
		if (pos < 0) pos = -pos;
		return 0 == fseek (handle, pos, mode);
	}

	bool Skip (long pos) const
	{
		return Ready () && 0 == fseek (handle, pos, SEEK_CUR);
	}

	bool Read (char * buffer, size_t size, size_t * done=0) const
	{
		if (!More () || !buffer || size<2) return false;
		size_t temp; if (!done) done = &temp;
		*done = fread (buffer, sizeof(char), size-1, handle);
		if (!*done) return false;
		buffer [*done] = 0;
		return !ferror (handle);
	}

	bool ReadExactly (char * buffer, size_t size, size_t * done=0) const
	{
		if (!More () || !buffer || size<1) return false;
		size_t temp; if (!done) done = &temp;
		*done = fread (buffer, sizeof(char), size, handle);
		return *done == size || !ferror (handle);
	}

	bool Write (const char * buffer, size_t size, size_t * done=0) const
	{
		if (!Ready ()) return false;
		size_t temp; if (!done) done = &temp;
		return *done = fwrite (buffer, sizeof(char), size, handle), size == *done;
	}

	bool Print (const char * text) const
	{
		if (!Ready ()) return false;
		size_t length = strlen (text);
		return length == fwrite (text, sizeof(char), length, handle);
	}

	bool Char (int & c) const
	{
		if (!Ready ()) return false;
		return c = fgetc (handle), EOF != c;
	}

	long Size () const
	{
		long offset = ftell (handle);
		fseek (handle, 0, SEEK_END);
		long size = ftell (handle);
		fseek (handle, offset, SEEK_SET);
		return size;
	}

	bool Mark ()
	{
		return Tell (mark);
	}

	bool Back ()
	{
		return Seek (mark);
	}

	bool Rewind () const
	{
		if (!Ready ()) return false;
		return rewind (handle), true;
	}
};
