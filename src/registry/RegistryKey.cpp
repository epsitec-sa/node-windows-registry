//	Copyright @ 2013-2022, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#include "RegistryKey.h"

namespace Epsitec
{
	namespace Win32
	{
		RegistryKey::RegistryKey(HKEY handle, bool writable, bool systemKey, RegistryView regView)
		{
			this->handle = handle;
			this->regView = regView;
			this->isDynamicallyAllocated = false;

			this->state = 0;
			if (systemKey)
				this->state |= 2;
			if (writable)
				this->state |= 4;
		}
		RegistryKey::RegistryKey(HKEY handle, int state, RegistryView regView)
		{
			// only used for dynamical allocation
			this->handle = handle;
			this->state = state;
			this->regView = regView;
			this->isDynamicallyAllocated = true;
		}
		RegistryKey::RegistryKey(RegistryKey &&key)
		{
			this->handle = const_cast<RegistryKey &>(key).DetachHandle();
			this->regView = key.regView;
			this->state = key.state;
			this->isDynamicallyAllocated = false;
		}
		RegistryKey::~RegistryKey()
		{
			if (!this->IsSystemKey() && !this->isDynamicallyAllocated)
			{
				auto handle = reinterpret_cast<HKEY>(::InterlockedExchangePointer(reinterpret_cast<void **>(&this->handle), nullptr));
				if (handle != nullptr)
					::RegCloseKey(handle);
			}
		}

		RegistryKey *RegistryKey::Malloc()
		{
			this->isDynamicallyAllocated = true; // this prevents destructor from releasing handle
			return new RegistryKey(this->handle, this->state, this->regView);
		}
		void RegistryKey::Release()
		{
			if (this->isDynamicallyAllocated)
			{
				auto handle = reinterpret_cast<HKEY>(::InterlockedExchangePointer(reinterpret_cast<void **>(&this->handle), nullptr));
				if (handle != nullptr)
					::RegCloseKey(handle);
			}
		}

