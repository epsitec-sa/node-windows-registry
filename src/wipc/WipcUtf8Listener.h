//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#pragma once

#include <functional>
#include <atlbase.h>
#include <atlconv.h>
#include "./Window.h"

namespace Epsitec
{
	namespace Wipc
	{
		class WipcUtf8Listener : public Epsitec::Windows::Window
		{
		public:
			typedef std::function<bool (HWND sender, LPCSTR message)> Callback;

		public:
			WipcUtf8Listener(const Callback& onMessage);
		
		private:
			virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) const;
		
		private:
			Callback onMessage;
		};
	}
}
