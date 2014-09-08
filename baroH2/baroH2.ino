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

// Altitude en mètres
#define ALTITUDE 165.0

LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

SFE_BMP180 pressure;

long previousMillis = 0;
int firstrun = 1;
double logpression[96];

void setup() {
  // On a mit une led sur la broche 8 pour
  // signaler une mesure
  pinMode(8, OUTPUT);
  lcd.begin(16, 2);
  if (!pressure.begin()) {
    lcd.print("ERREUR BMP180");
    while(1);
  }
}

// Enregistrement de la pression 
// et décalage des valeur dans le tableau
void logpush(double P) {
  for(int i=95; i>0; i--) {
    logpression[i] = logpression[i-1];
  }
  logpression[0] = P;
}

// Affichage LCD de la différence de pression
// entre la cellulle 0 (maintenant) et une cellule 
// passée en argument
void printdelta(int cellule) {
  char texte[6];
  if(logpression[cellule]==0) {
    // il n'y a rien dans la cellule, pas de variation
    // a calculer
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
    // allumage led
    digitalWrite(8, HIGH);
    // efface LCD
    lcd.clear();
    // prise de température
    status = pressure.startTemperature();
    if(status != 0) {
      // le bmp180 répond, on attend le temps qu'il dit avant de lire la mesure
      delay(status);
      // on récupère la température
      status = pressure.getTemperature(T);
      if (status != 0) {
        delay(status);
	// prise de mesure de pressionn
        status = pressure.startPressure(3);
        if(status != 0) {
	  // pareil, on attend comme il le veux le temps de faire la mesure
          delay(status);
	  // on récupère la valeur compensée avec la température
          status = pressure.getPressure(P,T);
          if(status != 0) {
	    // P est la pression absolue, il faut compenser en fonction de l'altitude
            p0 = pressure.sealevel(P,ALTITUDE);
	    // conversion de la valeur en xxx,xx (5 digit en tout minimum, donc 2 chiffres après la virgule)
            dtostrf(T, 5, 2, ligne);
            lcd.print(ligne);
            dtostrf(p0, 11, 2, ligne);
            lcd.print(ligne);
            lcd.setCursor(0, 1);
	    // on enregistre la pression dans le tableau
            logpush(p0);
	    // on affiche la différence de pression entre maintenant et
            printdelta(60/5);  // 1h
            printdelta(120/5); // 2h
            printdelta(240/5); // 4h
            printdelta(95);    // 8h
          }
        }
      }
    }
    if(firstrun) firstrun=0;
    previousMillis = currentMillis;
    // extinction led
    digitalWrite(8, LOW);
  }
}

