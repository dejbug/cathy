#pragma once
#include <winsock2.h>
// #pragma comment (lib,"ws2_32.lib")
#include <stdarg.h>

//VERSION: 2009/03/11

#define WRAP_NOTIFY_LISTENERS(TYPE,CALL) for (TYPE::iterator it = begin (); it != end (); it++) (*it)->CALL

namespace wrap
{
	namespace debug
	{
	#ifdef _DEBUG
		void print (const char * f, ...) {
			va_list l;
			va_start (l, f);
			HANDLE handle = GetStdHandle (STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo (handle, &info);
			SetConsoleTextAttribute (handle, 0xF3);
			vprintf (f, l);
			printf ("\n");
			SetConsoleTextAttribute (handle, info.wAttributes);
			va_end (l);
		}

		void out (const char * f, ...) {
			va_list l; va_start (l, f);
			char buffer [1024];
			buffer [_vsnprintf (buffer, sizeof (buffer)-1,f,l)] = '\0';
			OutputDebugString (buffer);
			va_end (l);
		}
	#else
		__declspec(naked) void print (LPCSTR f, ...) {}
		__declspec(naked) void out (LPCSTR f, ...) {}
	#endif
	};

	namespace mem
	{
		template <unsigned long DIM> class buffer { public:
		static enum {SIZE=DIM > 0 ? DIM : 1};
		char text [DIM];
		unsigned long length;
		unsigned long done;
			buffer () : length (0), done (0) { *text = '\0'; }
			void clear () { *text = '\0'; length = 0; }
			bool set (const char * text) { length = text ? strlen (text) : 0; return length ? strcpy (this->text, text), true : *this->text = '\0', false;}
			bool append (const char * text) { return text ? strcat (this->text, text), length = strlen (this->text), true : false; }
			bool format (const char * format, ...) { va_list l; if (!format) return false; va_start (l,format); vsprintf (text, format, l); va_end (l); return true; }
		};

		template <typename TYPE, unsigned long DIM> class array { public:
		static enum {SIZE=DIM > 0 ? DIM : 1};
		TYPE data [SIZE];
		unsigned long current;
			array () : current (0) {}
			bool full () const { return current >= SIZE; }
			bool empty () const { return 0 == current; }
			bool contains (unsigned long index) const { return index < current; }
			unsigned long count () const { return current; }
			unsigned long capacity () const { return SIZE; }
			void clear () { current = 0; }
			void fill (TYPE & t) { for (unsigned long i=0; i<current; i++) data [i] = t; }
			bool append (TYPE & t) { return full () ? false : data [current++] = t, true; }
			bool insert (TYPE & t, unsigned long index) { return append (data [index]) ? data [index] = t, true : false; }
			bool remove (unsigned long index) { return empty () ? false : data [index] = data [--current], true; }
			bool find (TYPE & t, unsigned long & index) { for (unsigned long i=0; i<current; i++) if (t == data [i]) return index = i, true; return false; }
		};
	};

	namespace sys
	{
		class event { public:
		HANDLE handle;
			event () : handle (INVALID_HANDLE_VALUE) {}
			~event () { CloseHandle (handle); }
			void attach (HANDLE handle) { this->handle = handle; }
			HANDLE detach () { HANDLE temp = handle; return handle = 0, temp;}
			bool create (bool manual=true, bool set=false) { handle = CreateEvent (0, manual ? TRUE : FALSE, set ? TRUE: FALSE, 0); return ready (); }
			bool free () { return ready () ? TRUE == CloseHandle (handle) && INVALID_HANDLE_VALUE == (handle = INVALID_HANDLE_VALUE) : true; }
			bool ready () const { return INVALID_HANDLE_VALUE != handle; }
			bool set () { return TRUE == SetEvent (handle); }
			bool reset () { return TRUE == ResetEvent (handle); }
			bool wait (unsigned long timeout) { return WAIT_OBJECT_0 == WaitForSingleObject (handle, timeout); }
			bool isset () { return WAIT_OBJECT_0 == WaitForSingleObject (handle, 0); }
		};

