//#define NAPI_VERSION 4

#include "registry.h"

bool getDefaultIsWritable(const int64_t hive)
{
  switch (hive)
  {
  case HKEY_LOCAL_MACHINE:
    return false;
  case HKEY_CURRENT_USER:
    return true;
  case HKEY_USERS:
    return true;
  case HKEY_CLASSES_ROOT:
    return false;
  default:
    return false;
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
/*
Napi::Value RegistryKeyWrapper::Greet(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() < 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsString())
  {
    Napi::TypeError::New(env, "You need to introduce yourself to greet")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::String name = info[0].As<Napi::String>();

  printf("Hello %s\n", name.Utf8Value().c_str());
  printf("I am %s\n", this->_greeterName.c_str());

  return Napi::String::New(env, this->_greeterName);
}*/

// int hive, bool writable -> RegistryKeyWrapper
Napi::Value OpenHive(const Napi::CallbackInfo &info)
{
  unsigned int result = 0;
  Napi::Env env = info.Env();

  if (info.Length() < 1 || info.Length() < 2)
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

  auto hive = info[0].As<Napi::Number>().Int64Value();
  auto isWritableDefined = info.Length() == 2;
  auto isWritable = isWritableDefined
                        ? info[1].As<Napi::Boolean>().Value()
                        : false;

  auto registryKey = isWritableDefined
                         ? RegistryKey((HKEY)hive, isWritable, true)
                         : RegistryKey((HKEY)hive, getDefaultIsWritable(hive), true);
  return env.Null();
  //return RegistryKeyWrapper::NewInstance(info.Env(), Napi::External::New(env, &registryKey));
}

Napi::Function RegistryKeyWrapper::GetClass(Napi::Env env)
{
  return DefineClass(
      env,
      "RegistryKeyWrapper",
      {
          //RegistryKeyWrapper::InstanceMethod("subkeyNames", &RegistryKeyWrapper::SubkeyNames),
          //RegistryKeyWrapper::InstanceMethod("valueNames", &RegistryKeyWrapper::ValueNames),
          //RegistryKeyWrapper::InstanceMethod("getValue", &RegistryKeyWrapper::GetValue),
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

  exports.Set(Napi::String::New(env, "OpenHive"), Napi::Function::New(env, OpenHive));

  return exports;
}

NODE_API_MODULE(addon, Init)