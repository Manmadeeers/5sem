let fs = require('fs');

class DbService{

    getFile = (req, res) => {
        res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
        res.end(fs.readFileSync('./index.html'));
    }

    getHandler = (req, res, databaseFunction, parameter = null) => {
        console.log('getHandler');
        if(parameter !== null){
            databaseFunction(parameter)
            .then(records => {
                res.writeHead(200, {'Content-Type': 'application/json; charset=utf-8'});
                res.end(JSON.stringify(records.recordset, null, 4));
            })
            .catch(err => {
                this.errorHandler(res, 422, err.message);
            })
        }
        else{
            databaseFunction()
            .then(records => {
                res.writeHead(200, {'Content-Type': 'application/json; charset=utf-8'});
                res.end(JSON.stringify(records.recordset, null, 4));
            })
            .catch(err => {
                this.errorHandler(res, 422, err.message);
            })
        }    
    }

    facultyHandler = (req, res, databaseFunction) => {
        let json = '';
        req.on('data', chunk => {
            json += chunk;
        })

        req.on('end', () => {
            json = JSON.parse(json);
            if(json.FACULTY === undefined || json.FACULTY_NAME === undefined){
                this.errorHandler(res, 422, 'invalid faculty parameter');
                return;
            }
            else if(json.FACULTY.trim() === '' || json.FACULTY_NAME.trim() === ''){
                this.errorHandler(res, 422, 'invalid faculty parameter');
                return;
            }

            databaseFunction(json.FACULTY, json.FACULTY_NAME)
                .then(() => {
                    res.end(JSON.stringify(json, null, 4));
                })
                .catch((err) => {
                    this.errorHandler(res, 422, err.message);
                })

        })
    }

    pulpitHandler = (req, res, databaseFunction) => {
        let json = '';
        req.on('data', chunk => {
            json += chunk;
        })

        req.on('end', () => {
            json = JSON.parse(json);
            console.log(json);
            if(json.PULPIT === undefined || json.PULPIT_NAME === undefined || json.FACULTY === undefined){
                this.errorHandler(res, 422, 'invalid pulpit parameter');
                return;
            }
            else if(json.PULPIT.trim() === '' || json.PULPIT_NAME.trim() === '' || json.FACULTY.trim() === ''){
                this.errorHandler(res, 422, 'invalid pulpit parameter');
                return;
            }

            databaseFunction(json.PULPIT, json.PULPIT_NAME, json.FACULTY)
                .then(() => {
                    res.end(JSON.stringify(json, null, 4));
                })
                .catch((err) => {
                    this.errorHandler(res, 422, err.message);
                })

        })
    }

    subjectHandler = (req, res, databaseFunction) => {
        let json = '';
        req.on('data', chunk => {
            json += chunk;
        })

        req.on('end', () => {
            json = JSON.parse(json);
            if(json.SUBJECT === undefined || json.SUBJECT_NAME === undefined || json.PULPIT === undefined){
                this.errorHandler(res, 422, 'invalid subject parameter');
                return;
            }
            else if(json.SUBJECT.trim() === '' || json.SUBJECT_NAME.trim() === '' || json.PULPIT.trim() === ''){
                this.errorHandler(res, 422, 'invalid subject parameter');
                return;
            }

            databaseFunction(json.SUBJECT, json.SUBJECT_NAME, json.PULPIT)
                .then(() => {
                    res.end(JSON.stringify(json, null, 4));
                })
                .catch((err) => {
                    this.errorHandler(res, 422, err.message);
                })

        })
    }

    auditoriumtypeHandler = (req, res, databaseFunction) => {
        let json = '';
        req.on('data', chunk => {
            json += chunk;
        })

        req.on('end', () => {
            json = JSON.parse(json);
            if(json.AUDITORIUM_TYPE === undefined || json.AUDITORIUM_TYPENAME === undefined){
                this.errorHandler(res, 422, 'invalid auditorium_type parameter');
                return;
            }
            else if(json.AUDITORIUM_TYPE.trim() === '' || json.AUDITORIUM_TYPENAME.trim() === ''){
                this.errorHandler(res, 422, 'invalid auditorium_type parameter');
                return;
            }

            databaseFunction(json.AUDITORIUM_TYPE, json.AUDITORIUM_TYPENAME)
                .then(() => {
                    res.end(JSON.stringify(json, null, 4));
                })
                .catch((err) => {
                    this.errorHandler(res, 422, err.message);
                })

        })
    }

    auditoriumHandler = (req, res, databaseFunction) => {
        let json = '';
        req.on('data', chunk => {
            json += chunk;
        })

        req.on('end', () => {
            json = JSON.parse(json);
            if(json.AUDITORIUM === undefined || json.AUDITORIUM_NAME === undefined || json.AUDITORIUM_CAPACITY === undefined || json.AUDITORIUM_TYPE === undefined){
                this.errorHandler(res, 422, 'invalid auditorium parameter');
                return;
            }
            else if(json.AUDITORIUM.trim() === '' || json.AUDITORIUM_NAME.trim() === '' || !Number(json.AUDITORIUM_CAPACITY) || json.AUDITORIUM_TYPE.trim() === ''){
                this.errorHandler(res, 422, 'invalid auditorium parameter');
                return;
            }

            databaseFunction(json.AUDITORIUM, json.AUDITORIUM_NAME, json.AUDITORIUM_CAPACITY, json.AUDITORIUM_TYPE)
                .then(() => {
                    console.log(json.AUDITORIUM);
                    console.log(json.AUDITORIUM_NAME);
                    console.log(json.AUDITORIUM_CAPACITY);
                    console.log(json.AUDITORIUM_TYPE);
                    console.log('next')
                    res.end(JSON.stringify(json, null, 4));
                })
                .catch((err) => {
                    this.errorHandler(res, 422, err.message);
                })

        })
    }

    deleteHandler = (req, res, databaseFunction, param) => {
        databaseFunction(param)
            .then(() => {
                res.end(JSON.stringify({key: param}, null, 4));
            })
            .catch(err => {
                this.errorHandler(res, 422, err.message);
            })
    }

    errorHandler = (res, errorCode, errorMsg) => {
        res.writeHead(errorCode, 'Error while creating request', {'Content-Type': 'application/json; charset=utf-8'});
        res.end(JSON.stringify({errorCode: errorCode, errorMsg: errorMsg}, null, 4));
    }
}

exports.DbService = DbService;