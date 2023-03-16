#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

/*
Podłączenia:

pin 2 - odbiornik rf433

pin 7 - przycisk lewy
pin 8 - przycisk prawy

pin 4 - CS czytnik kart SD
pin 11 - MOSI czytnik kart SD
pin 12 - MISO czytnik kart SD
pin 13 - CLK czytnik kart SD

pin A4 - SDA wyświetlacz lcd
pin A5 - SCL wyświetlacz lcd

*/


RCSwitch mySwitch = RCSwitch();

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin1 = 7;
const int buttonPin2 = 8;

bool button1State = 0;
bool button2State = 0;

int MenuState = 0;

long ReceivedValue = 0;
int ReceivedDelay = 0;
int ReceivedBitlength = 0;
int ReceivedProtocol = 0;
int ReceivedRawdata = 0;

void setup() {
  InitializeScreen();
  InitializeButtons();
  DisplayIntro();
  mySwitch.enableReceive(0);
}

void loop() {
  button1State = digitalRead(buttonPin1);
  button2State = digitalRead(buttonPin2);
  DisplayMainMenu();
  ShowButtonResponse();

  button1State = digitalRead(buttonPin1);
  button2State = digitalRead(buttonPin2);

  //jeżeli wciśniemy przycisk 1, przechodzimy na kolejny poziom
  if (button1State == 0) {
    delay(10);
    MenuState = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning");
  }

  while (MenuState == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Scanning");

    if (mySwitch.available()) {

      ReceivedValue = mySwitch.getReceivedValue();
      ReceivedBitlength = mySwitch.getReceivedBitlength();
      ReceivedProtocol = mySwitch.getReceivedProtocol();
      ReceivedDelay = mySwitch.getReceivedDelay();
      ReceivedRawdata = mySwitch.getReceivedRawdata();

      lcd.setCursor(0, 0);
      lcd.print("D:");
      lcd.setCursor(2, 0);
      lcd.print(ReceivedValue);
      lcd.setCursor(0, 1);
      lcd.print("BL:");
      lcd.setCursor(3, 1);
      lcd.print(ReceivedBitlength);
      lcd.setCursor(8, 1);
      lcd.print("PR:");
      lcd.setCursor(11, 1);
      lcd.print(ReceivedProtocol);
      mySwitch.resetAvailable();
      delay(500);

      File dataFile = SD.open("myData.txt", FILE_WRITE);

      if (dataFile) {
        lcd.clear();
        lcd.print("Saving file");  
        dataFile.println("*****START*****"); 
        dataFile.println("ReceivedValue;ReceivedBitlength;ReceivedProtocol;ReceivedDelay;ReceivedRawdata");
        dataFile.println(String(ReceivedValue)+";"+String(ReceivedBitlength)+";"+String(ReceivedProtocol)+";"+String(ReceivedDelay)+";"+String(ReceivedRawdata));
        dataFile.println("*****END*****");
        dataFile.println(" "); 

        dataFile.close();
        delay(100);
        lcd.setCursor(0,1);
        lcd.print("Done!");
        delay(100);
        lcd.clear();
        
      } else {
        lcd.clear();
        lcd.print("Error saving");
        delay(500);
      }
    }
    delay(200);

    button1State = digitalRead(buttonPin1);
    button2State = digitalRead(buttonPin2);

    //jeżeli wciśniemy przycisk 2, wracamy do poprzedniego poziomu
    if (button2State == 0) {
      MenuState = 0;
    }
  }
}

//all required functions

void InitializeButtons() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

void InitializeScreen() {
  lcd.init();  //initialize LCD
  lcd.backlight();
}

void DisplayIntro() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("433MHz Sniffer");
  delay(2000);
  lcd.clear();

  if (!SD.begin(4)) {
    lcd.print("Error SD Card");
  }
  lcd.print("SD Card OK!");

  delay(1000);
  lcd.clear();
  lcd.print("Ready!");
  delay(1000);
  lcd.clear();
}

void DisplayMainMenu() {
  MenuState = 0;
  lcd.setCursor(0, 0);
  lcd.print("Select option");
  lcd.setCursor(0, 1);
  lcd.print("Scan");
}

void ShowButtonResponse() {

  button1State = digitalRead(buttonPin1);
  button2State = digitalRead(buttonPin2);

  if (button1State == 0) {
    lcd.setCursor(0, 1);
    lcd.print("XXXXX");
    delay(50);
    lcd.clear();
  }
  if (button2State == 0) {
    lcd.setCursor(11, 1);
    lcd.print("XXXXX");
    delay(50);
    lcd.clear();
  }
}