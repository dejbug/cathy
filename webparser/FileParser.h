#pragma once
#include "File.h"

class FileParser : public File
{
public:
	FileParser () {}

public:
	bool Find (const char * text) const
	{
		if (!Ready ()) return false;
		if (!text || !*text) return false;

		int len = strlen (text);
		int pos = 0;
		int c;

//You're wasting too much energy not moving your body.

		while (Char (c))
		{
			if (c == text [pos]) ++pos;
			else {
				if (pos) Skip (-pos+1), pos = 0;
				//if (pos) Skip (-pos), pos = 0;
			}
			if (pos >= len) return true;
		}

		return false;
	}

	bool GetNextToken (char * buffer, size_t size, const char * prefix, const char * suffix)
	{
		if (!Ready ()) return false;
		if (!buffer || size<2) return false;
		if (!prefix && !suffix) return false;

		long substart, subend;

		memset (buffer, 0, size);

		if (!!prefix) if (!Find (prefix)) return false;
		if (!Tell (substart)) return false;
		if (!!suffix) if (!Find (suffix)) return false;
		if (!Tell (subend)) return false;

		size_t sublen = subend - strlen (suffix) + 1 - substart;
		if (sublen < size) size = sublen + 1;
		
		if (!Mark ()) return false;
		if (!Seek (substart)) return false;
		if (!Read (buffer, sublen)) return false;
		if (!Back ()) return false;

		return true;
	}
};
