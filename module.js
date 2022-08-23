const registryAddon = require("bindings")("registry.node");

const hives = {
  HKEY_LOCAL_MACHINE: 2,
  HKEY_CURRENT_USER: 1,
  HKEY_USERS: 3,
  HKEY_CLASSES_ROOT: 0,
};

const errors = {
  ItemNotFound: 2,
};

function _isNativeErrorWithCode(err, code) {
  if (err && err.message) {
    try {
      const errValue = parseInt(err.message);
      return errValue === code;
    } catch (err2) {
      return false;
    }
  }

  return false;
}

function _openHive(hive, isWritableDefined, writable, cb) {
  setImmediate(
    (hive, isWritableDefined, writable) => {
      try {
        if (isWritableDefined) {
          return cb(
            null,
            new RegistryKey(registryAddon.openHive(hive, writable))
          );
        } else {
          return cb(null, new RegistryKey(registryAddon.openHive(hive)));
        }
      } catch (err) {
        cb(err);
      }
    },
    hive,
    isWritableDefined,
    writable
  );
}

function isNotFoundError(err) {
  return _isNativeErrorWithCode(errors.ItemNotFound);
}

class RegistryKey {
  constructor(_registryKey) {
    this._registryKey = _registryKey;
  }

  dispose() {
    this._registryKey.close();
  }

  openSubkey(name, writable, cb) {
    setImmediate(
      (name, writable) => {
        try {
          cb(
            null,
            new RegistryKey(this._registryKey.openSubkey(name, writable))
          );
        } catch (err) {
          cb(err);
        }
      },
      name,
      writable
    );
  }

  getValue(name, cb) {
    setImmediate((name) => {
      try {
        cb(null, this._registryKey.getValue(name));
      } catch (err) {
        cb(err);
      }
    }, name);
  }

  listValues(cb) {
    setImmediate(() => {
      try {
        const values = {};
        const valueNames = this._registryKey.valueNames();
        for (let valueName of valueNames) {
          values[valueName] = this._registryKey.getValue(valueName);
        }

        cb(null, values);
      } catch (err) {
        cb(err);
      }
    });
  }

  listSubkeys(cb) {
    setImmediate(() => {
      try {
        cb(null, this._registryKey.subkeyNames());
      } catch (err) {
        cb(err);
      }
    });
  }
}

function openKey(name, options, cb) {
  // todo: handle name starting with hive
  const hive =
    options && options.hive ? options.hive : hives.HKEY_LOCAL_MACHINE;
  const isWritableDefined =
    options && options.writable !== null && options.writable !== undefined;

  _openHive(
    hive,
    isWritableDefined,
    isWritableDefined ? options.writable : null,
    (err, hiveKey) => {
      if (err) {
        cb(err);
      } else {
        hiveKey.openSubkey(
          name,
          isWritableDefined ? options.writable : false,
          (err2, subkey) => {
            try {
              hiveKey.dispose();
              if (err2) {
                cb(err2);
              } else {
                cb(null, subkey);
              }
            } catch (err3) {
              cb(err3);
            }
          }
        );
      }
    }
  );
}

module.exports = {
  openKey,
  ...hives,
  isNotFoundError,
};
