const plantController = require('../app/Controllers/plantController');
module.exports = {
    run: (app) => {
        app.get('/pump', plantController.controlPump);
        app.get('/pump-off', plantController.controlPumpOff);
        app.get('/lamp', plantController.controlLamp);
        app.get('/lamp-off', plantController.controlLampOff);
        app.get('/update', plantController.updateData);
        app.get('/time', plantController.getTime);
        app.post('/schedule', plantController.schedule);
        app.post('/time', plantController.addTime);
        app.delete('/time', plantController.removeTime);
    }
}