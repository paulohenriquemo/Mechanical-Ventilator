

// ****************************************** Inputs and outputs configuration **************************************************


int StartInspirationPin = A0;    // select the input pin for the StartInspiration sensor (LDR)
int MinPressurePin = A1;            // select the input pin for the Minimum pressure sensor (LDR)
int MaxPressurePin = A2;    // select the input pin for the EndInspiration sensor (LDR)
int HighPressurePin = A3;    // select the input pin for the High Pressure sensor (LDR)
int PotentiometerRateIEPin = A4;    // select the input pin for the potentiometer that adjust the inspiration/expiration rate
int PotentiometerRatePin = A5;    // select the input pin for the potentiometer that adjust the respiratory rate
int SupplyVoltagePin = A7;          // Select input pin for monitoring Supply voltage

int SwitchPin = 12;               // select the input pin for the Calibration Switch
int AlarmPin = 13;               // select one output pin for Alarm. ( Some Buzzer, light or Both )
int IntakePin = 10;      // select the output pin for the intake solenoid
int ExhaustPin = 11;    // select the output pin for the exhaust solenoid
int LedHPPin = 7;    // select the output pin for LED that iluminate de StartInspiration sensor (LDR)
int LedEIPin = 6;    // select the output pin for LED that iluminate de EndInspiration sensor (LDR)
int LedMPPin = 5;    // select the output pin for LED that iluminate de High Pressure sensor (LDR)
int LedSIPin = 4;    // select the output pin for LED that iluminate de Minimum Pressure sensor (LDR)

// ****************************************** Variables used in the code **************************************************

int PotentiometerValue = 0;  // variable to store the value coming from the potentiometer
float RespiratoryRate = 10; // variable to store the value of Respiratory Rate in Breaths per minute
int RespiratoryTime = 5000; // variable to store the value of Respiratory Time in miliseconds
int InsuficientPressureError = 0;   // variable to Count the number of cycles that desired pressure was not reached
int HighPressureError = 0;          // variable to Count the number of times that High Pressure sensor has triggered
int treshold_StartInspiration = 177;      //  Adjust With serial Monitor
int treshold_MinPressure = 417;          //  Adjust With serial Monitor
int treshold_MaxPressure = 407 ;        //  Adjust With serial Monitor
int treshold_HighPressure = 495 ;      //  Adjust With serial Monitor
int treshold_SupplyVoltage = 928 ;    //  Adjust With serial Monitor

unsigned long  CicleTime = 0;
unsigned long InspiratoryTime = 0;
float RateIE = 50;
boolean flag_StartInspiration = 0;
boolean flag_EndInspiration = 0;
boolean flag_HighPressure = 0;
boolean flag_DetectPressure = 1;
boolean flag_DetectTimeout = 1;

boolean calibration_complete = 0;
boolean Switch = 0;
boolean AlarmState = 0;



// ****************************************** Setup **************************************************

