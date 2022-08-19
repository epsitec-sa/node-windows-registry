const registryAddon = require("bindings")("registry.node");

const hives = {
  HKEY_LOCAL_MACHINE: 2,
  HKEY_CURRENT_USER: 1,
  HKEY_USERS: 3,
  HKEY_CLASSES_ROOT: 0,
};

class RegistryKey {
  constructor(_registryKey) {
    this._registryKey = _registryKey;
  }

  getValue(name) {
    return this._registryKey.getValue(name);
  }
}

function _openHive(hive, isWritableDefined, writable) {
  if (isWritableDefined) {
    return registryAddon.openHive(hive, writable);
  } else {
    return registryAddon.openHive(hive);
  }
}

function openKey(name, options) {
  // todo: handle name starting with hive
  const hive =
    options && options.hive ? options.hive : hives.HKEY_LOCAL_MACHINE;
  const isWritableDefined =
    options && options.writable !== null && options.writable !== undefined;

  const hiveKey = _openHive(
    hive,
    isWritableDefined,
    isWritableDefined ? options.writable : null
  );

  return new RegistryKey(
    hiveKey.openSubkey(name, isWritableDefined ? options.writable : false)
  );
}

module.exports = {
  openKey,
  ...hives,
};