		/*RegistryKey RegistryKey::CreateSubkey(LPCTSTR subkey, bool writable, RegistryOptions options) const
		{
			this->EnsureWritable();
			HKEY hKey;
			DWORD disposition;
			auto result = ::RegCreateKeyEx(this->handle, subkey, 0, nullptr, (DWORD)options, this->AccessMask(writable), nullptr, &hKey, &disposition);
			if (result == ERROR_SUCCESS)
				return RegistryKey(hKey, writable, false, this->regView);
			return RegistryKey();
		}*/
		RegistryKey RegistryKey::OpenSubkey(std::wstring name, bool writable, int view) const
		{
			this->EnsureValid();
			auto nameW = name.c_str();
			auto newRegView = this->ComputeRegistryView(view);
			HKEY hkey;
			auto result = ::RegOpenKeyExW(this->handle, nameW, 0, this->AccessMask(writable, newRegView), &hkey);
			if (result == ERROR_SUCCESS)
			{
				return RegistryKey(hkey, writable, false, newRegView);
			}
			else
			{
				throw RegistryException(result, _T("Error during call to RegOpenKeyEx\n"));
			}
		}
		/*bool RegistryKey::DeleteSubkey(LPCTSTR subkey) const
		{
			this->EnsureWritable();
			auto result = ::RegDeleteKeyEx(this->handle, subkey ? subkey : _T(""), (DWORD)this->regView, 0);
			return result == ERROR_SUCCESS;
		}
		bool RegistryKey::DeleteSubkeyTree(LPCTSTR subkey) const
		{
			this->EnsureWritable();
			auto isSystemTree = this->IsSystemKey() && (subkey == nullptr || subkey[0] == 0);
			if (!isSystemTree && this->IsWritable())
			{
				return this->DeleteSubkeyTreeInternal(subkey);
			}
			return false;
		}
		bool RegistryKey::DeleteSubkeyTreeInternal(LPCTSTR subkey) const
		{
			auto result = true;
			{
				auto key = this->OpenSubkey(subkey, true);
				if (key.IsValid())
				{
					auto names = key.SubkeyNames();
					for (auto &&name : names)
					{
						if (!key.DeleteSubkeyTreeInternal(name.c_str()))
							result = false;
					}
				}
			}
			if (ERROR_SUCCESS != ::RegDeleteKeyEx(this->handle, subkey ? subkey : _T(""), (DWORD)this->regView, 0))
				result = false;

			return result;
		}*/
		int RegistryKey::SubkeyCount(LPDWORD maxSubkeyLength) const
		{
			this->EnsureValid();
			DWORD subkeyCount;
			auto result = ::RegQueryInfoKey(this->handle, nullptr, nullptr, nullptr, &subkeyCount, maxSubkeyLength, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
			if (result == ERROR_SUCCESS)
				return subkeyCount;
			return -1;
		}
		int RegistryKey::ValueCount(LPDWORD maxValueLength) const
		{
			this->EnsureValid();
			DWORD valueCount;
			auto result = ::RegQueryInfoKey(this->handle, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &valueCount, maxValueLength, nullptr, nullptr, nullptr);
			if (result == ERROR_SUCCESS)
				return valueCount;
			return -1;
		}
		std::vector<std::wstring> RegistryKey::SubkeyNames() const
		{
			this->EnsureValid();
			std::vector<std::wstring> names;
			auto count = this->SubkeyCount();
			if (count > 0)
			{
				wchar_t buffer[256];
				for (int i = 0; i < count; ++i)
				{
					DWORD size = sizeof(buffer);
					auto result = ::RegEnumKeyExW(this->handle, i, buffer, &size, nullptr, nullptr, nullptr, nullptr);
					if (result == ERROR_SUCCESS)
					{
						names.push_back(std::wstring(buffer));
					}
					else if (result == ERROR_NO_MORE_ITEMS)
					{
						break;
					}
					else
					{
						throw RegistryException(result, _T("Error during call to RegEnumKeyEx\n"));
					}
				}
			}
			return names;
		}
		std::vector<std::wstring> RegistryKey::ValueNames() const
		{
			this->EnsureValid();
			std::vector<std::wstring> names;
			auto count = this->ValueCount();
			if (count > 0)
			{
				wchar_t buffer[256];
				for (int i = 0; i < count; ++i)
				{
					DWORD size = sizeof(buffer);
					auto result = ::RegEnumValueW(this->handle, i, buffer, &size, nullptr, nullptr, nullptr, nullptr);
					if (result == ERROR_SUCCESS)
					{
						names.push_back(std::wstring(buffer));
					}
					else if (result == ERROR_NO_MORE_ITEMS)
					{
						break;
					}
					else
					{
						throw RegistryException(result, _T("Error during call to RegEnumValue\n"));
					}
				}
			}
			return names;
		}
		/*bool RegistryKey::DeleteValue(LPCTSTR name) const
		{
			this->EnsureWritable();
			auto result = ::RegDeleteValue(this->handle, name);
			return result == ERROR_SUCCESS;
		}

		bool RegistryKey::SetMultiString(LPCTSTR name, std::vector<LPCTSTR> value) const
		{
			size_t len = 1;
			for (size_t i = 0; i < value.size(); ++i)
			{
				len += (::_tcslen(value[i]) + 1) * sizeof(TCHAR);
			}
			auto buffer = std::vector<BYTE>(len, 0);
			auto ptr = (LPTSTR)&buffer[0];
			for (size_t i = 0; i < value.size(); ++i)
			{
				::_tcscpy(ptr, value[i]);
				ptr += ::_tcslen(value[i]) + 1;
			}
			return this->SetValue(name, buffer, RegistryValueKind::MultiString);
		}
		bool RegistryKey::SetStringInternal(LPCTSTR name, LPCTSTR value, bool isExpandString) const
		{
			auto valueKind = isExpandString ? RegistryValueKind::ExpandString : RegistryValueKind::String;
			return this->SetValueInternal(name, valueKind, (LPBYTE)value, (DWORD)((::_tcslen(value) + 1) * sizeof(TCHAR)));
		}
		bool RegistryKey::SetValueInternal(LPCTSTR name, RegistryValueKind valueKind, LPBYTE value, DWORD size) const
		{
			this->EnsureWritable();
			auto result = ::RegSetValueEx(this->handle, name, 0, (DWORD)valueKind, value, size);
			return result == ERROR_SUCCESS;
		}*/

		//--------------------------------------------------------------------- GetMultiString
		/*
		std::vector<LPCTSTR> RegistryKey::GetMultiString(LPCTSTR name, const std::vector<LPCTSTR> &defaultValue) const
		{
			RegistryValueKind valueKind;
			auto value = this->GetValue(name, valueKind);

			if (valueKind != RegistryValueKind::Unknown || value.size() != 0)
			{
				if (valueKind != RegistryValueKind::MultiString)
					throw RegistryException(_T("Registry value is not a multi string"));

				if (value.size() > 0)
				{
					std::vector<LPCTSTR> result;
					auto temp = (LPTSTR)&value[0];
					size_t index = 0;
					size_t len = ::_tcslen(temp);
					while (len > 0)
					{
						result.push_back(&temp[index]);
						index += len + 1;
						len = ::_tcslen(&temp[index]);
					}
					return result;
				}
			}
			return std::vector<LPCTSTR>(defaultValue.begin(), defaultValue.end());
		}

		//--------------------------------------------------------------------- GetBinary

		std::vector<BYTE> RegistryKey::GetBinary(LPCTSTR name, const std::vector<BYTE> &defaultValue) const
		{
			RegistryValueKind valueKind;
			auto value = this->GetValue(name, valueKind);

			if (valueKind != RegistryValueKind::Unknown || value.size() != 0)
			{
				if (valueKind != RegistryValueKind::Binary)
					throw RegistryException(_T("Registry value is not binary"));

				return value;
			}
			return defaultValue;
		}
*/
		//--------------------------------------------------------------------- GetValue

		std::vector<BYTE> RegistryKey::GetValue(std::wstring name, RegistryValueKind &valueKind) const
		{
			this->EnsureValid();
			auto nameW = name.c_str();
			DWORD size;
			auto result = ::RegQueryValueExW(this->handle, nameW, nullptr, (LPDWORD)&valueKind, nullptr, &size);
			if (result == ERROR_SUCCESS)
			{
				auto buffer = std::vector<BYTE>(size);
				result = ::RegQueryValueExW(this->handle, nameW, nullptr, (LPDWORD)&valueKind, &buffer[0], &size);
				if (result == ERROR_SUCCESS)
				{
					return buffer;
				}
				else
				{
					throw RegistryException(result, _T("Error during second call to RegQueryValueEx\n"));
				}
			}
			else
			{
				throw RegistryException(result, _T("Error during first call to RegQueryValueEx\n"));
			}
		}

		RegistryView RegistryKey::ComputeRegistryView(int newView) const
		{
			if (newView == 0) // default
			{
				return this->regView;
			}
			else if (newView == 1) //x64
			{
				return RegistryView::Registry64;
			}
			else if (newView == 2) //x86
			{
				return RegistryView::Registry32;
			}

			throw RegistryException(_T("Registry view is not supported."));
		}

		//--------------------------------------------------------------------- Exceptions

		void RegistryKey::EnsureValid() const
		{
			if (!this->IsValid())
				throw RegistryException(_T("Registry key not valid."));
		}
		void RegistryKey::EnsureWritable() const
		{
			this->EnsureValid();
			if (!this->IsWritable())
				throw RegistryException(_T("Registry key not writable."));
		}
	}
}
