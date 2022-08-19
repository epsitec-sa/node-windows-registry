//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#include <functional>
#include "./Window.h"
#include "./Module.h"

using namespace Epsitec::System;

namespace Epsitec
{
	namespace Windows
	{
		HWND Window::CreateHandle(HINSTANCE hInst, LPCTSTR className, DWORD style, HWND parent)
		{
			if (!hInst) hInst = Module::ExecutingModule();
			className = Window::RegisterWndClass(hInst, className);

			auto hWnd = ::CreateWindowEx(0, className, NULL, style, 0, 0, 0, 0, parent, NULL, hInst, NULL);
			//ASSERT(hWnd != 0);
			::SetWindowText(hWnd, className);
			return hWnd;
		}
		LPCTSTR Window::RegisterWndClass(HINSTANCE hInst, LPCTSTR className)
		{
			static TCHAR defClassName[] = _T("Node.Windows.Worker");
			if (!className || !*className)
			{
				className = defClassName;
			}
			WNDCLASS wndcls;
			if (!::GetClassInfo(hInst, className, &wndcls))
			{
				wndcls.style         = 0;
				wndcls.lpfnWndProc   = (WNDPROC)Window::ClassWindowProc;
				wndcls.cbClsExtra    = 0;
				wndcls.cbWndExtra    = sizeof(Window*);
				wndcls.hInstance     = hInst;
				wndcls.hIcon         = 0;
				wndcls.hCursor       = 0;
				wndcls.hbrBackground = 0;
				wndcls.lpszMenuName  = NULL;
				wndcls.lpszClassName = className;

				/*VERIFY(*/::RegisterClass(&wndcls)/*)*/;
			}
			return className;
		}

		Window::Window(HWND hWnd)
			: hWnd(hWnd)
		{
			//ASSERT(hWnd != 0);
			::SetWindowLongPtr(hWnd, 0, (LONG_PTR)this);
		}
		Window::~Window()
		{
			::DestroyWindow(this->hWnd);
		}

		LRESULT Window::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) const
		{
			return this->DefWindowProc(msg, wParam, lParam);
		}

		LRESULT WINAPI Window::ClassWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			auto pThis = (Window*)::GetWindowLongPtr(hWnd, 0);
			return pThis
				? pThis->WindowProc(msg, wParam, lParam)
				: ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
}
