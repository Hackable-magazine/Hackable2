// Article Baromètre BMP180 - Hackable 2

#include <LiquidCrystal.h>
#include <SFE_BMP180.h>
#include <Wire.h>

#define LCD_RS 2
#define LCD_E 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

#define ALTITUDE 165.0

LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

SFE_BMP180 pressure;

long previousMillis = 0;
int firstrun = 1;
double logpression[96];

void setup() {
  pinMode(8, OUTPUT);
  lcd.begin(16, 2);
  if (!pressure.begin()) {
    lcd.print("ERREUR BMP180");
    while(1);
  }
}

void logpush(double P) {
  for(int i=95; i>0; i--) {
    logpression[i] = logpression[i-1];
  }
  logpression[0] = P;
}

void printdelta(int cellule) {
  char texte[6];
  if(logpression[cellule]==0) {
    lcd.print("____");
    return;
  }
  dtostrf(logpression[0]-logpression[cellule], 4, 0, texte);
  lcd.print(texte);
}

void loop() {
  char status;
  double T,P,p0;
  char ligne[16];

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > (300000UL) || firstrun) {
    digitalWrite(8, HIGH);
    lcd.clear();
    status = pressure.startTemperature();
    if(status != 0) {
      delay(status);
      status = pressure.getTemperature(T);
      if (status != 0) {
        delay(status);
        status = pressure.startPressure(3);
        if(status != 0) {
          delay(status);
          status = pressure.getPressure(P,T);
          if(status != 0) {
            p0 = pressure.sealevel(P,ALTITUDE);
            dtostrf(T, 5, 2, ligne);
            lcd.print(ligne);
            dtostrf(p0, 11, 2, ligne);
            lcd.print(ligne);
            lcd.setCursor(0, 1);
            logpush(p0);
            printdelta(60/5);  // 1h
            printdelta(120/5); // 2h
            printdelta(240/5); // 4h
            printdelta(95); // 8h
          }
        }
      }
    }
    if(firstrun) firstrun=0;
    previousMillis = currentMillis;
    digitalWrite(8, LOW);
  }
}

