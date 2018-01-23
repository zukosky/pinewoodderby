#define beforeStartSensor 1
#define passingStartSensor 2
#define pastStartSensor 3
#define passingFinishSensor 4
#define pastFinishSensor 5

#define TRUE 1
#define FALSE 0
int carLength = 7;
int trackLength = 20*12; //Was 18 feet

int lightPinStart = 0;
int lightPinFinish = 1;
int nBaselineAvgStart = 0;
int nTriggerValStart=0;
int nBaselineAvgFinish = 0;
int nTriggerValFinish=0;
int nCurrentState=0;
float fTolerance = .15;
//*****************************************************************
int nDebugLevel = 1;
//*****************************************************************
unsigned long ulPrevTime;
unsigned long ulThisTime;
unsigned long ulDiffTime;
unsigned long ulPassingStartSensorEntryTime;
unsigned long ulPastStartSensorEntryTime;
unsigned long ulPassingFinishSensorEntryTime;
unsigned long ulPastFinishSensorEntryTime;

void setup() {
  // Setup the serial port
  int sensorValue;
  int nBaselineMeasurements = 10;
  int nVal[nBaselineMeasurements];
  int nSum = 0;
  char strToPrint[50];
  char strVal[50];

  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  logMsg("Serial port is ready",TRUE);

  nBaselineAvgStart = establishBaseline(lightPinStart);
  // Calculate a trigger value while we're here
  nTriggerValStart = (1 - fTolerance) * nBaselineAvgStart;

  //Repeat for the finish sensor
  nBaselineAvgFinish = establishBaseline(lightPinFinish);
  nTriggerValFinish = (1 - fTolerance) * nBaselineAvgFinish;

  // Let the monitor know
  printDescValPair("BaselineStart value: ", nBaselineAvgStart);
  printDescValPair("BaselineFinish value: ", nBaselineAvgFinish);  
  printDescValPair("TriggerStart value: ", nTriggerValStart);
  printDescValPair("TriggerFinish value: ", nTriggerValFinish);    

  //Capture the time before we go into the loop (msec since init)
  ulPrevTime = millis();

  // Set the initial state
  nCurrentState = beforeStartSensor;
  logMsg("New state: beforeStart", TRUE);  
logMsg("**********************   READY   ***************************", TRUE);    
}

