//	Copyright @ 2013-2019, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#pragma once

#include <windows.h>
#include <atlstr.h>

namespace Epsitec
{
	namespace System
	{
		class Module
		{
		public:
			static Module EntryModule();
			static Module ExecutingModule();

		public:
			Module(Module&& module) { this->handle = module.Detach(); }
			Module& operator =(Module&& module) { return this->Attach(module.Detach()); }
			virtual ~Module() { this->Attach(NULL); }
			operator HMODULE() const { return this->handle; }
		
		public:
			bool		IsValid()  const { return this->handle != NULL; }
			CString		Path()   const;
			CStringW	PathW()  const;

		private:
			Module(HMODULE handle) : handle(handle) {}
			Module& Attach(HMODULE handle);
			HMODULE Detach();

		private:
			Module() = delete;
			Module(const Module& module) = delete;
			Module& operator =(const Module& module) = delete;

		private:
			HMODULE handle;
		};
	}
}

