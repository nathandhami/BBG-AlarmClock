var express = require('express');
var xssFilters = require('xss-filters');
var moment = require('moment-timezone');
var Alarm = require('../models/Alarm');
var router = express.Router();
var isInitialised = false;


/* GET home page. */
router.get('/', function(req, res, next) {
    var socketClient = req.app.get('socketClient');
    Alarm.find({}, function(err, alarms) {
	    if (err){ 
	    	throw err;
	    } 
	    else {
        // initialise array in C.
        if (!isInitialised) {
          var alarmString = "";
          for (let i = 0; i < alarms.length; i++) {
            alarmString += "Time=" + alarms[i].time + "-";
            alarmString += "StatusOn=" + alarms[i].statusOn + "-";
            alarmString += "Difficulty=" + alarms[i].level + "-";
            alarmString += "Question=" + alarms[i].question + "-";
            alarmString += "Days=" + alarms[i].days + "-";
            alarmString += "ID=" + alarms[i].identification;
            alarmString += "\n";
          }
          socketClient.emit('initArray', alarmString);
          isInitialised = true;
        }

	    	res.render('index', { 
			  	title: 'Wake Up',
			  	alarms: alarms,
			});
	    }
	});
});

router.post('/trigger', function(req, res, next) {
  var qType = xssFilters.inHTMLData(req.body.type);
  var question = xssFilters.inHTMLData(req.body.question);
  var answers;

  if (qType == "true") {
    answers = JSON.parse(xssFilters.inHTMLData(req.body.options))
  }

  var socketClient = req.app.get('socketClient');
  socketClient.emit("trigger", qType, question, answers);
});

router.post('/stop', function(req, res, next) {
  var socketClient = req.app.get('socketClient');
  socketClient.emit("stop");
});

router.route('/alarm/set')
  .post((req, res) => {
    var socketClient = req.app.get('socketClient');
    var alarmTime = xssFilters.inHTMLData(req.body.time);

    var days = [];
    var isDaySet = false;

    days.push(xssFilters.inHTMLData(req.body.sunday));
    days.push(xssFilters.inHTMLData(req.body.monday));
    days.push(xssFilters.inHTMLData(req.body.tuesday));
    days.push(xssFilters.inHTMLData(req.body.wednesday));
    days.push(xssFilters.inHTMLData(req.body.thursday));
    days.push(xssFilters.inHTMLData(req.body.friday));
    days.push(xssFilters.inHTMLData(req.body.saturday));

    var questionType = xssFilters.inHTMLData(req.body.questionType);

    for (let i = 0; i < days.length; i++) {
      if (days[i] == 'on') {
        isDaySet = true;
        break;
      }
    }

    // TODO: date time zone needs to be set!
    if (!isDaySet) {
      var todayId = moment().tz("America/Los_Angeles").day();
      days[todayId] = 'on';
    }

    var diff_level = xssFilters.inHTMLData(req.body.level);
    var id = xssFilters.inHTMLData(req.body.existingId);

    Alarm.findOne({'identification': id}).exec((err, alarm) => {
      if (err) {
        throw err;
      } 
      else if (alarm == null) {
        const newAlarm = new Alarm({
          time: alarmTime,
          days: days,
          level: diff_level,
          question: questionType,
          statusOn: true,
        });

        newAlarm.save((err, object) => {
          if (err) throw err;
          var alarmString = "";
          alarmString += "Time=" + newAlarm.time + "-";
          alarmString += "StatusOn=" + newAlarm.statusOn + "-";
          alarmString += "Difficulty=" + newAlarm.level + "-";
          alarmString += "Question=" + newAlarm.question + "-";
          alarmString += "Days=" + newAlarm.days + "-";
          alarmString += "ID=" + newAlarm.identification;
          alarmString += "\n";
          socketClient.emit('createAlarm', alarmString);
        });
      }
      else {
          alarm.time = alarmTime;
          alarm.days = days;
          alarm.level = diff_level;
          alarm.question = questionType;
          alarm.status = true;

          alarm.save((err, object) => {
            if (err) throw err;
            var alarmString = "";
            alarmString += "Time=" + alarm.time + "-";
            alarmString += "StatusOn=" + alarm.statusOn + "-";
            alarmString += "Difficulty=" + alarm.level + "-";
            alarmString += "Question=" + alarm.question + "-";
            alarmString += "Days=" + alarm.days + "-";
            alarmString += "ID=" + alarm.identification;
            alarmString += "\n";
            socketClient.emit('editAlarm', alarmString);
          });
      }

      res.redirect('/');
    });

	// TODO: set alarm through C application
  });


router.route('/alarm/delete')
  .post((req, res, next) => {
    var socketClient = req.app.get('socketClient');
    var id = xssFilters.inHTMLData(req.body.alarmId);
    console.log("ALARM: " + id);

    Alarm.findOne({'identification': id}).exec((err, alarm) => {
      if (err || alarm == null) {
        throw err;
      } else {
          alarm.remove();
          socketClient.emit('deleteAlarm', id);
      }

      res.redirect('/');
    });

  });

router.route('/alarm/status')
  .post((req, res, next) => {
    var socketClient = req.app.get('socketClient');
    var id = xssFilters.inHTMLData(req.body.alarmId);
    var status = xssFilters.inHTMLData(req.body.status);

    Alarm.findOne({'identification': id}).exec((err, alarm) => {
      if (err || alarm == null) {
        throw err;
      } else {
          if (status == "true") {
            alarm.statusOn = true;
          } else if (status == "false") {
            alarm.statusOn = false;
          }

          alarm.save((err, object) => {
            if (err) throw err;
            var alarmString = "";
            alarmString += "Time=" + alarm.time + "-";
            alarmString += "StatusOn=" + alarm.statusOn + "-";
            alarmString += "Difficulty=" + alarm.level + "-";
            alarmString += "Question=" + alarm.question + "-";
            alarmString += "Days=" + alarm.days + "-";
            alarmString += "ID=" + alarm.identification;
            alarmString += "\n";
            socketClient.emit('editAlarm', alarmString);
          });
      }
    });

  });

module.exports = router;
