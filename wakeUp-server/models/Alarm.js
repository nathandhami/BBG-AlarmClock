const mongoose = require('mongoose');
const autoIncrement = require('mongoose-auto-increment');

const db = mongoose.connection;
autoIncrement.initialize(db);

const AlarmSchema = new mongoose.Schema({
  time: String,
  days: [],
  level: String,
  question: String,
  statusOn: Boolean,
  identification: Number,
});

AlarmSchema.virtual('alarmId')
  .get(function() {
    return 'alarm-' + this.identification;
  });

AlarmSchema.plugin(autoIncrement.plugin, {model: 'Alarm', field: 'identification', startAt: 1});
module.exports = mongoose.model('Alarm', AlarmSchema);
