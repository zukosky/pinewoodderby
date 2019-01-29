int lightPinStart = 0;
int lightPinFinish = 1;
int nDebugLevel=1;
/*
setup()    Sets the transfer rate on the USB connection and waits until the port is ready. 
*/
void setup() {
  // Setup the serial port
  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo

      Serial.println("Serial port is ready");
  //logMsg("Serial port is ready");
}

/*
loop()   Infinite loop for monitoring the two ADC inputs corresponding to the start and finish photoresistors.
Note: This has to be called "loop()".  
*/
void loop() {

int valStart;
int valFinish;

 //Serial.println("In interrupt");
valStart  = analogRead(lightPinStart);      
valFinish  = analogRead(lightPinFinish);      
//Serial.println(valStart);  
//printDescValPair("Start Sensor Value    : ", valStart);  
//printDescValPair("                                      Finish Sensor Value    : ", valFinish);  
printReadings("Start Sensor Value    : %d               Finish Sensor Value    : %d", valStart, valFinish);
//Wait a little bit and read again.
delay(500);
}

void printDescValPair(const char *strDesc, int nVal) 
{
  char strToPrint[500];
  char strVal[500];
    
  sprintf(strVal, "%d", nVal);
  strcpy(strToPrint, strDesc);
  strcat(strToPrint, strVal);
  //Send the string out to the serial monitoring window
  Serial.println(strToPrint);
  
}

void printReadings(const char *strDesc, int nStartVal, int nEndVal) 
{
  char strToPrint[500];
  char strStartVal[500];
  char strEndVal[500];  
    
//  printf(strDesc, nStartVal, nEndVal);
// sprintf(strEndVal, "%d", nEndVal);  
 // strcpy(strToPrint, strDesc);
//  strcat(strToPrint, strVal);
  //Send the string out to the serial monitoring window
//  Serial.println(strToPrint);
    sprintf(strToPrint, strDesc, nStartVal, nEndVal);
    Serial.println(strToPrint);
  
}

void logMsg(const char *strVal) {
  if (nDebugLevel) {
    Serial.println(strVal);
  }
}