		template <unsigned long DIM> class events : public wrap::mem::array <HANDLE, SIZE> { public:
		static enum {SIZE=DIM > 64 ? 64 : DIM < 1 ? 1 : DIM};
			bool add (wrap::sys::event & event) { return append (event.handle); }
			bool wait (unsigned long & index, unsigned long timeout = INFINITE, bool all = false, bool alertable = false) { index = WaitForMultipleObjectsEx (count (), data, all ? TRUE : FALSE, timeout, alertable ? TRUE : FALSE) - WAIT_OBJECT_0; return index < count (); }
		};

		class thread { public:
			class listener { public:
			typedef std::vector <listener *> vector;
				virtual void OnThreadEnter (wrap::sys::thread & source) = 0;
				virtual void OnThreadRun (wrap::sys::thread & source) = 0;
				virtual void OnThreadLeave (wrap::sys::thread & source) = 0;
			};
			class listeners: public listener::vector, public listener { public:
				virtual void OnThreadEnter (wrap::sys::thread & source) { WRAP_NOTIFY_LISTENERS (listeners, OnThreadEnter (source)); }
				virtual void OnThreadRun (wrap::sys::thread & source) { WRAP_NOTIFY_LISTENERS (listeners, OnThreadRun (source)); }
				virtual void OnThreadLeave (wrap::sys::thread & source) { WRAP_NOTIFY_LISTENERS (listeners, OnThreadLeave (source)); }
			};
		HANDLE handle;
		unsigned long id;
		listeners notifier;		
			void add (listener * listener) { notifier.push_back (listener); }
			thread () : handle (INVALID_HANDLE_VALUE), id (0) {}
			~thread () { if (handle) CloseHandle (handle); }
			bool create () { free (); handle = CreateThread (0,0,main,(void *)this,0,&id); return ready (); }
			bool free () { return handle ? CloseHandle (handle), handle = 0, id=0, true : id=0, false; }
			bool ready () { return INVALID_HANDLE_VALUE != handle; }
			static unsigned long CALLBACK main (void * vobj) { thread * pobj = (thread *) vobj; pobj->notifier.OnThreadEnter (*pobj); pobj->notifier.OnThreadRun (*pobj); pobj->notifier.OnThreadLeave (*pobj); return 0; }
		};
	};

	namespace net
	{
		class winsock { public:
		WSADATA data;
			bool startup (int major, int minor) { return 0 == WSAStartup (MAKEWORD(major,minor), &data); }
			bool cleanup () { return 0 == WSACleanup (); }
		};

		class address { public:
		SOCKADDR_IN data;
			static bool isip (const char * host) {
				int length = host ? strlen (host) : 0;
				if (length <= 0 || length > 15) return false;
				for (int i=0, digits = 0, dots = 0; i<length; i++) {
					if (host[i] >= '0' && host[i] <= '9') digits++;
					else if ('.' == host[i]) dots++;
					else return false;
				}
				return 3==dots && digits >= 4;
			}
			static bool lookup (const char * name, const char *& ip) {
				hostent * h = gethostbyname (name);
				int code = WSAGetLastError ();
				return h ? (ip = inet_ntoa (*((in_addr *) h->h_addr_list [0]))), true : false;
			}
			bool compile (const char * host, unsigned short port) {
				if (!isip (host) && !lookup (host, host)) return false;
				memset (&data.sin_zero, 0, sizeof (data.sin_zero));
				data.sin_family = AF_INET;
				data.sin_addr.S_un.S_addr = inet_addr (host);
				data.sin_port = htons (port);
				return true;
			}
		};

