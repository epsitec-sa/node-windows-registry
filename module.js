const registryAddon = require("bindings")("registry.node");

const hives = {
  HKEY_LOCAL_MACHINE: 2,
  HKEY_CURRENT_USER: 1,
  HKEY_USERS: 3,
  HKEY_CLASSES_ROOT: 0,
};

const errors = {
  Unknown: -1,
  ItemNotFound: 2,
};

function parseNativeError(err) {
  if (err.message) {
    try {
      const errValue = parseInt(err.message);
      return {
        code: errValue,
      };
    } catch (_err) {
      return {
        code: errors.Unknown,
        message: err.message,
      };
    }
  } else {
    return err;
  }
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
          cb(parseNativeError(err));
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
        cb(parseNativeError(err));
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
        cb(parseNativeError(err));
      }
    });
  }

  listSubkeys(cb) {
    setImmediate(() => {
      try {
        cb(null, this._registryKey.subkeyNames());
      } catch (err) {
        cb(parseNativeError(err));
      }
    });
  }
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
        cb(parseNativeError(err));
      }
    },
    hive,
    isWritableDefined,
    writable
  );
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
        cb(parseNativeError(err));
      } else {
        hiveKey.openSubkey(
          name,
          isWritableDefined ? options.writable : false,
          (err2, subkey) => {
            try {
              hiveKey.dispose();
              if (err2) {
                cb(parseNativeError(err2));
              } else {
                cb(null, subkey);
              }
            } catch (err3) {
              cb(parseNativeError(err3));
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
  ...errors,
};
