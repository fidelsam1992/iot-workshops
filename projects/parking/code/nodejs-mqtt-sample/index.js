var mqtt = require('mqtt');
var client  = mqtt.connect('mqtt://mosquitto:1883');
var mongoose = require('mongoose');
var Counter = require('./models/Counter');
const dotenv = require("dotenv");
const env = dotenv.config();
var WebSocketServer = require('ws').Server;

// variables for counting the number of entering cars
var counter = 0;
var lastTimeChecked = 0;
var interval = 5000;
var sensorFlags = [false, false, false, false, false];


// connect to mongoDB instance
mongoose.connect(`mongodb://${env.parsed['USER']}:${env.parsed['PASSWORD']}@${env.parsed['DB']}`, { useNewUrlParser: true });
mongoose.connection.once('open', () => {
  console.log("connected to db");
  Counter.collection.findOne(function(err, storedCounter) {
    if (err) throw err;
    counter = storedCounter.counter;
  });
}, (err) => {
  console.log("ERR --> ", err);
});

// WebSocketServer init and config
wss = new WebSocketServer({ port: 40510 });

// re-connect mechanism included
function connect() {
  wss.on('connection', function (ws, err) {
    ws.on('error', function() {
      console.log('socket error in server');
    });
    ws.on('close', function() {
        console.log('socket close in server');
        clearInterval(intervalId);
        console.log('Reconnecting..');
        setTimeout(connect, 2000);
        console.log('Calling connect method');
    });
    var intervalId = setInterval(() => {
      ws.send(counter);
    }, 2000);  
  })
}
connect();

// connect to sensor topics to the MQTT server
client.on('connect', function () {
  client.subscribe('sensor1', function (err) {
    if (err) throw err;
    console.log('subscribed to sensor1');
    client.subscribe('sensor2', function(err) {
      console.log('subscribed to sensor2');
      client.subscribe('sensor3', function(err) {
        console.log('subscribed to sensor3');
        client.subscribe('sensor4', function(err) {
          console.log('subscribed to sensor4');
        });
      });
    });
  });
})

/**
 * on receiving a message from a topic, this method contains the logic 
 * for counting the entering/exiting cars depending on the order of 
 * sent sensor signals
 */
client.on('message', function (topic, message) {

  // message is Buffer
  console.log('Topic: ', topic, ', message: ', message.toString())
  
  var currentEnter = new Date().getTime();
  var currentExit = new Date().getTime();
  
  if (currentEnter - lastTimeChecked > interval) {
    sensorFlags[1] = false;
    sensorFlags[2] = false;
  }

  if (currentExit - lastTimeChecked > interval) {
    sensorFlags[3] = false;
    sensorFlags[4] = false;
  }
  

  // handling sensor1 and sensor2 signals
  if (!sensorFlags[1] && !sensorFlags[2] && topic == 'sensor1') {
      sensorFlags[1] = true;
      lastTimeChecked = currentEnter;
  } else if ((currentEnter - lastTimeChecked < interval) && sensorFlags[2] && topic == 'sensor1') {
      updateCounter(--counter);
      sensorFlags[2] = false; 
  } else if (!sensorFlags[1] && !sensorFlags[2] && topic == 'sensor2') {
      sensorFlags[2] = true;
      lastTimeChecked = currentEnter;
  } else if ((currentEnter - lastTimeChecked < interval) && sensorFlags[1] && topic == 'sensor2') {
      updateCounter(++counter);
      sensorFlags[1] = false; 
  }

  // handling sensor3 and sensor4 signals
  if (!sensorFlags[3] && !sensorFlags[4] && topic == "sensor3") {
    sensorFlags[3] = true;
    lastTimeChecked = currentExit;
  } else if ((currentExit - lastTimeChecked < interval) && sensorFlags[4] && topic == "sensor3") {
      updateCounter(++counter);
      sensorFlags[4] = false;
  } else if (!sensorFlags[3] && !sensorFlags[4] && topic == "sensor4") {
      sensorFlags[4] = true;
      lastTimeChecked = currentExit;
  } else if ((currentExit - lastTimeChecked < interval) && sensorFlags[3] && topic == "sensor4") {
      updateCounter(--counter);
      sensorFlags[3] = false; 
  }

  // uncomment to debug counter value
  // console.log('COUNTER --> ', counter);

  // uncomment when needed to receive only one message!
  // client.end()
})

// update counter in mongoDB instance
function updateCounter(counter) {
  Counter.findOneAndUpdate({}, { $set: { counter: counter } }, {new: true}, (err, doc) => {
    if (err) {
        console.log("Something wrong when updating data!");
    }
    console.log(doc);
  });
}