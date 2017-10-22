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
    var diff_level;

    days.push(xssFilters.inHTMLData(req.body.sunday));
    days.push(xssFilters.inHTMLData(req.body.monday));
    days.push(xssFilters.inHTMLData(req.body.tuesday));
    days.push(xssFilters.inHTMLData(req.body.wednesday));
    days.push(xssFilters.inHTMLData(req.body.thursday));
    days.push(xssFilters.inHTMLData(req.body.friday));
    days.push(xssFilters.inHTMLData(req.body.saturday));

    diff_level = xssFilters.inHTMLData(req.body.level);

    const alarm = new Alarm({
      time: alarmTime,
      days: days,
      level: diff_level,
    });

    alarm.save((err, posting) => {
      if (err) throw err;
    });

	// TODO: set alarm through C application

    res.redirect('/');
  });

module.exports = router;
