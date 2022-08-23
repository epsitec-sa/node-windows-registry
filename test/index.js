/*global describe,it*/

const assert = require("assert");
const lib = require("../");

describe("OpenKey", function () {
  it("should open a key", function (done) {
    lib.openKey(
      "SOFTWARE\\Epsitec\\Cresus Monolith\\Setup",
      {
        hive: lib.HKEY_LOCAL_MACHINE,
      },
      (err, key) => {
        if (err) {
          done(err);
        } else {
          assert.ok(key);
          assert.ok(key._registryKey);

          key.dispose();
          done();
        }
      }
    );
  });
});

describe("GetValue", function () {
  it("should open a key and get a value", function (done) {
    lib.openKey(
      "SOFTWARE\\Epsitec\\Cresus Monolith\\Setup",
      {
        hive: lib.HKEY_LOCAL_MACHINE,
      },
      (err, key) => {
        if (err) {
          done(err);
        } else {
          assert.ok(key);
          assert.ok(key._registryKey);

          key.getValue("InstallDir", (err2, installDir) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(installDir, "C:\\Program Files\\Cresus");

              key.dispose();
              done();
            }
          });
        }
      }
    );
  });
  it("should open a key and get a value with accent", function (done) {
    lib.openKey(
      "SOFTWARE\\Epsitec\\Cresus Monolith\\Tests",
      {
        hive: lib.HKEY_LOCAL_MACHINE,
      },
      (err, key) => {
        if (err) {
          done(err);
        } else {
          assert.ok(key);
          assert.ok(key._registryKey);

          key.getValue("accént", (err2, accent) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(accent, "no accent");

              key.dispose();
              done();
            }
          });
        }
      }
    );
  });
});

describe("ListValues", function () {
  it("should open a key and list its values", function (done) {
    lib.openKey(
      "SOFTWARE\\Epsitec\\Cresus Monolith\\Setup",
      {
        hive: lib.HKEY_LOCAL_MACHINE,
      },
      (err, key) => {
        if (err) {
          done(err);
        } else {
          assert.ok(key);
          assert.ok(key._registryKey);

          key.listValues((err2, values) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(!!values.InstallDir, true);
              assert.equal(values.InstallDir, "C:\\Program Files\\Cresus");

              key.dispose();
              done();
            }
          });
        }
      }
    );
  });
});

describe("ListSubkeys", function () {
  it("should open a key and list its subkeys", function (done) {
    lib.openKey(
      "SOFTWARE\\Epsitec\\Cresus Monolith",
      {
        hive: lib.HKEY_LOCAL_MACHINE,
      },
      (err, key) => {
        if (err) {
          done(err);
        } else {
          assert.ok(key);
          assert.ok(key._registryKey);

          key.listSubkeys((err2, subkeys) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(subkeys.includes("Setup"), true);
              key.dispose();
              done();
            }
          });
        }
      }
    );
  });
});
