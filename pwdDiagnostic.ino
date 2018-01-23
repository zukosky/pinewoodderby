int lightPinStart = 0;
int lightPinFinish = 1;
int nDebugLevel=1;

void setup() {
  // Setup the serial port
  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo

      Serial.println("Serial port is ready");
  //logMsg("Serial port is ready");
}

void loop() {

int valStart;
int valFinish;

 //Serial.println("In interrupt");
  valStart  = analogRead(lightPinStart);      
valFinish  = analogRead(lightPinFinish);      
//Serial.println(valStart);  
printDescValPair("Start Sensor Value    : ", valStart);  
printDescValPair("                                      Finish Sensor Value    : ", valFinish);  

      //Wait a little bit and reset
      delay(500);
}

void printDescValPair(const char *strDesc, int nVal) 
{
  char strToPrint[500];
  char strVal[500];
    
  sprintf(strVal, "%d", nVal);
  strcpy(strToPrint, strDesc);
  strcat(strToPrint, strVal);
  Serial.println(strToPrint);
  
}

void logMsg(const char *strVal) {
  if (nDebugLevel) {
    Serial.println(strVal);
  }
}


