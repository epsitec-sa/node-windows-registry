{
  "targets": [
    {
      "target_name": "registry",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "include_dirs": [
          "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "sources": [ "./src/messaging.cpp", "./src/wipc/Module.cpp", "./src/wipc/Window.cpp", "./src/wipc/WipcUtf8Listener.cpp" ],
      "libraries": [],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}