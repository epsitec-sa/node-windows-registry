//	Copyright @ 2013-2022, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#pragma once

#include "RegistryKey.h"

namespace Epsitec
{
	namespace Win32
	{
		class Registry
		{
		public:
			static RegistryKey LocalMachine(bool writable = false) { return RegistryKey(HKEY_LOCAL_MACHINE, writable, true); }
			static RegistryKey CurrentUser(bool writable = true) { return RegistryKey(HKEY_CURRENT_USER, writable, true); }
			static RegistryKey Users(bool writable = true) { return RegistryKey(HKEY_USERS, writable, true); }
			static RegistryKey ClassesRoot(bool writable = false) { return RegistryKey(HKEY_CLASSES_ROOT, writable, true); }
		};
	}
}
