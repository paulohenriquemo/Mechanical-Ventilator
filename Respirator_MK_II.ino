

int SwitchPin = 13;               // select the input pin for the Calibration Switch
int AlarmPin = 12;               // select one output pin for Alarm. ( Some Buzzer, light or Both )
int PotentiometerRatePin = A5;    // select the input pin for the potentiometer that adjust the respiratory rate
int PotentiometerRateIEPin = A4;    // select the input pin for the potentiometer that adjust the inspiration/expiration rate
int StartInspirationPin = A1;    // select the input pin for the StartInspiration sensor (LDR)
int EndInspirationPin = A2;    // select the input pin for the EndInspiration sensor (LDR)
int HighPressurePin = A3;    // select the input pin for the High Pressure sensor (LDR)
int IntakePin = 10;      // select the output pin for the intake solenoid
int ExhaustPin = 11;    // select the output pin for the exhaust solenoid
int LedSIPin = 7;    // select the output pin for LED that iluminate de StartInspiration sensor (LDR)
int LedEIPin = 6;    // select the output pin for LED that iluminate de EndInspiration sensor (LDR)
int LedHPPin = 5;    // select the output pin for LED that iluminate de High Pressure sensor (LDR)

int PotentiometerValue = 0;  // variable to store the value coming from the potentiometer
float RespiratoryRate = 10; // variable to store the value of Respiratory Rate in Breaths per minute
int RespiratoryTime = 5000; // variable to store the value of Respiratory Time in miliseconds
int InsuficientPressureError = 0;   // variable to Count the number of cycles that desired pressure was not reached
int HighPressureError = 0;          // variable to Count the number of times that High Pressure sensor has triggered
int treshold_StartInspiration = 450;
int treshold_EndInspiration = 420;
int treshold_HighPressure = 230 ;

unsigned long  CicleTime = 0;
unsigned long InspiratoryTime = 0;
float RateIE = 50;
boolean flag_StartInspiration = 0;
boolean flag_EndInspiration = 0;
boolean flag_HighPressure = 0;
boolean flag_DetectPressure = 1;

boolean calibration_complete = 0;
boolean Switch = 0;
boolean AlarmState = 0;


void setup() {
 //Serial.begin (9600);

  pinMode(AlarmPin, OUTPUT);
  pinMode(ExhaustPin, OUTPUT);
  pinMode(IntakePin, OUTPUT);
  pinMode(LedSIPin, OUTPUT);
  pinMode(LedEIPin, OUTPUT);
  pinMode(LedHPPin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
 digitalWrite(LedSIPin, HIGH); 
 digitalWrite(LedEIPin, HIGH); 
 digitalWrite(LedHPPin, HIGH); 
 delay(1000);


}

void loop() {


// ****************************************** Use Serial Monitor only for debug than disable **************************************************

  

 //  delay(500);
 // Serial.print ("Start Inspiration "); Serial.println (analogRead (StartInspirationPin));
 //  Serial.print ("End Inspiration "); Serial.println (analogRead (EndInspirationPin));
 // Serial.print ("High Pressure "); Serial.println (analogRead (HighPressurePin));
 // Serial.println ();
 


// ****************************************** Read Sensors and potentiometer **************************************************


if ((millis() - CicleTime) > ( 0.5 * RespiratoryTime)) {                                // Condition to avoid multiple flag trigger 
  if (analogRead (StartInspirationPin) > treshold_StartInspiration){                   // Check if Start Inspiration sensor has triggered
     flag_StartInspiration = 1;
     digitalWrite(LedSIPin, LOW);
  delay(200);
  digitalWrite(LedSIPin, HIGH);
      }}
if(flag_DetectPressure){
if (analogRead (EndInspirationPin) < treshold_EndInspiration){                          // Check if End Inspiration sensor has triggered
  digitalWrite(LedEIPin, LOW); 
  flag_EndInspiration = 1;
  flag_DetectPressure = 0;
 // InsuficientPressureError = 0;                                                       // Reset the error count by pressure not reached
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

}
 
if((millis() - CicleTime) > InspiratoryTime) {         // End Inspiration Cicle if "flag_EndInspiration" has not been Triggered
  flag_EndInspiration = 1;

}


if(flag_EndInspiration) {
  
digitalWrite(IntakePin, LOW);                                // Close Intake Solenoid
digitalWrite(ExhaustPin, HIGH);                              // Open Exhaust Solenoid
flag_EndInspiration = 0;
delay(150);
digitalWrite(LedEIPin, HIGH);                              // turn de LED on for the next cicle


if (analogRead (StartInspirationPin) > treshold_StartInspiration){      // Acidental disconection alarm
  digitalWrite(AlarmPin, HIGH);
  delay(1000);
  digitalWrite(AlarmPin, LOW); }
}



  
// ****************************************** Calibrate LDR Sensors  **************************************************



if (digitalRead(SwitchPin) == LOW) {                  // Triggers de alarm if High pressure was reached in 5 cicles
  digitalWrite (AlarmPin, HIGH);
  delay(1000);
   digitalWrite (AlarmPin, LOW);
   delay (500);


  

int Max_StartInspiration;
int Min_StartInspiration;
int Max_EndInspiration;
int Min_EndInspiration;
int Max_HighPressure;
int Min_HighPressure;

Max_StartInspiration = analogRead (StartInspirationPin);
Max_EndInspiration = analogRead (EndInspirationPin);
Max_HighPressure = analogRead (HighPressurePin);

  digitalWrite (AlarmPin, HIGH);
  delay(200);
   digitalWrite (AlarmPin, LOW);
   digitalWrite(LedEIPin, LOW); 
   digitalWrite(LedSIPin, LOW);
   digitalWrite(LedHPPin, LOW);
   delay(200);   
   digitalWrite(LedEIPin, HIGH); 
   digitalWrite(LedSIPin, HIGH);
   digitalWrite(LedHPPin, HIGH);

 delay(10000);


Min_StartInspiration = analogRead (StartInspirationPin);
Min_EndInspiration = analogRead (EndInspirationPin);
Min_HighPressure = analogRead (HighPressurePin);



treshold_StartInspiration = (Max_StartInspiration + Min_StartInspiration) / 2;
treshold_EndInspiration = (Max_EndInspiration + Min_EndInspiration) / 2;
treshold_HighPressure = (Max_HighPressure + Min_HighPressure) / 2 ;


   digitalWrite(LedEIPin, LOW); 
   digitalWrite(LedSIPin, LOW);
   digitalWrite(LedHPPin, LOW);
   delay(200);   
   digitalWrite(LedEIPin, HIGH); 
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
