var express = require('express');
var xssFilters = require('xss-filters');
var Alarm = require('../models/Alarm');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
    Alarm.find({}, function(err, alarms) {
	    if (err){ 
	    	throw err;
	    } 
	    else {
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

    days.push(xssFilters.inHTMLData(req.body.sunday));
    days.push(xssFilters.inHTMLData(req.body.monday));
    days.push(xssFilters.inHTMLData(req.body.tuesday));
    days.push(xssFilters.inHTMLData(req.body.wednesday));
    days.push(xssFilters.inHTMLData(req.body.thursday));
    days.push(xssFilters.inHTMLData(req.body.friday));
    days.push(xssFilters.inHTMLData(req.body.saturday));

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

module.exports = router;
