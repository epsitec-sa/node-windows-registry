//	Copyright @ 2013-2022, EPSITEC SA, CH-1400 Yverdon-les-Bains, Switzerland
//	Author: Roger Vuistiner

#pragma once

#include <string>
#include <tchar.h>

#include <array>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <windows.h>
#include <WinDNS.h>

namespace Epsitec
{
	namespace Win32
	{

		//--------------------------------------------------------------------- RegistryView

		enum class RegistryView
		{
			Default,
			Registry64 = KEY_WOW64_64KEY,
			Registry32 = KEY_WOW64_32KEY
		};

		//--------------------------------------------------------------------- RegistryOptions

		enum class RegistryOptions
		{
			None = REG_OPTION_NON_VOLATILE,
			Volatile = REG_OPTION_VOLATILE
		};

		//--------------------------------------------------------------------- RegistryValueKind

		enum class RegistryValueKind
		{
			None = -1,
			Unknown = REG_NONE,
			String = REG_SZ,
			ExpandString = REG_EXPAND_SZ,
			Binary = REG_BINARY,
			DWord = REG_DWORD,
			MultiString = REG_MULTI_SZ,
			QWord = REG_QWORD,
		};

		//--------------------------------------------------------------------- RegistryException

		class RegistryException
		{
		public:
			RegistryException(LPCTSTR message) { this->message = message; }
			LPCTSTR Message() const { return this->message; }

		private:
			LPCTSTR message;
		};

		//--------------------------------------------------------------------- RegistryKey

		class RegistryKey final
		{
			friend class Registry;

		public:
			RegistryKey(RegistryKey &&key);
			~RegistryKey();
			RegistryKey() : RegistryKey(nullptr) {}
			RegistryKey(HKEY handle, bool writable = false, bool systemKey = false, RegistryView registryView = RegistryView::Default);
			RegistryKey(const RegistryKey &key) = delete;
			RegistryKey &operator=(const RegistryKey &key) = delete;

		public:
			HKEY handle;
			bool IsValid() const { return this->handle != nullptr; }
			bool IsSystemKey() const { return (this->state & 2) != 0; }
			bool IsWritable() const { return (this->state & 4) != 0; }
			int SubkeyCount(LPDWORD maxSubkeyLength = nullptr) const;
			int ValueCount(LPDWORD maxValueLength = nullptr) const;
			std::vector<LPCTSTR> SubkeyNames() const;
			std::vector<LPCTSTR> ValueNames() const;
			RegistryKey CreateSubkey(LPCTSTR subkey, bool writable, RegistryOptions options = RegistryOptions::None) const;
			RegistryKey OpenSubkey(LPCTSTR name, bool writable) const;
			bool DeleteSubkey(LPCTSTR subkey = nullptr) const;
			bool DeleteSubkeyTree(LPCTSTR name = nullptr) const;
			bool DeleteValue(LPCTSTR name) const;

			bool SetInt(LPCTSTR name, int value) const { return this->SetDWord(name, (DWORD)value); }
			bool SetUInt(LPCTSTR name, unsigned int value) const { return this->SetDWord(name, (DWORD)value); }
			bool SetLong(LPCTSTR name, long value) const { return this->SetDWord(name, (DWORD)value); }
			bool SetULong(LPCTSTR name, unsigned long value) const { return this->SetDWord(name, (DWORD)value); }
			bool SetLLong(LPCTSTR name, long long value) const { return this->SetQWord(name, (QWORD)value); }
			bool SetULLong(LPCTSTR name, unsigned long long value) const { return this->SetQWord(name, (QWORD)value); }
			bool SetDWord(LPCTSTR name, DWORD value) const { return this->SetValueInternal(name, RegistryValueKind::DWord, (LPBYTE)&value, sizeof(DWORD)); }
			bool SetQWord(LPCTSTR name, QWORD value) const { return this->SetValueInternal(name, RegistryValueKind::QWord, (LPBYTE)&value, sizeof(QWORD)); }
			bool SetString(LPCTSTR name, LPCTSTR value) const { return this->SetStringInternal(name, value, false); }
			bool SetExpandString(LPCTSTR name, LPCTSTR value) const { return this->SetStringInternal(name, value, true); }
			bool SetMultiString(LPCTSTR name, std::vector<LPCTSTR> value) const;
			bool SetBinary(LPCTSTR name, std::vector<BYTE> value) const { return this->SetValueInternal(name, RegistryValueKind::Binary, &value[0], (DWORD)value.size()); }
			bool SetValue(LPCTSTR name, std::vector<BYTE> value, RegistryValueKind valueKind) const { return this->SetValueInternal(name, valueKind, &value[0], (DWORD)value.size()); }

