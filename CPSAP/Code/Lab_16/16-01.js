const http = require('http');
const { buildSchema,graphql } = require('graphql');
const sql = require('mssql');


const dbConfig = {
    user: 'GQLuser',            
    password: 'SuperPass!',  
    server: 'DESKTOP-I',
    port:1433,    
    database: 'FIA_GQL',     
    options: {
        encrypt: false, 
        trustServerCertificate: true 
    }
};

const schema = buildSchema(`
    type FACULTY {
        FACULTY: String!
        FACULTY_NAME: String
    }

    type PULPIT {
        PULPIT: String!
        PULPIT_NAME: String
        FACULTY: String!
    }

    type TEACHER {
        TEACHER: String!
        TEACHER_NAME: String
        PULPIT: String!
    }

    type SUBJECT {
        SUBJECT: String!
        SUBJECT_NAME: String
        PULPIT: String!
    }

    type TEACHERS_BY_FACULTY_RESULT {
        FACULTY: String!
        TEACHERS: [TEACHER]
    }

    type SUBJECTS_BY_FACULTY_RESULT {
        PULPIT: String!
        PULPIT_NAME: String!
        FACULTY: String!
        SUBJECTS: [SUBJECT]
    }

    type Query {
        getFaculties(faculty: String): [FACULTY]
        getPulpits(pulpit: String): [PULPIT]
        getTeachers(teacher: String): [TEACHER]
        getSubjects(subject: String): [SUBJECT]
        
        getTeachersByFaculty(faculty: String!): [TEACHERS_BY_FACULTY_RESULT]
        getSubjectsByFaculties(faculty: String!): [SUBJECTS_BY_FACULTY_RESULT]
    }

    type Mutation {
        setFaculty(faculty: String!, faculty_name: String!): FACULTY
        setPulpit(pulpit: String!, pulpit_name: String!, faculty: String!): PULPIT
        setTeacher(teacher: String!, teacher_name: String!, pulpit: String!): TEACHER
        setSubject(subject: String!, subject_name: String!, pulpit: String!): SUBJECT

        delFaculty(faculty: String!): Boolean
        delPulpit(pulpit: String!): Boolean
        delTeacher(teacher: String!): Boolean
        delSubject(subject: String!): Boolean
    }
`);


