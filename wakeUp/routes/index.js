var express = require('express');
var xssFilters = require('xss-filters');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { 
  	title: 'Wake Up' 
  });
});

router.route('/alarm/set')
  .post((req, res) => {
    var time = xssFilters.inHTMLData(req.body.time);

    var days = [];
    var level =[];

    days.push(xssFilters.inHTMLData(req.body.sunday));
    days.push(xssFilters.inHTMLData(req.body.monday));
    days.push(xssFilters.inHTMLData(req.body.tuesday));
    days.push(xssFilters.inHTMLData(req.body.wednesday));
    days.push(xssFilters.inHTMLData(req.body.thursday));
    days.push(xssFilters.inHTMLData(req.body.friday));
    days.push(xssFilters.inHTMLData(req.body.saturday));

    level.push(xssFilters.inHTMLData(req.body.level_easy));
    level.push(xssFilters.inHTMLData(req.body.level_med));
    level.push(xssFilters.inHTMLData(req.body.level_hard));

    console.log(time);
    console.log(days);
    console.log(level);

    // TODO: set alarm through C application

    res.redirect('/');
  });

module.exports = router;
