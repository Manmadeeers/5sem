
const normalize = (v) => {
    if (v === undefined) {
        return [];
    }
    return Array.isArray(v) ? v : [v];
}

module.exports = {normalize};