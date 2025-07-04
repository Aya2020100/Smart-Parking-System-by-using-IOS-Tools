#include <Servo.h>
#include <LiquidCrystal.h>

// Define pins for ultrasonic sensors and servo motor
#define trigPin1 9   // Entrance sensor TRIG pin
#define echoPin1 10   // Entrance sensor ECHO pin
#define trigPin2 11   // Exit sensor TRIG pin
#define echoPin2 12   // Exit sensor ECHO pin
#define servoPin 3    // Servo motor pin

// Define pins for LCD display
#define rs 4
#define en 5
#define d4 6
#define d5 7
#define d6 8
#define d7 13

Servo myServo;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Parking space variables
const int totalSpaces = 4; // Total number of parking spaces available
int occupiedSpaces = 0;      // Number of occupied spaces

void setup() {
    Serial.begin(9600);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    
    myServo.attach(servoPin);
    myServo.write(0); // Start with gate closed

    lcd.begin(16, 2); // Initialize the LCD display
    lcd.print("Parking Spaces:");
}

void loop() {
    long duration1, distance1;
    long duration2, distance2;

    // Entrance sensor reading
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);

    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = (duration1 / 2) * 0.0343;

    // Exit sensor reading
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);

    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = (duration2 / 2) * 0.0343;

    // Check entrance sensor for vehicle detection
    if (distance1 < 10 && occupiedSpaces < totalSpaces) { // Adjust this distance as needed 
        Serial.println("Car detected at entrance! Opening gate...");
        openGate(); // Open the gate when a car is detected at entrance
        
        occupiedSpaces++; // Increment occupied spaces count
        
        updateLCD(); // Update LCD display with new count
        
        delay(5000); // Keep gate open for a while before closing it 
        closeGate(); // Close the gate after some time 
    } else {
        Serial.println("No car detected at entrance.");
    }

    // Check exit sensor for vehicle detection 
    if (distance2 < 10 && occupiedSpaces > 0) { // If a car is detected at exit and there are occupied spaces 
        Serial.println("Car detected at exit! Opening gate...");
        
        openGate(); // Open the gate when a car is detected at exit
        
        occupiedSpaces--; // Decrement occupied spaces count
        
        updateLCD(); // Update LCD display with new count
        
        delay(5000); // Keep gate open for a while before closing it 
        closeGate(); // Close the gate after some time 
    } else {
        Serial.println("No car detected at exit.");
    }

    delay(500); // Short delay for stability in readings 
}

// Function to open the gate using the servo motor 
void openGate() {
    myServo.write(90); // Move servo to open position (90 degrees)
}

// Function to close the gate using the servo motor 
void closeGate() {
    myServo.write(0); // Move servo back to closed position (0 degrees)
}

// Function to update the LCD display with current parking status 
void updateLCD() {
    lcd.setCursor(0, 1); // Move cursor to second line 
    lcd.print("Available: ");
    
    int availableSpaces = totalSpaces - occupiedSpaces; 
    lcd.print(availableSpaces); 
    
    if (availableSpaces <= 0) { 
        lcd.print(" FULL "); 
    } else { 
        lcd.print("       "); // Clear any leftover characters from previous display 
    }
}
