{
  "targets": [
    {
      "target_name": "registry",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs": [
          "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "sources": [ "./src/registry.cpp", "./src/registry/RegistryKey.cpp" ],
      "libraries": [],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}