#include <FirmataDefines.h>
#include <FirmataConstants.h>
#include <Boards.h>
#include <Firmata.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

 /*
 * Detect intercom buzzer by analyzing audio level from electrect microphone
 * 
 */
const int DEFAULT_SAMPLE_WINDOW = 100; // Sample window width in mS
const double DEFAULT_MIN_MILLI_VOLTS = 3.0; // Value required to trigger a positive
const int DEFAULT_NUMBER_OF_HITS = 8; // Requires being above threshold for NUMBER_OF_CONFIRMATIONS times in a row

unsigned int value;
unsigned int minNumberOfHits;
unsigned int n = 0;
int sampleWindow;
double minMilliVolts;

void sysexCallback(byte command, byte argc, byte *argv)
{
  switch(command) {
  case 0x01: // Setup detection params
    if(argc < 3) break;
    byte sample_window; // value in 10 ms, e.g., 25 => 250 ms
    byte min_milli_volts; // value x 100, e.g. 32 => 3200 mV
    byte number_of_hits;
    sample_window = argv[0];
    min_milli_volts = argv[1];
    number_of_hits = argv[2];
    //
    sampleWindow = sample_window * 10;
    minMilliVolts = min_milli_volts * 100;
    minNumberOfHits = number_of_hits;
  default:
    break;
  }
  Firmata.sendSysex(command, argc, argv);
}

void setup() 
{
  // Variable init with defaults
  sampleWindow = DEFAULT_SAMPLE_WINDOW;
  minMilliVolts = DEFAULT_MIN_MILLI_VOLTS;
  minNumberOfHits = DEFAULT_NUMBER_OF_HITS;
  //
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.begin(57600);
}

void loop() 
{
 unsigned long start = millis();  // Start of sample window
 unsigned int peakToPeak = 0;   // peak-to-peak level
 unsigned int signalMax = 0;
 unsigned int signalMin = 655; //1024;
 
 while (Firmata.available()) {
   Firmata.processInput();
 }


 
 // collect data for SAMPLE_WINDOW miliseconds
 while (millis() - start < sampleWindow)
 {
   value = analogRead(0);
      if (value < 1024)  //This is the max of the 10-bit ADC so this loop will include all readings
      {
         if (value > signalMax)
         {
           signalMax = value;  // save just the max levels
         }
      else if (value < signalMin)
        {
          signalMin = value;  // save just the min levels
        }
     }
 }
 peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
 double volts = (peakToPeak * 3300) / 655; // 1024;  // convert to volts
 if (volts > minMilliVolts) {
  n = n + 1;
  if (n >= minNumberOfHits) {
    n = 0;
    Firmata.sendString("RINGING");  
  }
 }
}
