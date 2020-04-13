const logger = require('pino')();
const pushsafer = require('pushsafer-notifications');
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');

// Load app config
const config = require('config');

// Create push client
const pushsaferClient = new pushsafer({
  k: config.get("PushsaferNotifications.privateKey"),
  debug: true 
});

// Initialize serial port reader / parser
const port = new SerialPort(config.get("Serial.port"), { baudRate: config.get("Serial.baudRate") });
const parser = port.pipe(new Readline({ delimiter: '\n' }));

// Global variables
var latestNotificationAt = new Date() - config.get("App.minIntervalBetweenNotifications");

function notifySomeoneIsRinging() {
  var msg = {
    m: "Quelqu'un sonne à la porte d'entrée !",
    t: "Interphone"
  };
  pushsaferClient.send(msg, function(err, result) {
    if (err) {
      logger.error("notifySomeoneRinging, error=%o", err);
    } else {
      logger.info("notifySomeoneIsRinging, result=%o", result);
    }
  });
}

//
// Main
//
logger.info("starting app");

// Read the port data
port.on('open', () => {
  logger.info('serial port open');
});

// Parse serial messages
parser.on('data', data => {
  logger.debug('got word from arduino:', data);
  if (data == "RINGING") {
    var timeSinceLastRingingMessage = new Date() - latestNotificationAt;
    if (timeSinceLastRingingMessage > config.get("App.minIntervalBetweenNotifications")) {
      notifySomeoneIsRinging();
      latestNotificationAt = new Date();
    }
  }
});
