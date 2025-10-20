const sendmail = require('sendmail')({silent:false});


sendmail({
    from:"philmade6@gmail.com",
    to:"philmades625@gmail.com",
    subject:"test",
    html:"<p>Test mesage</p>"
},function(err,reply){
    console.log(err);
    console.log(reply);
});