const root = {
    getFaculties: async ({ faculty }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const request = pool.request();
            let query = 'SELECT * FROM FACULTY';
            
            if (faculty) {
                request.input('id', sql.NVarChar, faculty);
                query += ' WHERE FACULTY = @id';
            }
            
            const result = await request.query(query);
            return result.recordset;
        } catch (err) { console.error(err); return []; }
    },

    getPulpits: async ({ pulpit }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const request = pool.request();
            let query = 'SELECT * FROM PULPIT';
            
            if (pulpit) {
                request.input('id', sql.NVarChar, pulpit);
                query += ' WHERE PULPIT = @id';
            }
            
            const result = await request.query(query);
            return result.recordset;
        } catch (err) { console.error(err); return []; }
    },

    getTeachers: async ({ teacher }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const request = pool.request();
            let query = 'SELECT * FROM TEACHER';
            
            if (teacher) {
                request.input('id', sql.NVarChar, teacher);
                query += ' WHERE TEACHER = @id';
            }
            
            const result = await request.query(query);
            return result.recordset;
        } catch (err) { console.error(err); return []; }
    },

    getSubjects: async ({ subject }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const request = pool.request();
            let query = 'SELECT * FROM SUBJECT';
            
            if (subject) {
                request.input('id', sql.NVarChar, subject);
                query += ' WHERE SUBJECT = @id';
            }
            
            const result = await request.query(query);
            return result.recordset;
        } catch (err) { console.error(err); return []; }
    },


    getTeachersByFaculty: async ({ faculty }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('fac', sql.NVarChar, faculty)
                .query(`
                    SELECT T.TEACHER, T.TEACHER_NAME, T.PULPIT 
                    FROM TEACHER T
                    JOIN PULPIT P ON T.PULPIT = P.PULPIT
                    WHERE P.FACULTY = @fac
                `);
            return [{
                FACULTY: faculty,
                TEACHERS: result.recordset
            }];
        } catch (err) { console.error(err); return []; }
    },

    getSubjectsByFaculties: async ({ faculty }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('fac', sql.NVarChar, faculty)
                .query(`
                    SELECT P.PULPIT, P.PULPIT_NAME, P.FACULTY, S.SUBJECT, S.SUBJECT_NAME
                    FROM PULPIT P
                    LEFT JOIN SUBJECT S ON S.PULPIT = P.PULPIT
                    WHERE P.FACULTY = @fac
                `);
            const pulpitsMap = new Map();

            result.recordset.forEach(row => {
                if (!pulpitsMap.has(row.PULPIT)) {
                    pulpitsMap.set(row.PULPIT, {
                        PULPIT: row.PULPIT,
                        PULPIT_NAME: row.PULPIT_NAME,
                        FACULTY: row.FACULTY,
                        SUBJECTS: []
                    });
                }
                
                if (row.SUBJECT) {
                    pulpitsMap.get(row.PULPIT).SUBJECTS.push({
                        SUBJECT: row.SUBJECT,
                        SUBJECT_NAME: row.SUBJECT_NAME,
                        PULPIT: row.PULPIT
                    });
                }
            });

            return Array.from(pulpitsMap.values());
        } catch (err) { console.error(err); return []; }
    },



    setFaculty: async ({ faculty, faculty_name }) => {
        const pool = await sql.connect(dbConfig);
        const result = await pool.request()
            .input('id', sql.NVarChar, faculty)
            .input('name', sql.NVarChar, faculty_name)
            .query(`
                MERGE FACULTY AS target
                USING (SELECT @id, @name) AS source (id, name)
                ON (target.FACULTY = source.id)
                WHEN MATCHED THEN  
                    UPDATE SET FACULTY_NAME = source.name
                WHEN NOT MATCHED THEN
                    INSERT (FACULTY, FACULTY_NAME) VALUES (source.id, source.name);
                SELECT * FROM FACULTY WHERE FACULTY = @id;
            `);
        return result.recordset[0];
    },

    setPulpit: async ({ pulpit, pulpit_name, faculty }) => {
        const pool = await sql.connect(dbConfig);
        const result = await pool.request()
            .input('id', sql.NVarChar, pulpit)
            .input('name', sql.NVarChar, pulpit_name)
            .input('fac', sql.NVarChar, faculty)
            .query(`
                MERGE PULPIT AS target
                USING (SELECT @id, @name, @fac) AS source (id, name, fac)
                ON (target.PULPIT = source.id)
                WHEN MATCHED THEN
                    UPDATE SET PULPIT_NAME = source.name, FACULTY = source.fac
                WHEN NOT MATCHED THEN
                    INSERT (PULPIT, PULPIT_NAME, FACULTY) VALUES (source.id, source.name, source.fac);
                SELECT * FROM PULPIT WHERE PULPIT = @id;
            `);
        return result.recordset[0];
    },

    setTeacher: async ({ teacher, teacher_name, pulpit }) => {
        const pool = await sql.connect(dbConfig);
        const result = await pool.request()
            .input('id', sql.NVarChar, teacher)
            .input('name', sql.NVarChar, teacher_name)
            .input('pul', sql.NVarChar, pulpit)
            .query(`
                MERGE TEACHER AS target
                USING (SELECT @id, @name, @pul) AS source (id, name, pul)
                ON (target.TEACHER = source.id)
                WHEN MATCHED THEN
                    UPDATE SET TEACHER_NAME = source.name, PULPIT = source.pul
                WHEN NOT MATCHED THEN
                    INSERT (TEACHER, TEACHER_NAME, PULPIT) VALUES (source.id, source.name, source.pul);
                SELECT * FROM TEACHER WHERE TEACHER = @id;
            `);
        return result.recordset[0];
    },

    setSubject: async ({ subject, subject_name, pulpit }) => {
        const pool = await sql.connect(dbConfig);
        const result = await pool.request()
            .input('id', sql.NVarChar, subject)
            .input('name', sql.NVarChar, subject_name)
            .input('pul', sql.NVarChar, pulpit)
            .query(`
                MERGE SUBJECT AS target
                USING (SELECT @id, @name, @pul) AS source (id, name, pul)
                ON (target.SUBJECT = source.id)
                WHEN MATCHED THEN
                    UPDATE SET SUBJECT_NAME = source.name, PULPIT = source.pul
                WHEN NOT MATCHED THEN
                    INSERT (SUBJECT, SUBJECT_NAME, PULPIT) VALUES (source.id, source.name, source.pul);
                SELECT * FROM SUBJECT WHERE SUBJECT = @id;
            `);
        return result.recordset[0];
    },

    // --- MUTATIONS (DELETE) ---

    delFaculty: async ({ faculty }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('id', sql.NVarChar, faculty)
                .query('DELETE FROM FACULTY WHERE FACULTY = @id');
            return result.rowsAffected[0] > 0;
        } catch (err) {
            console.error('Error deleting faculty:', err.message);
            return false;
        }
    },

    delPulpit: async ({ pulpit }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('id', sql.NVarChar, pulpit)
                .query('DELETE FROM PULPIT WHERE PULPIT = @id');
            return result.rowsAffected[0] > 0;
        } catch (err) { return false; }
    },

    delTeacher: async ({ teacher }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('id', sql.NVarChar, teacher)
                .query('DELETE FROM TEACHER WHERE TEACHER = @id');
            return result.rowsAffected[0] > 0;
        } catch (err) { return false; }
    },

    delSubject: async ({ subject }) => {
        try {
            const pool = await sql.connect(dbConfig);
            const result = await pool.request()
                .input('id', sql.NVarChar, subject)
                .query('DELETE FROM SUBJECT WHERE SUBJECT = @id');
            return result.rowsAffected[0] > 0;
        } catch (err) { return false; }
    }
};

//
const server = http.createServer((req, res) => {
    res.setHeader('Content-Type', 'application/json');

    if (req.method === 'POST') {
        let body = '';

        req.on('data', chunk => {
            body += chunk;
        });

        req.on('end', async () => {
            try {
                const { query, variables } = JSON.parse(body);

                const result = await graphql({
                    schema: schema,
                    source: query,    
                    rootValue: root,
                    variableValues: variables
                });

                res.statusCode = 200;
                res.end(JSON.stringify(result));

            } catch (error) {
                res.statusCode = 400;
                res.end(JSON.stringify({ error: 'Invalid JSON or Request', details: error.message }));
            }
        });
    } else {
        res.statusCode = 405;
        res.end(JSON.stringify({ error: 'Method Not Allowed. Use POST with JSON body.' }));
    }
});

server.listen(3000, () => {
    console.log('Server running at http://localhost:3000/');
});