var util = require('util');
var events = require('events');

var db_data = [
    {id:1,name:"CPSAP"},
    {id:2,name:"OS"},
    {id:3,name:"SP"},
    {id:4,name:"NAP"},
    {id:5,name:"CGB"},
    {id:6,name:"CMS"}
];


function DB(){
    this.get = ()=>{return db_data;};
    this.post = (data_piece)=>{db_data.push(data_piece);};
    this.post = (to_update)=>{
        const index = db_data.findIndex(item=>item.id==to_update.id);
        if(index!=-1){
            db_data[index] = {...db_data[index],...to_update.name};
        }
    };
}

util.inherits(DB,events.EventEmitter);

exports.DB = DB;