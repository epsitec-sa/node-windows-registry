const registryAddon = require("bindings")("registry.node");

const hives = {
  HKEY_LOCAL_MACHINE: 2,
  HKLM: 2,
  HKEY_CURRENT_USER: 1,
  HKCU: 1,
  HKEY_USERS: 3,
  HKU: 3,
  HKEY_CLASSES_ROOT: 0,
  HKCR: 0,
};

const views = {
  defaultView: 0,
  x64: 1,
  x86: 2,
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
  return _isNativeErrorWithCode(err, errors.ItemNotFound);
}

class RegistryKey {
  constructor(_registryKey) {
    this._registryKey = _registryKey;
  }

  dispose() {
    this._registryKey.close();
  }

  _getValue(name) {
    const value = this._registryKey.getValue(name);
    return value && typeof value === "string"
      ? value.replace(/\0/g, "")
      : value;
  }

  openSubkey(name, options, cb) {
    setImmediate(
      (name, options) => {
        try {
          const writable =
            options &&
            options.writable !== undefined &&
            options.writable !== null
              ? options.writable
              : false;
          const view =
            options && options.view !== undefined && options.view !== null
              ? options.view
              : views.defaultView;
          cb(
            null,
            new RegistryKey(this._registryKey.openSubkey(name, writable, view))
          );
        } catch (err) {
          cb(err);
        }
      },
      name,
      options
    );
  }

  getValue(name, cb) {
    setImmediate((name) => {
      try {
        cb(null, this._getValue(name));
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
          values[valueName.replace(/\0/g, "")] = this._getValue(valueName);
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
        const subkeys = [];
        const subkeyNames = this._registryKey.subkeyNames();
        for (let subkeyName of subkeyNames) {
          subkeys.push(subkeyName.replace(/\0/g, ""));
        }

        cb(null, subkeys);
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
  const isViewDefined =
    options && options.view !== null && options.view !== undefined;

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
          {
            writable: isWritableDefined ? options.writable : false,
            view: isViewDefined ? options.view : views.defaultView,
          },
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
  isNotFoundError,
  ...hives,
  ...views,
};
