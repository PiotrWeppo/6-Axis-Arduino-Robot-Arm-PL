#include <Adafruit_TiCoServo.h>
#include <PololuMaestro.h>
#include <SoftwareSerial.h>

SoftwareSerial maestroSerial(10, 11);
MicroMaestro maestro(maestroSerial);
Adafruit_TiCoServo servo_6;

int pot0 = A0, pot1 = A1, pot2 = A2, pot3 = A3, pot4 = A4, pot5 = A5;
int led_1 = 12, led_2 = 13, led_3 = 3; 
int przycisk_1 = 4, przycisk_2 = 5, przycisk_3 = 6, przycisk_4 = 7, przycisk_5 = 2;
int buzzer = 8;
int val0, val1, val2, val3, val4, val5;
int stan_led_1 = LOW, stan_led_2 = LOW, stan_led_3 = LOW;        
int stan_przycisku_1, stan_przycisku_2, stan_przycisku_3;
int ost_stan_przycisku_1 = LOW, ost_stan_przycisku_2 = LOW, ost_stan_przycisku_3 = LOW;
unsigned long ost_stan_odbicia_1 = 0, ost_stan_odbicia_2 = 0, ost_stan_odbicia_3 = 0; 
unsigned long opoznienie_odbicia = 50;
int oczek;
int staw_0[15], staw_1[15], staw_2[15], staw_3[15], staw_4[15], staw_5[15], chwytak[15];
int zwrot_chwytaka = 0;
int krok = 0;
int top;
float dlugosc_wcisniecia_przycisku_4 = 0;
int opcja_czasowo_krotsza = 100;
int opcja_czasowo_dluzsza = 600; 

void setup()
{
 maestroSerial.begin(9600);
 Serial.begin(115200);
 pinMode(przycisk_1, INPUT_PULLUP);
 pinMode(przycisk_2, INPUT_PULLUP);
 pinMode(przycisk_3, INPUT_PULLUP);
 pinMode(przycisk_4, INPUT_PULLUP);
 pinMode(przycisk_5, INPUT_PULLUP);
 pinMode(led_1, OUTPUT);
 pinMode(led_2, OUTPUT);
 pinMode(led_3, OUTPUT);
 digitalWrite(led_1, stan_led_1);
 digitalWrite(led_2, stan_led_2);
 digitalWrite(led_3, stan_led_3);
 servo_6.write(150); 
 servo_6.attach(9);
 servo_6.write(150);
 predkosc_ogolna(); 
 przys_ogolne();
 delay(50);
 Serial.println("Robot gotowy do pracy.");

}

void loop()
{  
  ster_pot();
  
  Nasluch_przycisk1();
  Nasluch_przycisk2();
  Nasluch_przycisk3();

  Poz_domowa();
  Skrypt_maestro();
  Chwytak();
  
  nauka_i_powtarzanie_krokow();
}

