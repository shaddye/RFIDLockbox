// program reading new NUID from a rfid tag.
// written in C++, this works fine as a base but we need to ensure
//  servo motor connection works simultaenously with RFID reader FIRST AND FOREMOST
// from there we can focus on adding the additional features like 
// SMS notifications, and LED displaying unlocked/locked,

#include <SPI.h>
#include <MFRC522.h> //RFID reader lib
#include <Servo.h>  // servo motor(locking mechanism) lib

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 7  // Change this to the pin your servo is connected to

MFRC522 rfid(SS_PIN, RST_PIN);  // create RFID object
Servo lockServo;                // creates servo object


#define LOCKED_POSITION 0   // // Positions for the servo motor, Aadjust as needed
#define UNLOCKED_POSITION 90  // Adjust as needed

byte lastCardID[4]; // This array stores the ID of the last card read

void setup() {
  Serial.begin(9600);
  SPI.begin(); /// Initializes RFID reader
  rfid.PCD_Init();

  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_POSITION);  // starts the servo motor in the locked position

  Serial.println("Ready to scan RFID cards...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) // checks if new card is present
    return;

  if (!rfid.PICC_ReadCardSerial()) // checks if RFID sucessfully read cards NUID
    return;

  Serial.print("Card type: "); // displays type of card presented 
  Serial.println(rfid.PICC_GetTypeName(rfid.PICC_GetType(rfid.uid.sak)));

  if (isNewCard(rfid.uid.uidByte)) { // determines whether RFID tag is unique or previously used
    Serial.println("New card detected!");

    
    lockServo.write(UNLOCKED_POSITION); // unlock the servo (grants access to treasure)
    delay(5000);  // motor stays retracted for 5 seconds
    lockServo.write(LOCKED_POSITION);  // locks again

    for (byte i = 0; i < 4; i++) {
      lastCardID[i] = rfid.uid.uidByte[i]; // stores last RFID card scanned as lastCardID.
    }

    Serial.print("Card ID in hex: ");       // lines execute when card is scanned and its NUID is different from last card scanned.
    printInHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.print("Card ID in decimal: ");
    printInDecimal(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  } else {
    Serial.println("This card was read previously.");
  }

  rfid.PICC_HaltA(); // RFID reader prepares for next card
  rfid.PCD_StopCrypto1();
}

bool isNewCard(byte *cardID) { // checks if scanned card in ID is different from previous
  for (byte i = 0; i < 4; i++) {
    if (cardID[i] != lastCardID[i]) {
      return true;
    }
  }
  return false;
}

void printInHex(byte *data, byte length) { // prints ID in hexadecimal 
  for (byte i = 0; i < length; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
}

void printInDecimal(byte *data, byte length) { // prints cards ID in decimmal
  for (byte i = 0; i < length; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
}
