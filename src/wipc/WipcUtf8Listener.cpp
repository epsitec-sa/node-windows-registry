//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#include "./Window.h"
#include "./WipcUtf8Listener.h"

using namespace Epsitec::Windows;

namespace Epsitec
{
	namespace Wipc
	{
		WipcUtf8Listener::WipcUtf8Listener(const WipcUtf8Listener::Callback& onMessage)
			: Window(Window::CreateHandle()), onMessage(onMessage)
		{
		}

		LRESULT WipcUtf8Listener::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) const
		{
			switch (msg)
			{
				case WM_COPYDATA:
				{
					auto sender = (HWND)wParam;
					auto data = (COPYDATASTRUCT*)lParam;

					// convert from UTF8 to Unicode
					//CA2W wMessage((LPCSTR)data->lpData, CP_UTF8);
					
					return this->onMessage(sender, (LPCSTR)data->lpData);
				}
			}
			return false;
		}
	}
}
