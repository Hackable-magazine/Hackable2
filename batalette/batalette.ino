// Article Boite à Lettre qui Tweet - Hackable 2
// Nécessite un shield Ethernet

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Twitter.h>

#define DELTA 20

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192,168,111,11);
EthernetClient client;

int ldrnow[5] = {0,0,0,0,0};
int ldrbase[5] = {1023,1023,1023,1023,1023};

// Mettre ici votre token 
Twitter twitter(">> TOKEN <<");
char msg[141];

int tweeted = 0;


void setup() {
  randomSeed(analogRead(5));
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(7, INPUT_PULLUP);
  
  if (!Ethernet.begin(mac)) {
    Serial.println("DHCP erreur !");
    Ethernet.begin(mac, ip);
    delay(2000);
  }
  Serial.println(Ethernet.localIP());
}

void blinky() {
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);
  delay(150);
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);
  delay(1000);
}

void calib() {
  Serial.println("Debut calibration/reset tweeter");
  blinky();
  digitalWrite(8, HIGH);
  delay(50);
  ldrbase[0] = analogRead(0);
  ldrbase[1] = analogRead(1);
  ldrbase[2] = analogRead(2);
  ldrbase[3] = analogRead(3);
  ldrbase[4] = analogRead(4);
  delay(50);
  digitalWrite(8, LOW);
  delay(1000);
  blinky();
  tweeted=0;
  Serial.println("Fin calibration/reset tweeter");
}

void loop() {
  int status;

  if(digitalRead(7) == LOW) 
    calib();
  digitalWrite(8, HIGH);
  delay(50);
  ldrnow[0] = analogRead(0);
  ldrnow[1] = analogRead(1);
  ldrnow[2] = analogRead(2);
  ldrnow[3] = analogRead(3);
  ldrnow[4] = analogRead(4);
  delay(50);
  digitalWrite(8, LOW);

  for(int i=0; i<5; i++) {
    if(ldrnow[i] > ldrbase[i]+DELTA) {
      ldrnow[i] = 1;
    } else {
      ldrnow[i] = 0;
    }
  }

  Serial.print(ldrnow[0]);
  Serial.print(' ');
  Serial.print(ldrnow[1]);
  Serial.print(' ');
  Serial.print(ldrnow[2]);
  Serial.print(' ');
  Serial.print(ldrnow[3]);
  Serial.print(' ');
  Serial.print(ldrnow[4]);
  Serial.print(' ');
  
  if(ldrnow[0] | ldrnow[1] | ldrnow[2] | ldrnow[3] | ldrnow[4]) {
    Serial.println("y'a du courrier !");
    if(tweeted == 0) {
      sprintf(msg, "y'a du courrier dans la boite (%lu) ", random(2000000000));
      if (twitter.post(msg)) {
        status = twitter.wait();
        if (status == 200) {
          Serial.println("tweet OK");
        } else {
          Serial.println("erreur tweeter");
        }
      }
      tweeted=1;
    } else {
      Serial.println("c'est deja tweete");
    }
  } else {
    Serial.println("...");
  }
  
  delay(2000);
}