			RegistryValueKind GetValueKind(LPCTSTR name) const;
			int GetInt(LPCTSTR name) const { return (int)this->GetDWord(name, 0); }
			int GetInt(LPCTSTR name, int defaultValue) const { return (int)this->GetDWord(name, defaultValue); }
			unsigned int GetUInt(LPCTSTR name) const { return (unsigned int)this->GetDWord(name, 0); }
			unsigned int GetUInt(LPCTSTR name, unsigned int defaultValue) const { return (unsigned int)this->GetDWord(name, defaultValue); }
			long GetLong(LPCTSTR name) const { return (long)this->GetDWord(name, 0); }
			long GetLong(LPCTSTR name, long defaultValue) const { return (long)this->GetDWord(name, defaultValue); }
			unsigned long GetULong(LPCTSTR name) const { return (unsigned long)this->GetDWord(name, 0); }
			unsigned long GetULong(LPCTSTR name, unsigned long defaultValue) const { return (unsigned long)this->GetDWord(name, defaultValue); }
			long long GetLLong(LPCTSTR name) const { return (long long)this->GetQWord(name, 0); }
			long long GetLLong(LPCTSTR name, long long defaultValue) const { return (long long)this->GetQWord(name, defaultValue); }
			unsigned long long GetULLong(LPCTSTR name) const { return (unsigned long long)this->GetQWord(name, 0); }
			unsigned long long GetULLong(LPCTSTR name, unsigned long long defaultValue) const { return (unsigned long long)this->GetQWord(name, defaultValue); }
			DWORD GetDWord(LPCTSTR name) const { return this->GetDWord(name, 0); }
			DWORD GetDWord(LPCTSTR name, DWORD defaultValue) const;
			QWORD GetQWord(LPCTSTR name) const { return this->GetQWord(name, 0); }
			QWORD GetQWord(LPCTSTR name, QWORD defaultValue) const;
			LPCTSTR GetString(LPCTSTR name) const { return this->GetString(name, _T("")); }
			LPCTSTR GetString(LPCTSTR name, LPCTSTR defaultValue) const;
			LPCTSTR GetExpandString(LPCTSTR name) const { return this->GetExpandString(name, _T("")); }
			LPCTSTR GetExpandString(LPCTSTR name, LPCTSTR defaultValue) const;
			LPCTSTR GetStringOrExpandString(LPCTSTR name) const
			{
				RegistryValueKind valueKind;
				return this->GetStringOrExpandString(name, _T(""), valueKind);
			}
			LPCTSTR GetStringOrExpandString(LPCTSTR name, LPCTSTR defaultValue) const
			{
				RegistryValueKind valueKind;
				return this->GetStringOrExpandString(name, defaultValue, valueKind);
			}
			LPCTSTR GetStringOrExpandString(LPCTSTR name, RegistryValueKind &valueKind) const { return this->GetStringOrExpandString(name, _T(""), valueKind); }
			LPCTSTR GetStringOrExpandString(LPCTSTR name, LPCTSTR defaultValue, RegistryValueKind &valueKind) const;
			std::vector<LPCTSTR> GetMultiString(LPCTSTR name) const { return this->GetMultiString(name, std::vector<LPCTSTR>()); }
			std::vector<LPCTSTR> GetMultiString(LPCTSTR name, std::vector<LPCTSTR> &&defaultValue) const { return this->GetMultiString(name, defaultValue); }
			std::vector<LPCTSTR> GetMultiString(LPCTSTR name, const std::vector<LPCTSTR> &defaultValue) const;
			std::vector<BYTE> GetBinary(LPCTSTR name) const { return this->GetBinary(name, std::vector<BYTE>()); }
			std::vector<BYTE> GetBinary(LPCTSTR name, std::vector<BYTE> &&defaultValue) const { return this->GetBinary(name, defaultValue); }
			std::vector<BYTE> GetBinary(LPCTSTR name, const std::vector<BYTE> &defaultValue) const;
			std::vector<BYTE> GetValue(LPCTSTR name, RegistryValueKind &valueKind) const;
			std::vector<BYTE> GetValue(LPCTSTR name, std::vector<BYTE> &&defaultValue, RegistryValueKind &valueKind) const;
			std::vector<BYTE> GetValue(LPCTSTR name, const std::vector<BYTE> &defaultValue, RegistryValueKind &valueKind) const;

		private:
			REGSAM AccessMask(bool writable) const { return RegistryKey::AccessMask(this->IsWritable(), this->regView); }
			HKEY DetachHandle() { return reinterpret_cast<HKEY>(::InterlockedExchangePointer(reinterpret_cast<void **>(&this->handle), nullptr)); }
			bool DeleteSubkeyTreeInternal(LPCTSTR name) const;
			void EnsureValid() const;
			void EnsureWritable() const;
			bool SetValueInternal(LPCTSTR name, RegistryValueKind valueKind, LPBYTE value, DWORD size) const;
			bool SetStringInternal(LPCTSTR name, LPCTSTR value, bool isExpandString) const;

		private:
			static REGSAM AccessMask(bool writable, RegistryView regView) { return (writable ? KEY_WRITE | KEY_READ : KEY_READ) | (REGSAM)regView; }

		private:
			int state;
			RegistryView regView;
		};
	}
}
