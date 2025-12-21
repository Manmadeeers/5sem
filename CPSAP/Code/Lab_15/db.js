const { MongoClient } = require('mongodb');

async function run() {
    const client = new MongoClient('mongodb://localhost:32768');
    try {
        await client.connect();
        const database = client.db('BSTU');

        const pulpits = database.collection('pulpit');
        await pulpits.insertMany([
            { pulpits: "ИСиТ", pulpits_name: "Информационных систем и технологий", faculty: "ИТ" },
            { pulpits: "ПИ", pulpits_name: "Программная инженерия", faculty: "ИТ" }
        ]);

        const faculties = database.collection('faculty');
        await faculties.insertMany([
            { faculty: "ИЗ", faculty_name: "Инженерно-экономический" },
            { faculty: "ИТ", faculty_name: "Информационных технологий" }
        ]);

        console.log("Коллекции созданы и данные вставлены успешно.");
    } finally {
        await client.close();
    }
}

run().catch(console.dir);