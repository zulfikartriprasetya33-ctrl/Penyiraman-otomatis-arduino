#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Keypad.h>

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi RTC
RTC_DS1307 rtc;

// Pemetaan Pin Keypad 4x4
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin Sensor Kelembaban Tanah
const int pinSensor1 = A0; 
const int pinSensor2 = A1; 

// Pin Relay Pompa Air
const int pinRelay1 = 10; 
const int pinRelay2 = 11; 

const int batasPenyiraman = 30; 

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Penyiraman ");
  lcd.setCursor(0, 1);
  lcd.print(" Otomatis ");
  delay(2000);
  lcd.clear();

  if (!rtc.begin()) {
    lcd.print("RTC Tdk Ditemukan!");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);

  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, HIGH);
}

void loop() {
  int nilaisensor1 = analogRead(pinSensor1);
  int nilaisensor2 = analogRead(pinSensor2);

  int lembab1 = map(nilaisensor1, 1023, 0, 0, 100);
  int lembab2 = map(nilaisensor2, 1023, 0, 0, 100);

  lembab1 = constrain(lembab1, 0, 100);
  lembab2 = constrain(lembab2, 0, 100);

  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(lembab1);
  lcd.print("% ");

  lcd.setCursor(8, 0);
  lcd.print("S2:");
  lcd.print(lembab2);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());

  if (lembab1 < batasPenyiraman) {
    digitalWrite(pinRelay1, LOW); 
    lcd.setCursor(7, 1);
    lcd.print("P1:ON ");
  } else {
    digitalWrite(pinRelay1, HIGH); 
    lcd.setCursor(7, 1);
    lcd.print("P1:OFF");
  }

  if (lembab2 < batasPenyiraman) {
    digitalWrite(pinRelay2, LOW);  
  } else {
    digitalWrite(pinRelay2, HIGH); 
  }

  char key = keypad.getKey();
  if (key) {
    Serial.print("Tombol Ditekan: ");
    Serial.println(key);
  }

  delay(1000);
}
