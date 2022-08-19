/*global describe,it*/

const assert = require("assert");
const lib = require("../");

describe("OpenKey", function () {
  it("should open a key", function () {
    const key = lib.openKey("SOFTWARE\\Microsoft", {
      hive: lib.hives.HKEY_LOCAL_MACHINE,
    });

    assert.ok(key);
  });
});
