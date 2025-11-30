const rpcWS = require('rpc-websockets').Client;
const client = new rpcWS("ws://localhost:4000/rpc");

client.on('open', () => {
    client.call('square', [3]).then((r) => { console.log("Square with 1 parameter: ", r); });
    client.call('square', [5, 4]).then((r) => { console.log("Square with 2 parameters: ", r); });

    client.call('sum', [2]).then((r) => { console.log("Sum with 1 parameter: ", r); });
    client.call('sum', [2, 4, 6, 8, 10]).then((r) => { console.log("Sum with 5 parameters", r); });

    client.call('mul', [3]).then((r) => { console.log("Mul with 1 parameter: ", r); });
    client.call('mul', [3, 5, 7, 9, 11, 13]).then((r) => { console.log("Mul with 6 parameters: ", r); });

    client.login({ login: 'mnmd', password: '12345' })
        .then((login) => {
            if (login) {
                client.call('fib', [1]).then((r) => { console.log("Fib for 1: ", r); }).catch((err) => { console.error("Error occured: ", err) });
                client.call('fib', [2]).then((r) => { console.log("Fib for 2: ", r); }).catch((err) => { console.error("Error occured: ", err) });
                client.call('fib', [7]).then((r) => { console.log("Fib for 7: ", r); }).catch((err) => { console.error("Error occured: ", err) });

                client.call('fact', [0]).then((r) => { console.log("Fact for 0: ", r); }).catch((err) => { console.error("Error occured: ", err) });
                client.call('fact', [5]).then((r) => { console.log("Fact for 5: ", r); }).catch((err) => { console.error("Error occured: ", err) });
                client.call('fact', [10]).then((r) => { console.log("Fact for 10: ", r); }).catch((err) => { console.error("Error occured: ", err) });
            }
            else {
                console.error('Login failed');
            }
        })
        .catch((err) => { console.error("Error occured: ", err); });
});