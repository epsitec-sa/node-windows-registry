const registryAddon = require("bindings")("registry.node");

const hives = {
  HKEY_LOCAL_MACHINE: 0x80000002,
  HKEY_CURRENT_USER: 0x80000001,
  HKEY_USERS: 0x80000003,
  HKEY_CLASSES_ROOT: 0x80000000,
};

function _openHive(options) {
  const hive =
    options && options.hive ? options.hive : hives.HKEY_LOCAL_MACHINE;
  const isWritableDefined =
    options && options.writable !== null && options.writable !== undefined;

  if (isWritableDefined) {
    return registryAddon.OpenHive(hive, options.writable);
  } else {
    return registryAddon.OpenHive(hive);
  }
}

function openKey(name, options) {
  const hiveKey = _openHive(options);

  return hiveKey;
}

module.exports = {
  openKey,
  hives,
};
