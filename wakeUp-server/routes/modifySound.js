var express = require('express');
var router = express.Router();


router.get('/', function(req, res, next) {
    var socketClient = req.app.get('socketClient');
    res.render('modifySound', { 
        title: 'Wake Up - change alarm sound',
  });

});


router.route('/set')
.post((req, res) => {

    var soundFile = req.files.alarm_sound;

    console.log("Name of file: " + soundFile.name);

    var socketClient = req.app.get('socketClient');

    socketClient.emit('changeSound', soundFile.name);
    
    res.redirect('/');    
    
});


module.exports = router;
