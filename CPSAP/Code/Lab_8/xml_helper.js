function ensureArray(v) {
  if (v === undefined || v === null) return [];

  return Array.isArray(v) ? v : [v];

}

module.exports = {ensureArray};