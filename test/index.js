/*global describe,it*/

const assert = require("assert");
const { expect } = require("chai");
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
  it("should throw on openning an inexistent key", async function () {
    try {
      await lib.openKey("SOFTWARE\\Epsitec\\Cresus Monolith\\Setup XYZ", {
        hive: lib.HKEY_LOCAL_MACHINE,
      });
    } catch (err) {
      expect(err).to.be.instanceOf(TypeError);
      assert.equal(lib.isNotFoundError(err), true);
    }
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
  it("should open a key and get a value with an accent in the name", function (done) {
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
  it("should open a key and get a value with accent in value", function (done) {
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

          key.getValue("Test", (err2, accent) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(accent, "Progràm Filés");

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
  it("should open a key and list its values with accent", function (done) {
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

          key.listValues((err2, values) => {
            if (err2) {
              done(err2);
            } else {
              assert.equal(!!values["accént"], true);
              assert.equal(values.Test, "Progràm Filés");

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
