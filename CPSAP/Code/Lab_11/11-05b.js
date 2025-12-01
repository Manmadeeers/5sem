const rpcWS = require('rpc-websockets').Client;
const async = require('async');
const client = new rpcWS("ws://localhost:4000");

client.on('open', () => {
    client.login({ login: 'mnmd', password: '12345' })
        .then((logged) => {
            if (!logged) {
                throw new Error("Login failed");
            }

            async.parallel({
                square1: (cb)=>client.call('square',[3]).then(r=>cb(null,r)).catch(cb),
                square2: (cb)=>client.call('square',[5,4]).then(r=>cb(null,r)).catch(cb),
                bigMul: (cb)=>client.call('mul',[3,5,7,9,11,13]).then(r=>cb(null,r)).catch(cb),
                fib7: (cb)=>client.call('fib',[7]).then(r=>cb(null,r)).catch(cb),
                smallMul: (cb)=>client.call('mul',[2,4,6]).then(r=>cb(null,r)).catch(cb)
            },(err,results)=>{
                if(err){
                    console.error("Error during parallel calls: ",err);
                    return;
                }
                let fibValue;
                if(Array.isArray(results.fib7)){
                    fibValue = results.fib7[6];
                }
                else{
                    fibValue = results.fib7;
                }
                
                let firstSum = results.square1+results.square2+results.bigMul;
                let secondMul = fibValue*results.smallMul;
                let finalSum = firstSum+secondMul;

                console.log("Result: ",finalSum);
            });
        })
        .catch((err)=>{
            console.error("Error occured(promise): ",err);
        });
});

client.on('error',(err)=>{
    console.error("Error ocured(client): ",err);
});
