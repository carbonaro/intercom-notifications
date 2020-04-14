 /*
 * Detect intercom buzzer by analyzing audio level from electrect microphone
 * 
 */
const int DEFAULT_SAMPLE_WINDOW = 150; // Sample window width in mS
const double DEFAULT_MIN_VOLTS = 3.0; // Value required to trigger a positive
const int DEFAULT_NUMBER_OF_HITS = 6; // Requires being above threshold for NUMBER_OF_CONFIRMATIONS times in a row

unsigned int value;
unsigned int minNumberOfHits;
unsigned int n = 0;
int sampleWindow;
double minVolts;

void setup() 
{
  // Variable init with defaults
  sampleWindow = DEFAULT_SAMPLE_WINDOW;
  minVolts = DEFAULT_MIN_VOLTS;
  minNumberOfHits = DEFAULT_NUMBER_OF_HITS;
  //
  Serial.begin(57600);
}

void loop() 
{
 unsigned long start = millis();  // Start of sample window
 unsigned int peakToPeak = 0;   // peak-to-peak level
 unsigned int signalMax = 0;
 unsigned int signalMin = 655; //1024;
 
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
 double volts = (peakToPeak * 3.3) / 655; // 1024;  // convert to volts
 if (volts > minVolts) {
  n = n + 1;
  if (n >= minNumberOfHits) {
    n = 0;
    Serial.println("RINGING");
  }
 } else {
  n = 0;
 }
}
