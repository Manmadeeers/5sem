var util = require('util');
var events = require('events');

var db_data = [
    {id:1,name:"Name1",bday:"03-10-2025"},
    {id:2,name:"Name2",bday:"02-10-2025"},
    {id:3,name:"Name3",bday:"01-10-2025"}
];


function DB(){
    this.select = ()=>{
        return db_data;
    };
    this.insert = (data_piece)=>{
        db_data.push(data_piece);
    };
    this.update= (to_update)=>{
        const index = db_data.findIndex(item=>item.id==to_update.id);
        if(index===-1){
           return false;
        }
        db_data[index] = to_update;
        return true;
    };
    this.delete = (id)=>{
        console.log(`Transmitted id: ${parseInt(id)}`);
        const index=db_data.findIndex(el=>el.id==id)
        if(index===-1){
            return false;
        }
        const deletedRow =db_data.splice(index,1)[0];
        return deletedRow;
    }
}

util.inherits(DB,events.EventEmitter);

exports.DB = DB;