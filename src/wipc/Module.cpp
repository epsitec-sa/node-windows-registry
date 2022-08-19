//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#include "./Module.h"
#include <algorithm>

namespace Epsitec
{
	namespace System
	{
		Module Module::EntryModule()
		{
			HMODULE handle = NULL;
			::GetModuleHandleEx(0, NULL, &handle);
			return Module(handle);
		}
		Module Module::ExecutingModule()
		{
			HMODULE handle = NULL;
			::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&Module::ExecutingModule, &handle);
			return Module(handle);
		}

		CString Module::Path() const
		{
			CString path;
			if (this->IsValid())
			{
				::GetModuleFileName(this->handle, path.GetBuffer(MAX_PATH), MAX_PATH);
				path.ReleaseBuffer();
			}
			return path;
		}

		CStringW Module::PathW() const
		{
			CStringW path;
			if (this->IsValid())
			{
				::GetModuleFileNameW(this->handle, path.GetBuffer(MAX_PATH), MAX_PATH);
				path.ReleaseBuffer();
			}
			return path;
		}

		Module& Module::Attach(HMODULE handle)
		{
			auto previous = (HMODULE)::InterlockedExchangePointer((PVOID*)&this->handle, handle);
			if (previous != handle && previous != NULL)
			{
				::FreeLibrary(previous);
			}
			return *this;
		}
		HMODULE Module::Detach()
		{
			return (HMODULE)::InterlockedExchangePointer((PVOID*)&this->handle, NULL);
		}
	}
}