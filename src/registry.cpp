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

Napi::Value handleRegistryException(const RegistryException &e, const Napi::Env &env)
{
  if (e.Code() != -1)
  {
    Napi::TypeError::New(env, std::to_string(e.Code()).c_str())
        .ThrowAsJavaScriptException();
  }
  else
  {
    Napi::TypeError::New(env, e.Message())
        .ThrowAsJavaScriptException();
  }

  return env.Null();
}

Napi::Value readValue(const RegistryKey *registryKey, LPCTSTR name, const Napi::Env &env)
{
  RegistryValueKind valueKind;
  auto value = registryKey->GetValue(name, valueKind);

  if (value.size() == 0)
  {
    return env.Null();
  }

  switch (valueKind)
  {
  case RegistryValueKind::String:
  case RegistryValueKind::ExpandString:
    return Napi::String::New(env, (LPCTSTR)&value[0]);
  case RegistryValueKind::Binary:
    printf("Warning: reading binary registry values is not yet supported");
    return env.Null();
  case RegistryValueKind::DWord:
    return Napi::Number::New(env, *(PDWORD)&value[0]);
  case RegistryValueKind::MultiString:
    printf("Warning: reading multistring registry values is not yet supported");
    return env.Null();
  case RegistryValueKind::QWord:
    return Napi::Number::New(env, *(PQWORD)&value[0]);
  default:
    return env.Null();
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

Napi::Value RegistryKeyWrapper::Close(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    if (info.Length() != 0)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    this->_registryKey->Release();
    delete this->_registryKey;

    return env.Null();
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
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

    auto name = info[0].As<Napi::String>().Utf8Value();
    return readValue(this->_registryKey, name.c_str(), env);
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
  }
}

// () -> string[]
Napi::Value RegistryKeyWrapper::SubkeyNames(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    if (info.Length() != 0)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    auto subkeyNames = this->_registryKey->SubkeyNames();
    auto subkeyNamesArray = Napi::Array::New(env, subkeyNames.size());

    for (int i = 0; i < subkeyNames.size(); i++)
    {
      subkeyNamesArray[i] = Napi::String::New(env, subkeyNames[i].c_str());
    }

    return subkeyNamesArray;
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
  }
}

// () -> string[]
Napi::Value RegistryKeyWrapper::ValueNames(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    if (info.Length() != 0)
    {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    auto valueNames = this->_registryKey->ValueNames();
    auto valueNamesArray = Napi::Array::New(env, valueNames.size());

    for (int i = 0; i < valueNames.size(); i++)
    {
      valueNamesArray[i] = Napi::String::New(env, valueNames[i].c_str());
    }

    return valueNamesArray;
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
  }
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

    auto subKey = this->_registryKey->OpenSubkey(name, writable);

    if (!subKey.IsValid())
    {
      Napi::TypeError::New(env, "Subkey is invalid")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    return RegistryKeyWrapper::NewInstance(info.Env(), Napi::External<RegistryKey>::New(env, subKey.Malloc()));
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
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
                           ? RegistryKey(hive, isWritable, true)
                           : RegistryKey(hive, getDefaultIsWritable(hiveP), true);

    if (!registryKey.IsValid())
    {
      Napi::TypeError::New(env, "Hive is not valid")
          .ThrowAsJavaScriptException();
      return env.Null();
    }

    return RegistryKeyWrapper::NewInstance(info.Env(), Napi::External<RegistryKey>::New(env, registryKey.Malloc()));
  }
  catch (const RegistryException &e)
  {
    return handleRegistryException(e, env);
  }
}

Napi::Function RegistryKeyWrapper::GetClass(Napi::Env env)
{
  return DefineClass(
      env,
      "RegistryKeyWrapper",
      {
          RegistryKeyWrapper::InstanceMethod("openSubkey", &RegistryKeyWrapper::OpenSubkey),
          RegistryKeyWrapper::InstanceMethod("subkeyNames", &RegistryKeyWrapper::SubkeyNames),
          RegistryKeyWrapper::InstanceMethod("valueNames", &RegistryKeyWrapper::ValueNames),
          RegistryKeyWrapper::InstanceMethod("getValue", &RegistryKeyWrapper::GetValue),
          RegistryKeyWrapper::InstanceMethod("close", &RegistryKeyWrapper::Close),
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