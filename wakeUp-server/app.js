var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var nconf = require('./src/nconf/nConfig');
var mongoose = require('mongoose');
var expressSession = require('express-session');
var MongoStore = require('connect-mongo')(expressSession);
var multer = require('multer');

var index = require('./routes/index');
var modifySound = require('./routes/modifySound');

var app = express();

mongoose.connect(nconf.get('db:url'));
const db = mongoose.connection;
db.on('error', console.error.bind(console, 'mongodb connection error:'));

// Session and Cookie configuration
app.use(expressSession({
  name: 'wakeUpId',
  store: new MongoStore({
    mongooseConnection: mongoose.connection,
    ttl: 1 * 60 * 60, // 1 hr
  }),
  secret: nconf.get('session:secret'),
  resave: false,
  saveUninitialized: false,
}));

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());

app.use(express.static(path.join(__dirname, 'public')));
app.use('/node_modules', express.static(__dirname + '/node_modules'));

app.use(multer({
  dest: './public/uploads/',
}).single("alarm_sound"));

var socketClient = require('socket.io-client')(nconf.get('http:url'));
socketClient.on('connect', function(data) {
  socketClient.emit('handshake');
});
app.set('socketClient', socketClient);

app.use('/alarmSound', modifySound);
app.use('/', index);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

module.exports = app;
