/*
Simulate RSSI voltage from Bridgewave microwave radio.
Assumption: RSSI value is from Bridgewave FLEX4g-LITE radio.
RSSI min = 1.0V -> step 205 -> RSL min = -80dBm
RSSI max = 4.0V -> step 819 -> RSL max = -20dBm
RSSI to RSL realtionship is linearised ... (y = mx + c)

Add random variable component to an average signal strength.
Output is PWM, convert to DC by RC filter.

James Ryan
2019-11-13

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const float RSSI_MIN = 1.0;     // volts
const float RSSI_MAX = 4.0;     // volts
const float RSL_MIN = -80.0;    // dBm
const float RSL_MAX = -20.0;    // dBm

const float VAR_RSL_MIN = -62.1;
const float VAR_RSL_MAX = -42.1;

const int DAC_BITS = 8;         // DAC resolution
const float DAC_RANGE = 5.0;    // volts

// calculated constants
float slope = (RSL_MAX - RSL_MIN) / (RSSI_MAX - RSSI_MIN);    // rise over run
float intercept = RSL_MAX - (slope * RSSI_MAX);               // c = y - mx, given m, y_max, x_max 
float dac_step_value = DAC_RANGE / pow(2, DAC_BITS);          // equivalent voltage of DAC step
  
float rsl;
int level;

// prototypes
float random_rsl(float, float);
int rsl_to_step(float, float, float, float);

// Output pin for RSSI (PWM)
const int RSSI = 9;


void setup() {
  // initialize digital pin as an output.
  pinMode(RSSI, OUTPUT);
}

void loop() {
  // random variation RSLs between limits
  rsl = random_rsl(VAR_RSL_MIN, VAR_RSL_MAX);
  level = rsl_to_step(rsl, slope, intercept, dac_step_value);
  analogWrite(RSSI, level);

  //delay(2);      // trim to get frequency 1.8kHz
}

float random_rsl(float min, float max) {
  // generate a random float between a min and max
  
  return  ((max - min) * ((float) rand() / (float) RAND_MAX)) + min ;
}

int rsl_to_step(float rsl, float m, float c, float ssv) {
  // Convert Received Signal Level in dBm to DAC step.

  // float, float, float, float -> int
  // Receives: RSL in dBm, slope, intercept, single step value
  // Returns : DAC output step

  // RSL to RSSI is linear relation ... re-arrange y = mx + c for x
  
  float rssi;
  int val;
  
  rssi = (rsl - c) / m;          // x = (y - c) / m 
  val = round(rssi / ssv);
  
  return val;
}
