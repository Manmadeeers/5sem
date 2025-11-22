const http = require('http');
const { XMLParser, XMLBuilder } = require('fast-xml-parser');
const parser = new XMLParser(
    {
        ignoreAttributes: false,
        attributeNamePrefix: '',
    }
);
const builder = new XMLBuilder(
    {
        ignoreAttributes: false,
        attributeNamePrefix: '',
        format: true,
        suppressEmptyNode: true,
    }
);

let xmlRaw = {
    request: {
        id: 1,
        x: [
            { value: 1 },
            { value: 2 }
        ],
        m: [
            { value: 'a' },
            { value: 'b' },
            { value: 'c' }
        ]
    }
}
let xmlRequest = builder.build(xmlRaw);

let options = {
    host: 'localhost',
    path: '/post-xml',
    port: 5000,
    method: 'POST',
    headers: {
        'content-type': 'application/xml',
        'accept': 'application/xml'
    }
}

const request = http.request(options, (response) => {
    console.log('Response status code: ', response.statusCode);
    let xmlResponse = '';
    response.on('data', (chunk) => {
        xmlResponse += chunk;
    });
    response.on('end', () => {
        const parsedXML = parser.parse(xmlResponse);
        console.log('Response body: ', parsedXML);
    });
});

request.on('error', (err) => {
    console.error(err);
});

request.end(xmlRequest);

