#pragma once

#include <napi.h>

#include "registry/Registry.h"

using namespace Napi;
using namespace Epsitec::Win32;

class RegistryKeyWrapper : public Napi::ObjectWrap<RegistryKeyWrapper>
{
public:
    RegistryKeyWrapper(const Napi::CallbackInfo &);

    Napi::Value OpenSubkey(const Napi::CallbackInfo &);
    //Napi::Value SubkeyNames(const Napi::CallbackInfo &);
    //Napi::Value ValueNames(const Napi::CallbackInfo &);
    Napi::Value GetValue(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);
    static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);

    RegistryKey *_registryKey;
};