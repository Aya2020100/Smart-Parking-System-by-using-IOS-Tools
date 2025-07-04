/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h> // Include the Servo library

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
#define SERVO_PIN       6          // Pin connected to the servo motor

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Servo myServo;                     // Create Servo instance

// Define the UID of your authorized RFID card
byte authorizedUID[] = {0x35, 0x7D, 0xE4, 0x00}; // Replace with your card's UID

void setup() {
    Serial.begin(9600);           // Initialize serial communications with the PC
    while (!Serial);              // Do nothing if no serial port is opened (for ATMEGA32U4)
    SPI.begin();                  // Init SPI bus
    mfrc522.PCD_Init();           // Init MFRC522
    delay(4);                     // Optional delay for initialization
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

    myServo.attach(SERVO_PIN);    // Attach servo to pin
    myServo.write(0);              // Start with the door closed (0 degrees)
}

void loop() {
    // Reset the loop if no new card present on the sensor/reader.
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Display card UID
    Serial.print(F("Card UID: "));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Check if the scanned card is authorized
    if (isAuthorized(mfrc522.uid.uidByte)) {
        Serial.println(F("Access granted. Opening door..."));
        openDoor();
        delay(5000); // Keep the door open for 5 seconds
        closeDoor();
        Serial.println(F("Door closed."));
    } else {
        Serial.println(F("Access denied."));
    }

    // Halt PICC
    mfrc522.PICC_HaltA();
}

// Function to check if the scanned UID matches the authorized UID
bool isAuthorized(byte *uid) {
    for (byte i = 0; i < 4; i++) {
        if (uid[i] != authorizedUID[i]) {
            return false;
        }
    }
    return true;
}

// Function to open the garage door
void openDoor() {
    myServo.write(90); // Move servo to 90 degrees to open door
}

// Function to close the garage door
void closeDoor() {
    myServo.write(0); // Move servo back to 0 degrees to close door
}


