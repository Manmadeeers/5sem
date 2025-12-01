const rpcWS = require('rpc-websockets').Server;
const SERVER_OPTIONS = {
    host: 'localhost',
    port: 4000
};

function factorial(num) {
    if (num < 0) {
        return {error:"Invalid factorial number"};
    }
    if (num <= 1) {
        return 1;
    }
    return num * factorial(num - 1);
}

function fibonacci(n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

const server = new rpcWS(SERVER_OPTIONS);

server.register('square', (params) => {
    if (params.length == 1) {
        return Math.PI * Math.pow(params[0], 2);
    }
    else if (params.length == 2) {
        return params[0] * params[1];
    }
}).public();

server.register('sum', (params) => {
    let sum = 0;
    params.forEach(param => {
        sum += param;
    });
    return sum;
}).public();

server.register('mul', (params) => {
    let mult = 1;
    params.forEach(param=>{
        mult*=param;
    });
    return mult;
}).public();

server.register('fib',(params)=>{
    if(params.length==1){
        let result = [];
        for(let i=0;i<params[0];++i){
            result[i] = fibonacci(i);
        }
        return result;
    }
}).protected();

server.register('fact',(params)=>{
    if(params.length==1){
        return factorial(params[0]);
    }   
}).protected();

server.setAuth((credentials)=>{
    return credentials.login=='mnmd'&&credentials.password == '12345';
});



console.log(`Server running on ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}`);