void loop() {
  int ulStartSensorPassDuration;
  int ulFinishSensorPassDuration;  
  int ulTravelDuration;
  int sensorValue;
  char strVal[50];
  // Cant do floating point prints.  Changing to ips
  int startSpeedIPS;
  int finishSpeedIPS;
  int avgSpeedIPS;
  // Set up the state machine
  switch (nCurrentState) {

  case beforeStartSensor:
      sensorValue  = analogRead(lightPinStart);    
      // Waiting for the sensor value to drop below the trigger value
      if(sensorValue < nTriggerValStart) {
        // Passing the start sensor.  Capture the time and set the next state.
        ulPassingStartSensorEntryTime =  millis();
        nCurrentState = passingStartSensor;  
        logMsg("New state: passingStartSensor", FALSE);
        logMsg("**********************   STARTED   ***************************", TRUE);        
      }    
      break;
    
  case passingStartSensor:
      sensorValue  = analogRead(lightPinStart);        
    // Waiting for the sensor value to rise above the trigger value
        if(sensorValue > nTriggerValStart) {
          ulPastStartSensorEntryTime =  millis();
          nCurrentState = pastStartSensor;
        logMsg("New state: pastStartSensor", FALSE);         
        }
        break;
  case pastStartSensor:
      sensorValue  = analogRead(lightPinFinish);       
      // Waiting for the sensor value to drop below the trigger value again.
      if(sensorValue < nTriggerValFinish) {
        // Passing the finish sensor.  Capture the time and set the next state.
        ulPassingFinishSensorEntryTime =  millis();
        nCurrentState = passingFinishSensor;  
        //logMsg("New state: passingFinishSensor", FALSE);         
      }    
      break;
    
  case passingFinishSensor:
      sensorValue  = analogRead(lightPinFinish);           
    // Waiting for the sensor value to rise above the trigger value    
        if(sensorValue > nTriggerValFinish) {
          ulPastFinishSensorEntryTime =  millis();
          nCurrentState = pastFinishSensor;
        logMsg("New state: pastFinishSensor", FALSE);                      
        }    
      break;       
  case pastFinishSensor:
    // Here we just need to do our calculations and send them out the serial port
    ulStartSensorPassDuration = ulPastStartSensorEntryTime - ulPassingStartSensorEntryTime;
    ulFinishSensorPassDuration = ulPastFinishSensorEntryTime - ulPassingFinishSensorEntryTime;
    ulTravelDuration = ulPassingFinishSensorEntryTime - ulPassingStartSensorEntryTime;   

    printDescValPair("ulPassingStartSensorEntryTime: ", ulPassingStartSensorEntryTime);
    printDescValPair("ulPastStartSensorEntryTime: ", ulPastStartSensorEntryTime);
    printDescValPair("ulPassingFinishSensorEntryTime: ", ulPassingFinishSensorEntryTime);
  printDescValPair("ulPastFinishSensorEntryTime: ", ulPastFinishSensorEntryTime);
    startSpeedIPS = durationToSpeed(ulStartSensorPassDuration, carLength);
    finishSpeedIPS = durationToSpeed(ulFinishSensorPassDuration, carLength);
    avgSpeedIPS = durationToSpeed(ulTravelDuration, trackLength);
//Serial.println(startSpeedIPS);
//Serial.println(finishSpeedIPS);

    printAllVals(startSpeedIPS, finishSpeedIPS, avgSpeedIPS, ulTravelDuration );

    strcpy(strVal,"Start sensor pass duration (mSec): ");
    printDescValPair(strVal, ulStartSensorPassDuration);
   
    strcpy(strVal,"Finish sensor pass duration (mSec): ");   
    printDescValPair(strVal, ulFinishSensorPassDuration);

    strcpy(strVal,"Total travel time (mSec): ");   
    printDescValPair(strVal, ulTravelDuration);
               
    //Wait a little bit and reset
    delay(5000);
    nBaselineAvgStart = establishBaseline(lightPinStart);
    nBaselineAvgFinish = establishBaseline(lightPinFinish);    
   
    nTriggerValStart = (1 - fTolerance) * nBaselineAvgStart;   
    nTriggerValFinish = (1 - fTolerance) * nBaselineAvgFinish;   
    nCurrentState = beforeStartSensor;
  logMsg("**********************   READY   ***************************", TRUE);    
    break;   
  default:
              Serial.println("In DEFAULT.  I SHOULD NEVER GET HERE!!");   
      break;             
  }
}

float durationToSpeed(unsigned long durmSec, int distIn) {
  float locSpeed;
  logMsg("In durationToSpeed", FALSE);
//  Serial.println(distFt);
 // Serial.println(durmSec);  
  locSpeed = distIn / (durmSec / 1000.);
  //Serial.println(locSpeed);    
  return (locSpeed);
}

void printDescValPair(const char *strDesc, unsigned long nVal)
{
  char strToPrint[50];
  char strVal[50];
 
    
  sprintf(strVal, "%d", nVal);
  strcpy(strToPrint, strDesc);
  strcat(strToPrint, strVal);
  logMsg(strToPrint, TRUE);
 
}

void logMsg(const char *strVal, bool bOverride) {
  if (nDebugLevel || bOverride) {
  Serial.println(strVal);
  }
}

int establishBaseline(int nPin) {
  int nBaselineMeasurements = 10;
  int locBaseline;
  int nVal[nBaselineMeasurements];
  int nSum = 0;  
  char strVal[50];  
  logMsg("Establishing baseline....", FALSE);  
  for (int i = 0; i < nBaselineMeasurements; i++)
  {
  nVal[i]  = analogRead(nPin);
  nSum = nSum + nVal[i];
  delay(10);
  }
  // Take an average of the values
  locBaseline = nSum / nBaselineMeasurements;
   
  printDescValPair("Baseline Value: ", locBaseline);  
  return(locBaseline);
}

void printAllVals(int startSpeedIPS,
              int finishSpeedIPS,
              int avgSpeedIPS,
              unsigned long ulTravelDuration ) {

  char strToPrint[500];
  char strVal[500];      
  logMsg("In printallvals", FALSE);
  //Serial.println(startSpeedIPS);
  //Serial.println(finishSpeedIPS);  
  sprintf(strVal, "%d,%d,%d,%d", startSpeedIPS, finishSpeedIPS, avgSpeedIPS, ulTravelDuration );
      Serial.println(strVal);
  sprintf(strVal, "Start Speed (IPS): %d, Finish Speed (IPS): %d, Avg Speed (IPS): %d, Travel Duration (mSec): %d", startSpeedIPS, finishSpeedIPS, avgSpeedIPS, ulTravelDuration );
      Serial.println(strVal);      
}





