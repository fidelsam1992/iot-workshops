var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var counterSchema = Schema({
    counter: Number
});

var Counter = mongoose.model('Counter', counterSchema);
module.exports = Counter;