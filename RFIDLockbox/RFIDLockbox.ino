#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  // Include the LCD library

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 3

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

#define LOCKED_POSITION 0
#define UNLOCKED_POSITION 180

byte knownNUIDs[][4] = {
  {0xC6, 0xF6, 0x0, 0x3},
  {0x9A, 0xBC, 0xDE, 0xF0}
};

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjusts address based on LCD's I2c address

bool isLocked = true; // var to track lock state

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_POSITION);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready to scan...");

  Serial.println("Ready to scan RFID cards...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  if (isAuthorizedCard(rfid.uid.uidByte)) {
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
    Serial.println("Warning: unauthorized card detected.");
    lcd.clear();
    lcd.print("Unauthorized");
    delay(2000); 
    lcd.clear();
    lcd.print(isLocked ? "Ready to scan..." : "Unlocked");
  }

  rfid.PICC_HaltA();
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

void printInHex(byte *data, byte length) {
  for (byte i = 0; i < length; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
}

void printInDecimal(byte *data, byte length) {
  for (byte i = 0; i < length; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
}