void nauka_i_powtarzanie_krokow() // Zapisywanie nadanych pozycji w przestrzeni i odtwarzanie ich w petli
{
while (digitalRead(przycisk_4) == LOW)
  {
   delay(100);
   dlugosc_wcisniecia_przycisku_4 = dlugosc_wcisniecia_przycisku_4 + 100;   
  }
  
  if (dlugosc_wcisniecia_przycisku_4 >= opcja_czasowo_dluzsza)
  {
  Serial.print("Odtwarzanie nagranej petli. Ilosc krokow w serii: ");
  Serial.println(top);
  tone(buzzer, 131, 200);
  delay (300);
  tone(buzzer, 147, 200);
  delay (300);
  tone(buzzer, 165, 200);
  delay (1000);
  maestro.setSpeed(0, 60);
  maestro.setSpeed(1, 60);
  maestro.setSpeed(2, 60);
  maestro.setSpeed(3, 60);
  maestro.setSpeed(4, 60);
  maestro.setSpeed(5, 60);
  while (digitalRead(przycisk_5) != LOW)
  {
  if (top < 2 || top >= 16)
  {
  tone(buzzer, 147, 1000);
  Serial.println("Za malo krokow w petli. Powrot do sterowania recznego.");
  predkosc_ogolna();
  break;
  }
  for (krok = 0; krok < top; krok++)
  {
  Serial.print(staw_0[krok]);
  Serial.print("; ");
  Serial.print(staw_1[krok]);
  Serial.print("; ");
  Serial.print(staw_2[krok]);
  Serial.print("; ");
  Serial.print(staw_4[krok]);
  Serial.print("; ");
  Serial.print(staw_5[krok]);
  Serial.print("; ");
  Serial.println(chwytak[krok]);
  
  maestro.setTarget(0, staw_0[krok]);
  maestro.setTarget(1, staw_1[krok]);
  maestro.setTarget(2, staw_2[krok]);
  maestro.setTarget(3, staw_3[krok]);
  maestro.setTarget(4, staw_4[krok]);
  maestro.setTarget(5, staw_5[krok]);
  while (maestro.getMovingState() == 1)
  {
  delay(50);
  }
  servo_6.write(chwytak[krok]);
  delay(1300);
  }
  krok = 0;
  if(digitalRead(przycisk_5) == LOW)
  {
  Serial.println("Powrot do sterowania recznego.");
  tone(buzzer, 165, 200);
  delay (300);
  tone(buzzer, 147, 200);
  delay (300);
  tone(buzzer, 131, 200);
  predkosc_ogolna();
  delay (1500);
  break;
  }
  }
  } 
  else if (dlugosc_wcisniecia_przycisku_4 >= opcja_czasowo_krotsza)
  {
  Serial.print("Nagrano krok nr: ");
  Serial.println(krok + 1);
  staw_0[krok] = val0;
  staw_1[krok] = val1;
  staw_2[krok] = val2;
  staw_3[krok] = val3;
  staw_4[krok] = val4;
  staw_5[krok] = val5;
  if (zwrot_chwytaka == 1)
  chwytak[krok] = 0;
  else if (zwrot_chwytaka == 0)
  {
  chwytak[krok] = 180;
  }
  krok += 1; 
  delay(10);
  top = krok;
  tone(buzzer, 147, 50);
  if (top > 15)
  {
  Serial.println("Przekroczono limit pamieci krokow. Resetowanie pamieci.");
  tone(buzzer, 147, 1000);
  krok = 0;
  top = 0;
  }
  }
dlugosc_wcisniecia_przycisku_4 = 0;  
}

void Poz_domowa() // Jazda do pozycji domowej po wcisnieciu przycisk_1
{
if (stan_led_1 == HIGH)
  {
  Serial.println("Jade do pozycji domowej.");
  jazda_do_poz_domowej();
  delay(500);
  oczek = maestro.getPosition(5); 
  if (oczek == 6292)
    {
  Serial.println("Dojechalem. Oczekuje.");
    }
  while(digitalRead(przycisk_1) != LOW);
    {      
  delay(10);
  stan_led_1 = !stan_led_1;
  digitalWrite(led_1, stan_led_1);
  Serial.println("Powrot do sterowania recznego.");
    }
  }
}
    
void Skrypt_maestro() // Odtwarzanie skryptu nagranego w sterowniku serw
{
   if (stan_led_2 == HIGH)
  {
  maestro.restartScript(0);
  Serial.println("Odtwarzanie skryptu.");
  delay(500);
  while(digitalRead(przycisk_2) != LOW);
    {
  stan_led_2 = !stan_led_2;
  digitalWrite(led_2, stan_led_2);
  Serial.println("Powrot do sterowania recznego.");
  maestro.stopScript();
    }
  }
}

void Chwytak() // Otwieranie lub zamykanie chwytaka po wcisnieciu przycisk_3
{
  if (stan_led_3 == HIGH)
  {
  if (zwrot_chwytaka == 0)
    {
  zwrot_chwytaka = 1;
  servo_6.write(0);
  Serial.println("Chwytak Otwarty");
  stan_led_3 = !stan_led_3;
  digitalWrite(led_3, stan_led_3);
    }
  else if (zwrot_chwytaka == 1)
    {
  zwrot_chwytaka = 0;
  servo_6.write(150);
  Serial.println("Chwytak zamknięty");
  stan_led_3 = !stan_led_3;
  digitalWrite(led_3, stan_led_3); 
    }
  }
}
  
