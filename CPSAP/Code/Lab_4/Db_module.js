var util = require('util');
var events = require('events');

var db_data = [
    { id: 1, name: "Name1", bday: "03-10-2025" },
    { id: 2, name: "Name2", bday: "02-10-2025" },
    { id: 3, name: "Name3", bday: "01-10-2025" }
];


function DB() {
    this.select = () => {
        return db_data;
    };

    this.insert = (data_piece) => {
        let found_id

        if (data_piece.id == null) {
            console.log("ID was null");
            found_id = 1;
        }
        else {
            found_id = parseInt(data_piece.id);
        }

        while (true) {
            if (db_data.findIndex(item => item.id == found_id) != -1) {
                found_id = found_id + 1;
            }
            else {
                break;
            }
        }
        let refreshed_piece = { id: found_id, name: data_piece.name, bday: data_piece.bday }
        db_data.push(refreshed_piece);
    };

    this.update = (to_update) => {
        const index = db_data.findIndex(item => item.id == to_update.id);
        if (index === -1) {
            return false;
        }
        db_data[index] = to_update;
        return true;
    };

    this.delete = (id) => {
        console.log(`Transmitted id: ${parseInt(id)}`);
        const index = db_data.findIndex(el => el.id == id)
        if (index === -1) {
            return false;
        }
        const deletedRow = db_data.splice(index, 1)[0];
        return deletedRow;
    }
}

util.inherits(DB, events.EventEmitter);

exports.DB = DB;