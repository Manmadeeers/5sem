const { MongoClient } = require('mongodb');
const http = require('http');
const url = require('url');

const uri = 'mongodb://localhost:32768';
const client = new MongoClient(uri);
const dbName = 'BSTU';

async function handleDbOperation(res, operation) {
    try {
        await client.connect();
        console.log('MongoDB: connect successful');
        const db = client.db(dbName);
        
        const result = await operation(db);
        if (result !== undefined) {
             res.writeHead(result ? 200 : 404, { 'Content-Type': 'application/json' });
             res.end(JSON.stringify(result || 'Not Found'));
        }
    } catch (err) {
        console.error('Error:', err);
        if (err.message.includes('Validation')) {
            res.writeHead(400, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ error: err.message }));
        } else {
            res.writeHead(500, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify('Internal Server Error'));
        }
    } finally {
        await client.close();
    }
}

http.createServer((req, res) => {
    let pathname = url.parse(req.url).pathname;
    let parts = pathname.split('/');
    let path = parts[1] + '/' + parts[2];
    let code = decodeURIComponent(parts[3]);
    console.log(code);
    let method = req.method;

    // --- GET FACULTIES ---
    if (method === 'GET' && path === 'api/faculties') {
        handleDbOperation(res, async (db) => {
            return await db.collection('faculty').find({}).toArray();
        });
    } 
    // --- GET PULPITS ---
    else if (method === 'GET' && path === 'api/pulpits') {
        handleDbOperation(res, async (db) => {
            return await db.collection('pulpit').find({}).toArray();
        });
    } 
    // --- POST FACULTIES ---
    else if (method === 'POST' && path === 'api/faculties') {
        let body = '';
        req.on('data', chunk => body += chunk.toString());
        req.on('end', () => {
            const data = JSON.parse(body);

            handleDbOperation(res, async (db) => {
                const result = await db.collection('faculty').insertOne(data);
                return { data, _id: result.insertedId };
            });
        });
    } 
    // --- POST PULPITS ---
    else if (method === 'POST' && path === 'api/pulpits') {
        let body = '';
        req.on('data', chunk => body += chunk.toString());
        req.on('end', () => {
            const data = JSON.parse(body);

            handleDbOperation(res, async (db) => {
                const result = await db.collection('pulpit').insertOne(data);
                return { data, _id: result.insertedId };
            });
        });
    } 
    // --- PUT FACULTIES ---
    else if (method === 'PUT' && path === 'api/faculties') {
        let body = '';
        req.on('data', chunk => body += chunk.toString());
        req.on('end', () => {
            const data = JSON.parse(body);
            const facultyCode = data.FACULTY || data.faculty;
            const updateData = {
                faculty: facultyCode,
                faculty_name: data.FACULTY_NAME || data.faculty_name
            };

            handleDbOperation(res, async (db) => {
                const result = await db.collection('faculty').findOneAndUpdate(
                    { faculty: facultyCode }, 
                    { $set: updateData },
                    { returnDocument: 'after' } 
                );
                return result;
            });
        });
    } 
    // --- PUT PULPITS ---
    else if (method === 'PUT' && path === 'api/pulpits') {
        let body = '';
        req.on('data', chunk => body += chunk.toString());
        req.on('end', () => {
            const data = JSON.parse(body);
            const pulpitCode = data.PULPIT || data.pulpit;
            const updateData = {
                pulpit: pulpitCode,
                pulpit_name: data.PULPIT_NAME || data.pulpit_name,
                faculty: data.FACULTY || data.faculty
            };

            handleDbOperation(res, async (db) => {
                 // Опционально: здесь тоже можно добавить проверку существования факультета перед обновлением
                const result = await db.collection('pulpit').findOneAndUpdate(
                    { pulpit: pulpitCode },
                    { $set: updateData },
                    { returnDocument: 'after' }
                );
                return result;
            });
        });
    } 
    // --- DELETE FACULTIES ---
    else if (method === 'DELETE' && path === 'api/faculties' && code) {
        handleDbOperation(res, async (db) => {
            const result = await db.collection('faculty').deleteOne({ faculty: code });
            if (result.deletedCount === 0) return null;
            return { message: 'Faculty deleted successfully' };
        });
    } 
    // --- DELETE PULPITS ---
    else if (method === 'DELETE' && path === 'api/pulpits' && code) {
        handleDbOperation(res, async (db) => {
            const result = await db.collection('pulpit').deleteOne({ pulpit: code });
            if (result.deletedCount === 0) return null;
            return { message: 'Pulpit deleted successfully' };
        });
    } 
    else {
        res.writeHead(404, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify('Not Found'));
    }
}).listen(3000, () => console.log('Server running at http://localhost:3000'));