#pragma once
#include <vector>
#include <string>

#define INVOKE1(x,a) {for (iterator it = begin (); it != end (); it++) (*it)->x (a);}
#define INVOKE2(x,a,b) {for (iterator it = begin (); it != end (); it++) (*it)->x (a,b);}
#define INVOKE(x,a) INVOKE1(x,a)

class Parser
{
public:
	typedef enum {Invalid=0, Opening, Closing, Inline} TagType;

	class Listener
	{
	public:
		typedef std::vector <Listener *> Vector;

	public:
		virtual void OnTagOpen (const char * text) = 0;
		virtual void OnTagClose (const char * text) = 0;
		virtual void OnTagInline (const char * text) = 0;
		virtual void OnAttribute (const char * key, const char * value) = 0;
		virtual void OnText (const char * text) = 0;
	};

	class Listeners : public Listener::Vector, public Listener
	{
	public:
		virtual void OnTagOpen (const char * text) INVOKE1(OnTagOpen, text);
		virtual void OnTagClose (const char * text) INVOKE1(OnTagClose, text);
		virtual void OnTagInline (const char * text) INVOKE1(OnTagInline, text);
		virtual void OnAttribute (const char * key, const char * value) INVOKE2(OnAttribute, key, value);
		virtual void OnText (const char * text) INVOKE(OnText, text);
	};

private:
	Listeners listeners;
	std::string text;

public:
	Parser () {}

	void Add (Listener * listener) { listeners.push_back (listener); }

public:
	void Write (const char * text) {this->text += text;}

	bool Parse ()
	{
		bool tag = false;
		std::string temp;
		std::string white;

		for (int i=0; i<text.size (); i++)
		{
			//bool last = i+1>=text.size ();
			int c = text.at (i);

			if (!tag && '<'==c)
			{
				tag=true;
				if (!temp.empty ()) listeners.OnText (temp.c_str ()), temp = "";
			}
			else if (tag && '>'==c)
			{
				tag=false;
				TagType type = GetTagType (temp, temp);

//TODO: ParseAttributes (temp, attributes);

				switch (type)
				{
					case Opening: listeners.OnTagOpen (temp.c_str ()); break;
					case Closing: listeners.OnTagClose (temp.c_str ()); break;
					case Inline: listeners.OnTagInline (temp.c_str ()); break;
				}
				temp = "";
			}
			else
			{
				AppendCharacterTrimmed (temp, c, white);
			}
		}
		return true;
	}

protected:
	bool IsWhiteSpaceCharacter (char c)
	{
		return ' '==c || '\t'==c || '\n'==c || '\r'==c;
	}

	void AppendCharacterTrimmed (std::string & text, char c, std::string & white)
	{
		if (!text.empty () && IsWhiteSpaceCharacter (c)) white += c;
		else text += white, text += c, white = "";
	}

	TagType GetTagType (std::string & text, std::string & tag)
	{
		int size = text.size ();
		tag = text;

		if (size < 1) return Invalid;
		else if (1 == size) return Opening;
		
		if ('/'==text.at (0)) {
			tag = text.substr (1, size-1);
			return Closing;
		}
		else if ('/'==text.at (size-1))
		{
			tag = text.substr (0, size-1);
			TrimText (text, text);
			return Inline;
		}

		return Opening;
	}

	void TrimText (std::string & text, std::string & out)
	{
		std::string temp = "";
		std::string::const_iterator it = text.begin ();
		for (; it != text.end (); it++)
		{
			if (IsWhiteSpaceCharacter (*it))
				if (temp.empty ()) continue; else break;
			else temp += *it;
		}
		out = temp;
	}
};
