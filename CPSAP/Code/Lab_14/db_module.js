let sql = require('mssql');

let prop = {
    user: 'UNIVER_USER',
    server: 'DESKTOP-I',
    password:'Strong_pass123!',
    database: 'UNIVERCITY',
    pool: { max: 10, min: 4 },
    options: {
        trustServerCertificate: true,
        enableArithAbort: true
    },
    connectionTimeout: 5000
};

class DB {

    pool = new sql.ConnectionPool(prop).connect()
        .then(pool => {
            console.log('connect to database');
            return pool;
        })
        .catch(err =>
            console.error('connection failed:', err.message)
        );
    getFaculties = () => {
        return this.pool.then(pool => {
            if (pool) {
                return pool.request().query('SELECT * FROM faculty');
            } else {
                throw new Error('No connection pool available');
            }
        });
    }
    getPulpits = () => {
        return this.pool.then(pool => pool.request().query('select * from pulpit'));
    }
    getSubjects = () => {
        return this.pool.then(pool => pool.request().query('select * from subject'));
    }
    getAuditoriumstypes = () => {
        return this.pool.then(pool => pool.request().query('select * from auditorium_type'));
    }
    getAuditoriums = () => {
        return this.pool.then(pool => pool.request().query('select * from auditorium'));
    }

    //--------------------------------

    insertFaculty = (faculty, facultyName) => {
        return this.pool.then(pool => {
            return pool.request()
                .input('faculty', sql.NChar, faculty)
                .input('facultyName', sql.NVarChar, facultyName)
                .query('insert faculty values(@faculty, @facultyName)')
        })
    }
    insertPulpit = (pulpit, pulpitName, faculty) => {
        return this.pool.then(pool => {
            return pool.request()
                .input('pulpit', sql.NChar, pulpit)
                .input('pulpitName', sql.NVarChar, pulpitName)
                .input('faculty', sql.NChar, faculty)
                .query('insert pulpit values(@pulpit, @pulpitName, @faculty)')
        })
    }
    insertSubject = (subject, subjectName, pulpit) => {
        return this.pool.then(pool => {
            return pool.request()
                .input('subject', sql.NChar, subject)
                .input('subjectName', sql.NVarChar, subjectName)
                .input('pulpit', sql.NChar, pulpit)
                .query('insert subject values(@subject, @subjectName, @pulpit)')
        })
    }
    insertAuditoriumtype = (auditoriumtype, auditoriumtypeName) => {
        return this.pool.then(pool => {
            return pool.request()
                .input('auditoriumtype', sql.NChar, auditoriumtype)
                .input('auditoriumtypeName', sql.NVarChar, auditoriumtypeName)
                .query('insert auditorium_type values(@auditoriumtype, @auditoriumtypeName)')
        })
    }
    insertAuditorium = (auditorium, auditoriumName, capasity, auditoriumtype) => {
        console.log(auditorium);
        console.log(auditoriumName);
        console.log(capasity);
        console.log(auditoriumtype);
        return this.pool.then(pool => {

            return pool.request()
                .input('auditorium', sql.NChar, auditorium)
                .input('auditoriumName', sql.NVarChar, auditoriumName)
                .input('capasity', sql.Int, capasity)
                .input('auditoriumtype', sql.NChar, auditoriumtype)
                .query('insert auditorium values(@auditorium, @auditoriumtype, @capasity, @auditoriumName)')
        })
    }

    //-------------------------------

