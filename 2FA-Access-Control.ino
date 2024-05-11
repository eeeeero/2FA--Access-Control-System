// Include required libraries
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
// Create instances
LiquidCrystal lcd(11, 12, 26, 28, 30, 32);
int Contrast=75;  

MFRC522 mfrc522(53, 5); // MFRC522 mfrc522(SS_PIN, RST_PIN)


char initial_password[4] = {'1', '2', '3', '4'};  // Variable to store initial password
String tagUID = "E7 C0 38 1B";  // String to store UID of tag. Change it with your tag's UID
char password[4];   // Variable to store users password
char userchoice[1];
volatile boolean RFIDMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
int i = 0;  // Variable used for counter
// defining how many rows and columns our keypad have
const byte rows = 4;
const byte columns = 4;
// Keypad pin map
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Initializing pins for keypad
byte row_pins[rows] = {35, 33, 31, 29};
byte column_pins[columns] = {27, 25, 23};
// Create instance for keypad
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);


void setup() {
  // Arduino Pin configuration
  Serial.begin(9600);
  pinMode(42, OUTPUT); //PIN LUZ BLANCA
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(3, OUTPUT); // PIN SERVO
  pinMode(10, OUTPUT);
  analogWrite(10,Contrast); 
  RFIDMode = true;
  lcd.begin(16,2);   // LCD screen
  digitalWrite(42, HIGH);
  digitalWrite(43, HIGH);
  digitalWrite(44,HIGH);
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  lcd.setCursor(0, 0);
  lcd.print("Access Control");
  lcd.setCursor(0, 1);
  lcd.print("Micro Prjt");
  delay(5000);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(3, HIGH);
  lcd.clear(); // Clear LCD screen
}
void loop() {


  // System will first look for mode
  if (RFIDMode == true) {
    digitalWrite(3, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("  Door Lock");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Tag");
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    //Reading from the card
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();

    Serial.println(tag);

    //Checking the card
    if (tag.substring(1) == tagUID)
    {
      // If UID of tag is matched.
      lcd.clear();
      lcd.print("Tag Matched");
      digitalWrite(42, HIGH);
      delay(3000);
      digitalWrite(42, LOW);
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0, 1);
      RFIDMode = false; // Make RFID mode false
    }
    else
    {
      // If UID of tag is not matched.
      lcd.clear();
      digitalWrite(43, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Wrong Tag Shown");
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
   
      delay(3000);
     
      lcd.clear();
      digitalWrite(43, LOW);
    }
  }
  // If RFID mode is false, it will look for keys from keypad
  if (RFIDMode == false) {
    key_pressed = keypad_key.getKey(); // Storing keys
    if (key_pressed)
    {
      password[i++] = key_pressed; // Storing in password variable
      lcd.print("*");
    }
    if (i == 4) // If 4 keys are completed
    {
      delay(200);
      if (!(strncmp(password, initial_password, 4))) // If password is matched
      {
        lcd.clear();
        lcd.print("Pass Accepted");
        digitalWrite(3, LOW); // Door Opened
        digitalWrite(44, HIGH);
        delay(10000);
        
        digitalWrite(44, LOW);
        digitalWrite(3, HIGH); // Door Closed
        i = 0;
        RFIDMode = true; // Make RFID mode true
      }
      else    // If password is not matched
      {
        lcd.clear();
        lcd.print("Wrong Password");
        digitalWrite(43, HIGH);
        
        delay(3000);
        lcd.clear();
        digitalWrite(43, LOW);
        i = 0;
        RFIDMode = true;// Make RFID mode true
     
      }

    }

  }
}

// Funciones no logramos implementar

void resetSystem(){   //NOT APPLIED
  RFIDMode = true;
  i = 0;
  loop();
}

void changePassword() { //NOT APPLIED

        lcd.setCursor(0, 0);
        lcd.print("Enter new");
        lcd.setCursor(0, 1);
        lcd.print("password:");

        // Wait for user input
        char newPassword[5];
        char getInputPinpad(newPassword, 4); // Function to get input from pinpad

        // Update the initial password with the new password
        strncpy(initial_password, newPassword, 4);

        // Confirmation message
        lcd.clear();
        lcd.print("Password");
        lcd.setCursor(0, 1);
        lcd.print("changed!");
        delay(2000);

        digitalWrite(42, LOW);
         digitalWrite(3, HIGH); // Door Closed
         i = 0;
         RFIDMode = true; // Make RFID mode true
         loop();
}