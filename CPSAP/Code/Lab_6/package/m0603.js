const sendmail = require('sendmail');
const nodemailer = require('nodemailer');
const { resolve } = require('path');
const email = 'philmade6@gmail.com';

/**
 * @param {string} message 
 * @param {string} password 
 * @returns {Promise}
*/


const send = (message, password) => {
    return new Promise((resolve, reject) => {
        const transporter = nodemailer.createTransport({
            service: 'gmail',
            auth: {
                user: email,
                pass: password
            },
            secure: true
        });

        const mailOptions = {
            from: email,
            to: email,
            subject: 'Message from m0603 module',
            text: message
        };

        transporter.sendMail(mailOptions, (err, info) => {
            if (err) {
                console.log(err);
                reject(err);
            }
            if (info) {
                console.log(info);
                resolve(info);
            }
        });
    })

}


module.exports = { send };