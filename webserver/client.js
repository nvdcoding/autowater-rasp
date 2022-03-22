const mqtt = require('mqtt')
const client = mqtt.connect('mqtt://ngoinhaiot.com:1111', {
    username: 'lkuno222',
    password: '0DC42C6DB41C43B6'
})
exports.client = client
exports.channel = (topic) => `lkuno222/${topic}`