void ster_pot() // Sterowanie reczne serwami przy pomocy potencjometrów
{
 val0 = analogRead(pot0);
 val0 = map(val0, 1023, 0, 2000, 9984);
 maestro.setTarget(0, val0);
 
 val1 = analogRead(pot1);
 val1 = map(val1, 1023, 0, 2000, 9984);
 maestro.setTarget(1, val1);

 val2 = analogRead(pot2);
 val2 = map(val2, 0, 1023, 4560, 9984);
 maestro.setTarget(2, val2);

 val3 = analogRead(pot3);
 val3 = map(val3, 0, 1023, 2000, 9984);
 maestro.setTarget(3, val3);

 val4 = analogRead(pot4);
 val4 = map(val4, 1023, 0, 2432, 8832);
 maestro.setTarget(4, val4);

 val5 = analogRead(pot5);
 val5 = map(val5, 0, 1023, 3200, 8832);
 maestro.setTarget(5, val5);

}

void Nasluch_przycisk1() // Przelacza diode 1 w przypadku wcisniecia przycisku 1
{
  int odczyt_1 = digitalRead(przycisk_1);

  if (odczyt_1 != ost_stan_przycisku_1) {
    ost_stan_odbicia_1 = millis();
  }

  if ((millis() - ost_stan_odbicia_1) > opoznienie_odbicia) {
    
    if (odczyt_1 != stan_przycisku_1) {
      stan_przycisku_1 = odczyt_1;

      if (stan_przycisku_1 == LOW) {
        stan_led_1 = !stan_led_1;
      }
    }
  }

  digitalWrite(led_1, stan_led_1);
  
  ost_stan_przycisku_1 = odczyt_1;
}

void Nasluch_przycisk2() // Przelacza diode 2 w przypadku wcisniecia przycisku 2
{ 
  int odczyt_2 = digitalRead(przycisk_2);

  if (odczyt_2 != ost_stan_przycisku_2) 
  {   
    ost_stan_odbicia_2 = millis();
  }
  if ((millis() - ost_stan_odbicia_2) > opoznienie_odbicia) 
  {
    if (odczyt_2 != stan_przycisku_2) 
    {
      stan_przycisku_2 = odczyt_2;
      
      if (stan_przycisku_2 == LOW) 
      {
        stan_led_2 = !stan_led_2;
      }
    }
  }
  digitalWrite(led_2, stan_led_2);  
  ost_stan_przycisku_2 = odczyt_2;
}

void Nasluch_przycisk3() // Przelacza diode 3 w przypadku wcisniecia przycisku 3
{
  int odczyt_3 = digitalRead(przycisk_3);
  
  if (odczyt_3 != ost_stan_przycisku_3) 
  {
    ost_stan_odbicia_3 = millis();
  }
  if ((millis() - ost_stan_odbicia_3) > opoznienie_odbicia) 
  {
    if (odczyt_3 != stan_przycisku_3) 
    {
      stan_przycisku_3 = odczyt_3;
      
      if (stan_przycisku_3 == LOW) 
      {
        stan_led_3 = !stan_led_3;
      }
    }
  }
  digitalWrite(led_3, stan_led_3); 
  ost_stan_przycisku_3 = odczyt_3;
}


void jazda_do_poz_domowej() // Ustawia robota w pozycji domowej
{
 maestro.setTarget(0, 5204);
 delay(700);
 maestro.setTarget(1, 9320);
 delay(700);
 maestro.setTarget(2, 4560);
 delay(700);
 maestro.setTarget(3, 5824);
 delay(700);
 maestro.setTarget(3, 5824);
 delay(700);
 maestro.setTarget(4, 5824);
 delay(700);
 maestro.setTarget(5, 6292);
 delay(700);
}

 void przys_ogolne() // Nastawia przyspieszenie robota do normalnej pracy
 {
 maestro.setAcceleration(0, 12);
 maestro.setAcceleration(1, 12);
 maestro.setAcceleration(2, 12);
 maestro.setAcceleration(3, 12);
 maestro.setAcceleration(4, 30);
 maestro.setAcceleration(5, 30);
 }

 void predkosc_ogolna()
 {
  maestro.setSpeed(0, 0);
  maestro.setSpeed(1, 0);
  maestro.setSpeed(2, 0);
  maestro.setSpeed(3, 0);
  maestro.setSpeed(4, 0);
  maestro.setSpeed(5, 0);
 }
