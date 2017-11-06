var express = require('express');
var xssFilters = require('xss-filters');
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
        if (!isInitialised) {
          var alarmString = "";
          for (let i = 0; i < alarms.length; i++) {
            alarmString += "Time=" + alarms[i].time + "-";
            alarmString += "StatusOn=" + alarms[i].statusOn + "-";
            alarmString += "Difficulty=" + alarms[i].level + "-";
            alarmString += "Days=" + alarms[i].days + "-";
            alarmString += "ID=" + alarms[i].identification;
            alarmString += "\n"
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

router.route('/alarm/set')
  .post((req, res) => {
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

    console.log(days);

    for (let i = 0; i < days.length; i++) {
      if (days[i] == 'on') {
        isDaySet = true;
        break;
      }
    }

    if (!isDaySet) {
      var now = new Date();

      console.log(now);

      var index = now.getDay();
      days[index] = 'on';
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
          statusOn: true,
        });

        newAlarm.save((err, object) => {
          if (err) throw err;
        });
      }
      else {
          alarm.time = alarmTime;
          alarm.days = days;
          alarm.level = diff_level;

          alarm.save((err, object) => {
            if (err) throw err;
          });
      }

      res.redirect('/');
    });

	// TODO: set alarm through C application
  });


router.route('/alarm/delete')
  .post((req, res, next) => {
    var id = xssFilters.inHTMLData(req.body.alarmId);
    console.log("ALARM: " + id);

    Alarm.findOne({'identification': id}).exec((err, alarm) => {
      if (err || alarm == null) {
        throw err;
      } else {
          alarm.remove();
      }

      res.redirect('/');
    });

  });

router.route('/alarm/status')
  .post((req, res, next) => {
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
          });
      }
    });

  });

module.exports = router;
