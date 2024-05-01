#include <Servo.h>    // Include the library for servo motor  
#include <avr/wdt.h> // Include the library for resetting  
Servo myservo;     
int trig = 7;    
int echo = 8;   
int servoPin = 9;

long duration, distance;   
long a[3]; 
bool objectDetected = false;
unsigned long objectStartTime = 0; //Variable to denote the start time when the object is encountered
unsigned long objectDuration = 0;  //Variable to store the durationfor which the object stays

void setup() {
    Serial.begin(9600);               // Initialize serial communication
    while (!Serial);                  // Wait for Serial Monitor to open
    myservo.attach(servoPin);  
    pinMode(trig, OUTPUT);  
    pinMode(echo, INPUT);  
    myservo.write(90);                // start the servo position with 90 degree      
    delay(1000);                      // 1 second for the servo motor to settle
    myservo.detach();
}

void measure() {                      //Function to measure the object object distance
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    pinMode(echo, INPUT);
    duration = pulseIn(echo, HIGH);
    distance = (duration/2) / 29.1;    
}

void resetArduino() {
    wdt_disable(); // Disable watchdog timer
    wdt_enable(WDTO_15MS); // Enable watchdog timer with a short timeout
    while (1); // Wait for the watchdog timer to reset the Arduino
}


void loop() { 
    for (int i = 0; i <= 2; i++) {   
        measure();               
        a[i] = distance;            
        delay(50);              
    }
    distance = (a[0] + a[1] + a[2]) / 3;    //To get the distance accurately


    if (distance < 15 && !objectDetected) {            // Object detected and no previous detection
        objectDetected = true;
        objectStartTime = millis(); //Object encounter time
    }
    else if (distance >= 15 && objectDetected) {      // Object removed after detection
        objectDetected = false;
        objectDuration = millis() - objectStartTime;  //Measuring object duration
        Serial.print("Object duration: ");
        Serial.print(objectDuration);
        Serial.println(" ms");

        myservo.attach(servoPin);
        delay(2000);                            //Delay of 2s after the object removal
        myservo.write(180);                     //Change the position from 90 to 180
        if(objectDuration<=10000){
          delay(5000);                          // Wait for 5 seconds before going back to 90 degrees
          myservo.write(90);                    // servo position 90 degree
          delay(5000);
          myservo.detach();
        }else{
          delay(10000);                         //Wait for 10seconds before going back to 90 degrees
          myservo.write(90);                    // servo position 90 degree
          delay(10000);
          myservo.detach();
        }
        resetArduino(); // Reset the Arduino after each loop execution
    }
    
}
