let http = require('http');
let url = require('url');
let { DbService } = require('./service');
let service = new DbService();
let { DB } = require('./db_module.js');
let database = new DB();

http.createServer((req, res) => {
    let pathname = url.parse(req.url).pathname;
    let path = pathname.split('/')[1] + '/' + pathname.split('/')[2];
    let code = pathname.split('/')[3];

    if(req.method === 'GET' && pathname === '/'){
        service.getFile(req, res);
    }
    else if(req.method === 'GET' && path === 'api/faculties'){
        console.log('GET api/faculties');
        service.getHandler(req, res, database.getFaculties);
    }
    else if(req.method === 'GET' && path === 'api/pulpits'){
        console.log('GET api/pulpits');
        service.getHandler(req, res, database.getPulpits);
    }
    else if(req.method === 'GET' && path === 'api/subjects'){
        console.log('GET api/subjects');
        service.getHandler(req, res, database.getSubjects);
    }
    else if(req.method === 'GET' && path === 'api/auditoriumstypes'){
        console.log('GET api/auditoriumstypes');
        service.getHandler(req, res, database.getAuditoriumstypes);
    }
    else if(req.method === 'GET' && path === 'api/auditoriums'){
        console.log('GET api/auditoriums');
        service.getHandler(req, res, database.getAuditoriums);
    }
    else if(req.method === 'POST' && path === 'api/faculties'){
        console.log('POST api/faculties');
        service.facultyHandler(req, res, database.insertFaculty);
    }
    else if(req.method === 'POST' && path === 'api/pulpits'){
        console.log('POST api/pulpits');
        service.pulpitHandler(req, res, database.insertPulpit);
    }
    else if(req.method === 'POST' && path === 'api/subjects'){
        console.log('POST api/subjects');
        service.subjectHandler(req, res, database.insertSubject);
    }
    else if(req.method === 'POST' && path === 'api/auditoriumstypes'){
        console.log('POST api/auditoriumstypes');
        service.auditoriumtypeHandler(req, res, database.insertAuditoriumtype);
    }
    else if(req.method === 'POST' && path === 'api/auditoriums'){
        console.log('POST api/auditoriums');
        service.auditoriumHandler(req, res, database.insertAuditorium);
    }
    else if(req.method === 'PUT' && path === 'api/faculties'){
        console.log('PUT api/faculties');
        service.facultyHandler(req, res, database.updateFaculty);
    }
    else if(req.method === 'PUT' && path === 'api/pulpits'){
        console.log('PUT api/pulpits');
        service.pulpitHandler(req, res, database.updatePulpit);
    }
    else if(req.method === 'PUT' && path === 'api/subjects'){
        console.log('PUT api/subjects');
        service.subjectHandler(req, res, database.updateSubject);
    }
    else if(req.method === 'PUT' && path === 'api/auditoriumstypes'){
        console.log('PUT api/auditoriumstypes');
        service.auditoriumtypeHandler(req, res, database.updateAuditoriumtype);
    }
    else if(req.method === 'PUT' && path === 'api/auditoriums'){
        console.log('PUT api/auditoriums');
        service.auditoriumHandler(req, res, database.updateAuditorium);
    }
    else if(req.method === 'DELETE'){
        if(code === undefined || code === ''){
            service.errorHandler(res, 400, 'invalid delete code');
        }
        switch(path){
            case 'api/faculties':
                console.log('DELETE api/facilties');
                service.deleteHandler(req, res, database.deleteFaculty, code);
                break;     
            case 'api/pulpits':
                console.log('DELETE api/pulpits');
                service.deleteHandler(req, res, database.deletePulpit, code);
                break;  
            case 'api/subjects':
                console.log('DELETE api/subjects');
                service.deleteHandler(req, res, database.deleteSubject, code);
                break;  
            case 'api/auditoriumstypes':
                console.log('DELETE api/auditoriumstypes');
                service.deleteHandler(req, res, database.deleteAuditoriumtype, code);
                break;  
            case 'api/auditoriums':
                console.log('DELETE api/auditoriums');
                service.deleteHandler(req, res, database.deleteAuditorium, code);
                break; 
            default:
                service.errorHandler(res, 404, 'Not found');
                break;
        }
    }
    else{
        service.errorHandler(res, 404, 'Not found');
    }
}).listen(3000, () => console.log('http://localhost:3000'));