    updateFaculty = async (faculty, facultyName) => {
        let faculties = (await this.getFaculties()).recordset;
        let isFound = false;
        for (let fac of faculties) {
            if (fac.FACULTY.trim() === faculty.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('faculty is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('faculty', sql.NChar, faculty)
                .input('facultyName', sql.NVarChar, facultyName)
                .query('update faculty set faculty_name = @facultyName where faculty = @faculty');
        })
    }
    updatePulpit = async (pulpit, pulpitName, faculty) => {
        let pulpits = (await this.getPulpits()).recordset;
        let isFound = false;
        for (let pul of pulpits) {
            if (pul.PULPIT.trim() === pulpit.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('pulpit is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('pulpit', sql.NChar, pulpit)
                .input('pulpitName', sql.NVarChar, pulpitName)
                .input('faculty', sql.NChar, faculty)
                .query('update pulpit set pulpit_name = @pulpitName, faculty = @faculty where pulpit = @pulpit');
        })
    }
    updateSubject = async (subject, subjectName, pulpit) => {
        let subjects = (await this.getSubjects()).recordset;
        let isFound = false;
        for (let sub of subjects) {
            if (sub.SUBJECT.trim() === subject.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('subject is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('subject', sql.NChar, subject)
                .input('subjectName', sql.NVarChar, subjectName)
                .input('pulpit', sql.NChar, pulpit)
                .query('update subject set subject_name = @subjectName, pulpit = @pulpit where subject = @subject');
        })
    }
    updateAuditoriumtype = async (auditoriumtype, auditoriumtypeName) => {
        let auditoriumtypes = (await this.getAuditoriumstypes()).recordset;
        let isFound = false;
        for (let aut of auditoriumtypes) {
            if (aut.AUDITORIUM_TYPE.trim() === auditoriumtype.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('auditoriumtype is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('auditoriumtype', sql.NChar, auditoriumtype)
                .input('auditoriumtypeName', sql.NVarChar, auditoriumtypeName)
                .query('update auditorium_type set auditorium_typename = @auditoriumtypeName where auditorium_type = @auditoriumtype');
        })
    }
    updateAuditorium = async (auditorium, auditoriumName, capacity, auditoriumtype) => {
        let auditoriums = (await this.getAuditoriums()).recordset;
        let isFound = false;
        for (let aud of auditoriums) {
            if (aud.AUDITORIUM.trim() === auditorium.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('auditorium is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('auditorium', sql.NChar, auditorium)
                .input('auditoriumName', sql.NVarChar, auditoriumName)
                .input('capasity', sql.Int, capacity)
                .input('auditoriumtype', sql.NChar, auditoriumtype)
                .query('update auditorium set auditorium_name = @auditoriumName, auditorium_capacity = @capasity, auditorium_type = @auditoriumtype where auditorium = @auditorium');
        })
    }

    //-------------------------------

    deleteFaculty = async (faculty) => {
        let faculties = (await this.getFaculties()).recordset;
        let isFound = false;
        for (let fac of faculties) {
            if (fac.FACULTY.trim() === faculty.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('faculty is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('faculty', sql.NChar, faculty)
                .query('delete faculty where faculty = @faculty');
        })
    }

    deletePulpit = async (pulpit) => {
        let pulpits = (await this.getPulpits()).recordset;
        let isFound = false;
        for (let pul of pulpits) {
            console.log(pul);
            if (pul.PULPIT.trim() === pulpit.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('pulpit is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('pulpit', sql.NChar, pulpit)
                .query('delete pulpit where pulpit = @pulpit');
        })
    }

    deleteSubject = async (subject) => {
        let subjects = (await this.getSubjects()).recordset;
        let isFound = false;
        for (let sub of subjects) {
            if (sub.SUBJECT.trim() === subject.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('subject is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('subject', sql.NChar, subject)
                .query('delete subject where subject = @subject');
        })
    }

    deleteAuditoriumtype = async (auditoriumtype) => {
        let auditoriumtypes = (await this.getAuditoriumstypes()).recordset;
        let isFound = false;
        for (let aut of auditoriumtypes) {
            if (aut.AUDITORIUM_TYPE.trim() === auditoriumtype.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('auditoriumtype is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('auditoriumtype', sql.NChar, auditoriumtype)
                .query('delete auditorium_type where auditorium_type = @auditoriumtype');
        })
    }
    deleteAuditorium = async (auditorium) => {
        let auditoriums = (await this.getAuditoriums()).recordset;
        let isFound = false;
        for (let aud of auditoriums) {
            if (aud.AUDITORIUM.trim() === auditorium.trim()) {
                isFound = true;
            }
        }
        if (!isFound) {
            throw new Error('auditorium is not found');
        }
        return this.pool.then(pool => {
            return pool.request()
                .input('auditorium', sql.NChar, auditorium)
                .query('delete auditorium where auditorium = @auditorium');
        })
    }

}

exports.DB = DB;