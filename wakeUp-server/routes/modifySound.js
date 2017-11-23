var express = require('express');
var router = express.Router();
var fs = require('fs');
var path = require('path')


router.route('/set')
.post((req, res) => {

    var tempPath = req.file.path;
    var fileName = req.file.originalname; 

    var randomNum = Math.floor(Math.random() * 50000);
    var targetName = randomNum + fileName;

    var targetPath = path.resolve('../wave-files/' + targetName);

    if (path.extname(fileName).toLowerCase() === '.wav') {
        fs.rename(tempPath, targetPath, function(err) {
            if (err) throw err;

            console.log("Upload completed!");
        });

    } else {
        fs.unlink(tempPath, function () {
            if (err) throw err;

            console.error("Only .wav files are allowed!");
        });
    }

    var socketClient = req.app.get('socketClient');
    socketClient.emit('changeSound', targetName);
    
    res.redirect('/');    
    
});


module.exports = router;
