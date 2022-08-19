//#define NAPI_VERSION 4

#include "registry.h"

bool getDefaultIsWritable(const int hive)
{
  switch (hive)
  {
  case 2:
    return false;
  case 1:
    return true;
  case 3:
    return true;
  case 0:
    return false;
  default:
    return false;
  }
}

HKEY getHandle(const int hive)
{
  switch (hive)
  {
  case 2:
    return HKEY_LOCAL_MACHINE;
  case 1:
    return HKEY_CURRENT_USER;
  case 3:
    return HKEY_USERS;
  case 0:
    return HKEY_CLASSES_ROOT;
  default:
    return HKEY_LOCAL_MACHINE;
  }
}

RegistryKeyWrapper::RegistryKeyWrapper(const Napi::CallbackInfo &info)
    : ObjectWrap(info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsExternal())
  {
    Napi::TypeError::New(env, "Wrong argument type")
        .ThrowAsJavaScriptException();
    return;
  }

  this->_registryKey = info[0].As<Napi::External<RegistryKey>>().Data();
}

// string name, bool writable -> RegistryKeyWrapper
Napi::Value RegistryKeyWrapper::OpenSubkey(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "Value name should be a string")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    if (!info[1].IsBoolean())
    {
      Napi::TypeError::New(env, "Writable should be a boolean")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    auto name = info[0].As<Napi::String>().Utf8Value().c_str();
    auto writable = info[1].As<Napi::Boolean>().Value();

    if (this->_registryKey->handle != HKEY_LOCAL_MACHINE)
    {
      Napi::TypeError::New(env, "Hive handle is not valid")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    auto subKey = this->_registryKey->OpenSubkey(name, writable);

    if (!subKey.IsValid())
    {
      Napi::TypeError::New(env, "Subkey does not exist")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    return RegistryKeyWrapper::NewInstance(info.Env(), Napi::External<RegistryKey>::New(env, &subKey));
  }
  catch (const RegistryException &e)
  {
    Napi::TypeError::New(env, e.Message())
        .ThrowAsJavaScriptException();
    return env.Null();
  }
}

// string name -> any
Napi::Value RegistryKeyWrapper::GetValue(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "Value name should be a string")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    auto name = info[0].As<Napi::String>().Utf8Value().c_str();

    auto value = this->_registryKey->GetStringOrExpandString(name);

    return Napi::String::New(env, value);
  }
  catch (const RegistryException &e)
  {
    Napi::TypeError::New(env, e.Message())
        .ThrowAsJavaScriptException();
    return env.Null();
  }
}

// int hive, bool writable -> RegistryKeyWrapper
Napi::Value OpenHive(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {

    unsigned int result = 0;

    if (info.Length() < 1 || info.Length() > 2)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    if (info.Length() == 1 && !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() == 2 && (!info[0].IsNumber() || !info[1].IsBoolean()))
    {
      Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto hiveP = info[0].As<Napi::Number>().Int32Value();
    auto hive = getHandle(hiveP);
    auto isWritableDefined = info.Length() == 2;
    auto isWritable = isWritableDefined
                          ? info[1].As<Napi::Boolean>().Value()
                          : false;

    auto registryKey = isWritableDefined
                           ? new RegistryKey(hive, isWritable, true)
                           : new RegistryKey(hive, getDefaultIsWritable(hiveP), true);

    if (!registryKey->IsValid())
    {
      Napi::TypeError::New(env, "Hive is not valid")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    return RegistryKeyWrapper::NewInstance(info.Env(), Napi::External<RegistryKey>::New(env, registryKey));
  }
  catch (const RegistryException &e)
  {
    Napi::TypeError::New(env, e.Message())
        .ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Function RegistryKeyWrapper::GetClass(Napi::Env env)
{
  return DefineClass(
      env,
      "RegistryKeyWrapper",
      {
          RegistryKeyWrapper::InstanceMethod("openSubkey", &RegistryKeyWrapper::OpenSubkey),
          //RegistryKeyWrapper::InstanceMethod("subkeyNames", &RegistryKeyWrapper::SubkeyNames),
          //RegistryKeyWrapper::InstanceMethod("valueNames", &RegistryKeyWrapper::ValueNames),
          RegistryKeyWrapper::InstanceMethod("getValue", &RegistryKeyWrapper::GetValue),
      });
}

Napi::Object RegistryKeyWrapper::NewInstance(Napi::Env env, Napi::Value arg)
{
  Napi::EscapableHandleScope scope(env);
  Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({arg});
  return scope.Escape(napi_value(obj)).ToObject();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Napi::String name = Napi::String::New(env, "RegistryKeyWrapper");
  auto classFunc = RegistryKeyWrapper::GetClass(env);
  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(classFunc);
  env.SetInstanceData(constructor);
  exports.Set(name, classFunc);

  exports.Set(Napi::String::New(env, "openHive"), Napi::Function::New(env, OpenHive));

  return exports;
}

NODE_API_MODULE(addon, Init)