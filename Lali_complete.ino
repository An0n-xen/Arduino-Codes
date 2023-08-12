#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>


#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myservo;
int analogPin = A1;


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Variables
String numbers = "";
String Password = "12345";
bool n_check = false;
bool isOpened = false;
int open = 0;
int close = 180;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  lcd.init();                       
  lcd.backlight();

  myservo.attach(analogPin);
  myservo.write(0);

  lcd.setCursor(3,0);
  lcd.print("Enter PIN");
  lcd.setCursor(5,1);
  Serial.begin(9600);
}

bool isFive(){
  if (numbers.length() == 4){
    return true;
  }
  else{
    return false;
  }
}

void OpenCloseDoor(){
  if(isOpened){
    myservo.write(close);
    isOpened = false;
  }else{
    myservo.write(open);
    isOpened = true;
  }
}

void pinNum(char key){
  if (numbers.length() <= 5){
    numbers += key;
    lcd.write(key);
  }
}

void changePassword(){
  numbers = "";
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter Old PIN");
  lcd.setCursor(5,1);

  while(true){
    char key = keypad.getKey();

    if (key){
      n_check = isFive();
      Serial.println(n_check);
      pinNum(key);

      if(n_check){
        if(numbers == Password){
          lcd.clear();
          savePassword();
          break;
        }else{
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Incorrect");
          numbers = "";
          changePassword();
          break;
        }
      }

    }
  }
}

void getUID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "53 F4 FC 11") //change here the UID of the card/cards that you want to give access
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Access granted");
    OpenCloseDoor();
    delay(1000);
    checkPassword();
  }
 
 else   {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Access denied");
    delay(1000);
    checkPassword();
  }

}

void checkPassword(){
  numbers = "";
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Enter PIN");
  lcd.setCursor(5,1);

  while(true){
    char key = keypad.getKey();

    getUID();

    if (key){
      n_check = isFive();
      Serial.println(n_check);
      pinNum(key);

      if(n_check){
        if(numbers == Password){
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Access granted");
          Serial.println("Access granted");
          OpenCloseDoor();
          checkPassword();
        }else if(numbers == "00000"){
          changePassword();
        }
        else{
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Incorrect password");
          delay(1000);
          checkPassword();
        }
      }
    }
  }
}
                      
void savePassword(){
  numbers = "";
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter New PIN");
  lcd.setCursor(5,1);

  while(true){
    char key = keypad.getKey();

    if (key){
      n_check = isFive();
      Serial.println(n_check);
      pinNum(key);

      if(n_check){
        Password = numbers;
        lcd.setCursor(0,0);
        lcd.print("Password Changed");
        delay(1000);
        lcd.clear();
        checkPassword();
      }
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  // char key = keypad.getKey();

  // if (key){
  //   n_check = isFive();
  //   pinNum(key);

  //   if (numbers == "00000"){
  //     changePassword();
  //   }
  // }

  checkPassword();
  
}
