#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the LCD library

// Pin Definitions
#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 3

// Create instances for RFID reader, servo and LCD
MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the 0x27 address based on LCD's I2C address

// Define where servo should be when locking/unlocking
#define LOCKED_POSITION 0
#define UNLOCKED_POSITION 90

// List of NUIDs for authorized access
byte knownNUIDs[][4] = {
  {0xC6, 0xF6, 0x0, 0x3},
  {0x9A, 0xBC, 0xDE, 0xF0}
};

// var tracking lock status
bool isLocked = true;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Initialize SPI bus for RFID reader
  rfid.PCD_Init(); // Initialize the RFID reader

  lockServo.attach(SERVO_PIN); // Attach the servo to its  defined control pin
  lockServo.write(LOCKED_POSITION); // Initialize servo to defined locked position

  // Initialize the LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready to scan...");

  Serial.println("Ready to scan RFID cards...");
}

void loop() {
  // Check if a new RFID card is present
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Attempt to read the card's data
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Check if the card is authorized
  if (isAuthorizedCard(rfid.uid.uidByte)) {
    // Handle unlocking and locking mechanism
    if (isLocked) {
      Serial.println("Authorized card detected! Unlocking...");
      lockServo.write(UNLOCKED_POSITION);
      lcd.clear();
      lcd.print("Unlocked");
      isLocked = false;
    } else {
      Serial.println("Locking again...");
      lockServo.write(LOCKED_POSITION);
      isLocked = true;
      lcd.clear();
      lcd.print("Locked");
      delay(1000);
      lcd.clear();
      lcd.print("Ready to scan...");
    }
  } else {
    // Handle unauthorized card detection
    Serial.println("Warning: unauthorized card detected.");
    lcd.clear();
    lcd.print("Unauthorized");
    delay(2000);
    lcd.clear();
    lcd.print(isLocked ? "Ready to scan..." : "Unlocked");
  }

  // Halt the RFID reader for the next card
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool isAuthorizedCard(byte *cardID) {
  // Check if the presented card ID matches any known NUIDs
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

void printInHex(byte *data, byte length) {
  // Print the card's ID in hexadecimal format
  for (byte i = 0; i < length; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
}

void printInDecimal(byte *data, byte length) {
  // Print the card's ID in decimal format
  for (byte i = 0; i < length; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
}
