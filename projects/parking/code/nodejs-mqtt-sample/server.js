var express = require('express');
var index = require('./index');
var cors = require('cors');

var app = express();

app.use(cors());

app.get('/', function (req, res) {
    res.sendFile(__dirname + '/ws.html');
})

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
})