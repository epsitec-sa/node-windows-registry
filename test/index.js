/*global describe,it*/

const assert = require("assert");
const lib = require("../");

describe("OpenKey", function () {
  it("should open a key", function () {
    const key = lib.openKey("SOFTWARE\\Epsitec\\Cresus Monolith\\Setup", {
      hive: lib.HKEY_LOCAL_MACHINE,
    });

    assert.ok(key);
    assert.ok(key._registryKey);
  });
});

describe("GetValue", function () {
  it("should open a key and get a value", function () {
    const key = lib.openKey("SOFTWARE\\Epsitec\\Cresus Monolith\\Setup", {
      hive: lib.HKEY_LOCAL_MACHINE,
    });

    assert.ok(key);
    assert.ok(key._registryKey);

    const installDir = key.getValue("InstallDir");
    assert.equal(installDir, "C:\\Program Files\\Cresus");
  });
});
