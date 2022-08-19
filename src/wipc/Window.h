//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#pragma once

#include <windows.h>

namespace Epsitec
{
	namespace Windows
	{
		class Window
		{
		public:
			static HWND CreateHandle(HINSTANCE hInst = 0, LPCTSTR className = 0, DWORD style = WS_POPUP, HWND parent = 0);

		public:
			Window(HWND hWnd);
			virtual ~Window();

		public:
			operator HWND  () const { return this->hWnd; }
			HWND	 Handle() const { return this->hWnd; }
			bool     Post(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return !!::PostMessage(this->hWnd, msg, wParam, lParam); }
			LRESULT  Send(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return   ::SendMessage(this->hWnd, msg, wParam, lParam); }

		protected:
			virtual LRESULT WindowProc   (UINT msg, WPARAM wParam, LPARAM lParam) const;
			LRESULT         DefWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) const { return ::DefWindowProc(this->hWnd, msg, wParam, lParam); }

		private:
			static LPCTSTR        RegisterWndClass(HINSTANCE, LPCTSTR className);
			static LRESULT WINAPI ClassWindowProc(HWND, UINT, WPARAM, LPARAM);

		private:
			HWND hWnd;
		};
	}
}
