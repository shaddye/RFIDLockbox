# RFIDLockbox
RFID Lockbox with Arduino Uno
This guide shows how I built an RFID lockbox using an Arduino Uno, RFID reader, servo, and an LCD display. The box unlocks when an authorized RFID card is scanned and relocks with the same card.

Materials Needed
Arduino Uno
MFRC522 RFID Reader
RFID cards/tags
Servo Motor (SG90 or similar)
16x2 I2C LCD Display
Jumper wires
Breadboard
Wiring
RFID Reader (MFRC522)
VCC: 3.3V
GND: Ground
SDA: Pin 10
SCK: Pin 13
MOSI: Pin 11
MISO: Pin 12
RST: Pin 9
Servo Motor
Control Pin: Pin 3
VCC: 5V
GND: Ground
LCD Display (I2C)
SDA: A4
SCL: A5
VCC: 5V
GND: Ground
Installing Libraries
MFRC522 (RFID)
LiquidCrystal_I2C (LCD)
Install them from Arduino's library manager.

Code Overview
The code checks for RFID cards. If the card’s UID matches one in the knownNUIDs list, it unlocks or locks the servo, and the LCD shows the status.

isAuthorizedCard: Verifies if the scanned card is authorized.
knownNUIDs: List of authorized card UIDs.
LOCKED_POSITION and UNLOCKED_POSITION: Servo positions for lock and unlock.
Uploading the Code
Copy the code into the Arduino IDE.
Select Arduino Uno and your port in the Tools menu.
Upload the code.
Testing
Open the Serial Monitor (9600 baud).
Scan your RFID card; the UID will print in the monitor.
Add your card’s UID to the knownNUIDs list in this format:
cpp
Copy code
{0xXX, 0xXX, 0xXX, 0xXX}
Test locking and unlocking.
That’s it! You now have an RFID-controlled lockbox.