		class socket { public:
		SOCKET handle;
			socket () : handle(INVALID_SOCKET) {}
			~socket () { closesocket (handle); }
			bool create () { return handle = ::socket (AF_INET, SOCK_STREAM, IPPROTO_TCP), INVALID_SOCKET != handle; }
			bool free () { return ready () ? 0 == closesocket (handle) && INVALID_SOCKET == (handle = INVALID_SOCKET) : false; }
			bool ready () const { return INVALID_SOCKET != handle; }
			bool write (const char * buffer, unsigned long size, unsigned long & done) {
				done = send (handle, buffer, size, 0);
				return SOCKET_ERROR != done || WSAEWOULDBLOCK == WSAGetLastError ();
			}
			bool read (char * buffer, unsigned long size, unsigned long & done) {
				done = recv (handle, buffer, size, 0);
				return 0 != done && (SOCKET_ERROR != done || WSAEWOULDBLOCK == WSAGetLastError ());
			}
			bool disable (bool reading=true, bool writing=true) { return 0 == shutdown (handle, (reading ? SD_RECEIVE : 0) | (writing ? SD_SEND : 0)); }
			bool connect (wrap::net::address & address) { return 0 == ::connect (handle, (SOCKADDR *) &address.data, sizeof (SOCKADDR_IN)); }
			bool disconnect () { return 0 == shutdown (handle, SD_BOTH); }
			bool listen (wrap::net::address & address) { return 0 == ::bind (handle, (SOCKADDR *) &address.data, sizeof (SOCKADDR_IN)) && 0 == ::listen (handle, 5); }

			template <unsigned long DIM> bool write (wrap::mem::buffer<DIM> & buffer) { return write (buffer.text, buffer.length, buffer.done); }
			template <unsigned long DIM> bool read (wrap::mem::buffer<DIM> & buffer) { return read (buffer.text, wrap::buffer<DIM>::SIZE-1, buffer.length) ? buffer.text [buffer.length] = '\0', true : false; }
		};

		class netevent { public:
		WSANETWORKEVENTS data;
			bool enumerate (SOCKET s, HANDLE e = 0) { return 0 == WSAEnumNetworkEvents (s, e, &data); }
			bool isset (long event) { return (event & data.lNetworkEvents) == event; }
		};
	};

	namespace gui
	{
		class base : public WNDCLASSEX { public:
			operator WNDCLASSEX & () { return * (WNDCLASSEX *) offsetof (wrap::gui::base, cbSize); }
			base (const char * name = 0)
			{
				memset ((WNDCLASSEX *) this, 0, sizeof (WNDCLASSEX));
				cbSize = sizeof (WNDCLASSEX);
				style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
				hbrBackground = (HBRUSH) COLOR_WINDOW;
				lpszClassName = name;
				hIcon = hIconSm = LoadIcon (0, MAKEINTRESOURCE (IDI_APPLICATION));
				hCursor = LoadCursor (0, MAKEINTRESOURCE (IDC_ARROW));
			}
			bool loadicon (int id) { hIcon = hIconSm = LoadIcon (hInstance, MAKEINTRESOURCE(id)); return 0 != hIcon; }
			//bool loadcursor (int id) { hCursor = LoadIcon (hInstance, MAKEINTRESOURCE(id)); return 0 != hCursor; }
			bool install () { return 0 != RegisterClassEx ((WNDCLASSEX *) this); }
			bool uninstall () { return 0 != UnregisterClass (lpszClassName, hInstance); }
		};

		class window : public CREATESTRUCT { public:
		HWND handle;
			operator CREATESTRUCT & () { return * (CREATESTRUCT *) offsetof (wrap::gui::window, lpCreateParams); }
			window (wrap::gui::base & base) : handle (0)
			{
				memset ((CREATESTRUCT *) this, 0, sizeof (CREATESTRUCT));
				hInstance = base.hInstance;
				lpszClass = base.lpszClassName;
				style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
				x = y = cx = cy = CW_USEDEFAULT;
			}
			bool create () {
				handle = CreateWindowEx (dwExStyle, lpszClass, lpszName, style, x, y, cx, cy, hwndParent, hMenu, hInstance, lpCreateParams);
				return 0 != handle;
			}
			bool show (int cmd) { return TRUE == ShowWindow (handle, cmd); }
			bool update () { return TRUE == UpdateWindow (handle); }
		};

		class msg : public MSG { public:
			operator MSG & () { return * (MSG *) offsetof (wrap::gui::msg, hwnd); }
			msg () { memset ((MSG *) this, 0, sizeof (MSG)); }
			bool get (HWND handle=0, UINT min=0, UINT max=0) { return 0 < GetMessage ((MSG *) this, handle, min, max); }
			bool translate () { return TRUE == TranslateMessage ((MSG *) this); }
			bool dispatch () { return TRUE == DispatchMessage ((MSG *) this); }
		};
	};
};
