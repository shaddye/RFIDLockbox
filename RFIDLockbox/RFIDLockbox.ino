// program reading new NUID from a rfid tag, accordingly locking servo motor if key is in list of known keys
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

byte knownNUIDs[][4] = {
  {0x12, 0x34, 0x56, 0x78},  // Ex NUID
  {0x9A, 0xBC, 0xDE, 0xF0}   // Ex2 NUID
  
};


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

  if (!rfid.PICC_ReadCardSerial()) // checks if RFID successfully read card's NUID
    return;

  Serial.print("Card type: "); // displays type of card presented 
  Serial.println(rfid.PICC_GetTypeName(rfid.PICC_GetType(rfid.uid.sak)));

  if (isAuthorizedCard(rfid.uid.uidByte)) { // determines whether RFID tag is in the list of known NUIDs
    Serial.println("Authorized card detected!");

    lockServo.write(UNLOCKED_POSITION); // unlock the servo (grants access)
    delay(15000);  // motor stays retracted for 15 seconds
    lockServo.write(LOCKED_POSITION);  // locks again

    Serial.print("Card ID in hex: ");       // lines execute when card is scanned and its NUID is different from last card scanned.
    printInHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.print("Card ID in decimal: ");    // lines execute when card is scanned and its NUID is different from last card scanned.
    printInDecimal(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  } else {
    Serial.println("Unauthorized card. Access denied.");
  }

  rfid.PICC_HaltA(); // RFID reader prepares for next card
  rfid.PCD_StopCrypto1();
}


bool isAuthorizedCard(byte *cardID) {
  for (uint8_t i = 0; i < sizeof(knownNUIDs) / sizeof(knownNUIDs[0]); i++) {
    bool match = true;
    for (uint8_t j = 0; j < 4; j++) {
      if (cardID[j] != knownNUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
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