void setup() {
// Serial.begin (9600);

  pinMode(AlarmPin, OUTPUT);
  pinMode(ExhaustPin, OUTPUT);
  pinMode(IntakePin, OUTPUT);
  pinMode(LedSIPin, OUTPUT);
  pinMode(LedEIPin, OUTPUT);
  pinMode(LedHPPin, OUTPUT);
  pinMode(LedMPPin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
 digitalWrite(LedSIPin, HIGH); 
 digitalWrite(LedEIPin, HIGH); 
 digitalWrite(LedHPPin, HIGH); 
 digitalWrite(LedMPPin, HIGH);
 digitalWrite(AlarmPin, HIGH);
 delay(800);
 digitalWrite(AlarmPin, LOW);
 delay(200);


}

void loop() {


// ****************************************** Use Serial Monitor only for debug than disable **************************************************

  

//   delay(500);
//  Serial.print ("Start Inspiration "); Serial.println (analogRead (StartInspirationPin));
//  Serial.print ("Min Pressure "); Serial.println (analogRead (MinPressurePin));
//  Serial.print ("Max Pressure "); Serial.println (analogRead (MaxPressurePin));
//  Serial.print ("High Pressure "); Serial.println (analogRead (HighPressurePin));
//Serial.print ("Bateria "); Serial.println (analogRead (A7));

//  Serial.println ();
 


// ****************************************** Read Sensors and potentiometer **************************************************


if ((millis() - CicleTime) > ( 0.5 * RespiratoryTime)) {                                // Condition to avoid multiple flag trigger 
  if (analogRead (StartInspirationPin) > treshold_StartInspiration){                   // Check if Start Inspiration sensor has triggered
     flag_StartInspiration = 1;
     digitalWrite(LedSIPin, LOW);
  delay(200);
  digitalWrite(LedSIPin, HIGH);
      }}
if(flag_DetectPressure){
if (analogRead (MaxPressurePin) < treshold_MaxPressure){                          // Check if End Inspiration sensor has triggered
  digitalWrite(LedEIPin, LOW); 
  flag_EndInspiration = 1;
  flag_DetectPressure = 0;
 
} 
}
  

 if (analogRead (HighPressurePin) < treshold_HighPressure){                        // Check if High Pressure sensor has triggered
  digitalWrite(IntakePin, LOW);                                                   // Close Intake Solenoid
 digitalWrite(ExhaustPin, HIGH);                                                 // Open Exhaust Solenoid
  digitalWrite(LedHPPin, LOW);
  delay(150);
  digitalWrite(LedHPPin, HIGH);
digitalWrite(AlarmPin, HIGH);
  delay(1000);
  digitalWrite(AlarmPin, LOW);
  
  }




 PotentiometerValue = analogRead(PotentiometerRatePin);
 RespiratoryRate = map(PotentiometerValue,0,1023,40,400);             // Ajust the scale of potentiometer




 RespiratoryTime = (60000 / RespiratoryRate)*10 ;                        // Calculates the time for each cicle in milliseconds
 
 PotentiometerValue = analogRead(PotentiometerRateIEPin); 
 RateIE = map(PotentiometerValue,0,1024,2000,5000);             // Ajust the scale of potentiometer

 
InspiratoryTime = (RespiratoryTime / RateIE * 1000);                     // Calculates the inspiratory time for each cicle in milliseconds






// ****************************************** Execute respiratory cicle **************************************************




  
 if((millis() - CicleTime) > RespiratoryTime ) {                      // Start Inspiration cicle if "flag_StartInspiration" has not been triggered  (Timeout)
  flag_StartInspiration = 1;
 }
 


if(flag_StartInspiration) {
  
digitalWrite(IntakePin, HIGH);                                 // Open Intake Solenoid
digitalWrite(ExhaustPin, LOW);                                // Close exaust Solenoid in case of "flag_EndExpiration" has not been Triggered
CicleTime = millis();
flag_StartInspiration = 0;
flag_DetectPressure = 1;
flag_DetectTimeout = 1;

}
if(flag_DetectTimeout){
if((millis() - CicleTime) > InspiratoryTime) {         // End Inspiration Cicle if "flag_EndInspiration" has not been Triggered
  flag_EndInspiration = 1;
}
}


if(flag_EndInspiration) {
  
digitalWrite(IntakePin, LOW);                                // Close Intake Solenoid
digitalWrite(ExhaustPin, HIGH);                              // Open Exhaust Solenoid
flag_EndInspiration = 0;
flag_DetectTimeout = 0;
delay(150);
digitalWrite(LedEIPin, HIGH);                              // turn de LED on for the next cicle

 if (analogRead (MinPressurePin) < treshold_MinPressure){      // Acidental disconection alarm reset
  digitalWrite(AlarmPin, LOW);
  }


if (analogRead (MinPressurePin) > treshold_MinPressure){      // Acidental disconection alarm
  digitalWrite(AlarmPin, HIGH);
  digitalWrite(LedMPPin, LOW);
  delay(250);
  digitalWrite(LedMPPin, HIGH);
   }

if (analogRead (SupplyVoltagePin) < treshold_SupplyVoltage){                        // Check the Battery Voltage
  digitalWrite(AlarmPin, HIGH);
  delay(200);
  digitalWrite(AlarmPin, LOW);}

  
}



  
// ****************************************** Calibrate LDR Sensors  **************************************************



if (digitalRead(SwitchPin) == LOW) {                  
  digitalWrite (AlarmPin, HIGH);
  delay(1000);
   digitalWrite (AlarmPin, LOW);
   delay (500);


  

int Max_StartInspiration;
int Min_StartInspiration;
int Max_MinPressure;
int Min_MinPressure;
int Max_MaxPressure;
int Min_MaxPressure;
int Max_HighPressure;
int Min_HighPressure;

Max_StartInspiration = analogRead (StartInspirationPin);
Max_MinPressure = analogRead (MinPressurePin);
Max_MaxPressure = analogRead (MaxPressurePin);
Max_HighPressure = analogRead (HighPressurePin);

  digitalWrite (AlarmPin, HIGH);
  delay(200);
   digitalWrite (AlarmPin, LOW);
   digitalWrite(LedEIPin, LOW); 
   digitalWrite(LedSIPin, LOW);
   digitalWrite(LedMPPin, LOW);
   digitalWrite(LedHPPin, LOW);
   delay(200);   
   digitalWrite(LedEIPin, HIGH); 
   digitalWrite(LedSIPin, HIGH);
   digitalWrite(LedMPPin, HIGH);
   digitalWrite(LedHPPin, HIGH);

 delay(10000);


Min_StartInspiration = analogRead (StartInspirationPin);
Min_MinPressure = analogRead (MinPressurePin);
Min_MaxPressure = analogRead (MaxPressurePin);
Min_HighPressure = analogRead (HighPressurePin);



treshold_StartInspiration = (Max_StartInspiration + Min_StartInspiration) / 2;
treshold_MinPressure = (Max_MinPressure + Min_MinPressure) / 2;
treshold_MaxPressure = (Max_MaxPressure + Min_MaxPressure) / 2;
treshold_HighPressure = (Max_HighPressure + Min_HighPressure) / 2 ;


   digitalWrite(LedEIPin, LOW); 
   digitalWrite(LedMPPin, LOW);
   digitalWrite(LedSIPin, LOW);
   digitalWrite(LedHPPin, LOW);
   delay(200);   
   digitalWrite(LedEIPin, HIGH); 
   digitalWrite(LedMPPin, HIGH);
   digitalWrite(LedSIPin, HIGH);
   digitalWrite(LedHPPin, HIGH);
   digitalWrite (AlarmPin, HIGH);
   delay(100);
   digitalWrite (AlarmPin, LOW);
   delay(400);
   digitalWrite (AlarmPin, HIGH);
   delay(100);
   digitalWrite (AlarmPin, LOW);
   delay(400);
   digitalWrite (AlarmPin, HIGH);
   delay(100);
   digitalWrite (AlarmPin, LOW);
   delay (8000);
   
  }

}
