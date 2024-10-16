## Kod programu robota

---

<p align="center">
      <img src="../images/robot.png" alt="Widok robota" />
      <br>Cały kod programu: <a href="../6_axis_robot_arm.ino">6_axis_robot_arm.ino</a>
      <br>
</p>

<hr>

Poniżej zamieszczono kod obsługi robota napisany w języku Arduino w programie Arduino IDE wraz z wyjaśnieniem działania. Na samym początku załączane są biblioteki używane  w tym projekcie. Pierwsza z nich jest zewnętrzną biblioteką do obsługi serw. W projekcie użyto kontrolera do serw o sześciu gniazdach Pololu 1350, jednakże wszystkich serw jest siedem, dlatego ostatni podpięty został bezpośrednio do mikrokontrolera i przez niego jest obsługiwany. Arduino posiada własną bibliotekę Servo.h, którą można by w normalnych okolicznościach użyć, jednak przy próbie użycia jej okazało się, że ostatnie serwo drży bezustannie i potrafi się zachowywać niekontrolowanie. Mikrokontroler Arduino korzysta z mechanizmu „przerwań”, czyli przerywa główną oś programu, aby następnie wywołać priorytetowe polecenie oraz wrócić do normalnego trybu. Przerwania pomagają działać takim funkcjom jak serial.read(), delay(), millis(), które zostały użyte w tym programie. Z drugiej strony serwa mają bardzo konkretne wymagania jeżeli chodzi o nastawę czasu, dlatego biblioteka Servo.h również korzysta z mechanizmu przerwań. Błąd, bądź różnica w nadawanym sygnale do serwa na poziomie um (mikrosekund), może powodować dziwne zachowania w pracy napędu. Z kolei przerwania do nadania polecenia serwu mogą przyczynić się do gubienia ticków przez funkcję millis(), która działa jako wewnętrzny stoper. Rozwiązań tego problemu może być kilka, zaczynając od użycia sterownika do serw z większą ilością gniazd, przez użycie mikrokontrolera, który lepiej sobie radzi z obsługą wielu zdarzeń na raz, lub użycie serwomechanizmów z lepszymi sensorami. Dla tego projektu najprostszym i najszybszym rozwiązaniem okazało się użycie innej biblioteki, która wyeliminowała powstały problem.
Kolejne cztery wiersze kodu odpowiadają za obsługę sterownika serw Maestro Pololu oraz komunikację z mikrokontrolerem. Piny 10 i 11 zdefiniowane są do połączenia UART, który pozwala na asynchroniczne wysyłanie i odbieranie danych między sterownikiem a mikrokontrolerem.
Warto jeszcze zwrócić uwagę na gniazda wejść i wyjść użyte w kodzie. Program korzysta z sześciu potencjometrów podpiętych pod wejścia analogowe, trzech diod LED, czterech przycisków do obsługi, oraz wbudowanego brzęczka do sygnalizacji dźwiękowych.

```arduino
#include <Adafruit_TiCoServo.h>
#include <PololuMaestro.h>
#include <SoftwareSerial.h>
SoftwareSerial maestroSerial(10, 11);
MicroMaestro maestro(maestroSerial);
Adafruit_TiCoServo servo_6;
int pot0 = A0, pot1 = A1, pot2 = A2, pot3 = A3, pot4 = A4, pot5 = A5;
int led_1 = 12, led_2 = 13, led_3 = 3;
int przycisk_1 = 4, przycisk_2 = 5, przycisk_3 = 6, przycisk_4 = 7,
przycisk_5 = 2;
int buzzer = 8;
int val0, val1, val2, val3, val4, val5;
int stan_led_1 = LOW, stan_led_2 = LOW, stan_led_3 = LOW;
int stan_przycisku_1, stan_przycisku_2, stan_przycisku_3;
int ost_stan_przycisku_1 = LOW, ost_stan_przycisku_2 = LOW,
ost_stan_przycisku_3 = LOW;
unsigned long ost_stan_odbicia_1 = 0, ost_stan_odbicia_2 = 0,
ost_stan_odbicia_3 = 0;
unsigned long opoznienie_odbicia = 50;
int oczek;
int staw_0[15], staw_1[15], staw_2[15], staw_3[15], staw_4[15], staw_5[15],
chwytak[15];
int zwrot_chwytaka = 0;
int krok = 0;
int top;
float dlugosc_wcisniecia_przycisku_4 = 0;
int opcja_czasowo_krotsza = 100;
int opcja_czasowo_dluzsza = 600;
```

Void setup() wyznacza funkcję, która wywoływana jest na początku programu tylko raz. Tutaj definiowane są wszystkie piny jako wejścia lub wyjścia, ustawiana jest prędkość przesyłu danych do komunikacji przez UART oraz do korzystania z monitora portu szeregowego. Na koniec wysyłana jest wiadomość dla użytkownika o gotowości robota do pracy.

```arduino
void setup(){
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

```

Void loop () jest główną osią programu, która nieprzerwanie jest powtarzana w pętli. Z tego miejsca przechodzi się do pomniejszych funkcji. Program główny został napisany z myślą o schludności kodu. Poszczególne funkcje i ich wywołania zostaną opisane w dalszej części.

```arduino
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

```

Funkcja nauka_i_powtarzanie_krokow() jest najdłuższą w programie i zasadniczo składa się z trzech części. Pierwsza odczytuje stan przycisku_4 i po wciśnięciu zaczyna zliczać jak długo przycisk był trzymany. Jeżeli czas trzymania przycisku przekroczył 600ms, aktywowana jest część odpowiedzialna za odtwarzanie nagranych kroków podczas ręcznej nauki trasy. Na początku wyświetlany jest komunikat o ilości kroków, która będzie odegrana w pętli, zaś brzęczek odtwarza trójtonowy sygnał dźwiękowy informujący o wybraniu tego trybu. Jeżeli wszystko jest dobrze, program odczyta nagrane koordynaty i odtworzy je w pętli z odpowiednio zwolnioną prędkością napędów. Odtwarzane kolejne koordynaty wysyłane do wszystkich serw wyświetlane są na ekranie. Jeżeli ilość kroków jest zbyt mała do odtworzenia pętli (czyt. 0 lub 1), lub z jakiegoś powodu byłaby większa niż zezwala na to zadana możliwa do zapisania ilość kroków, użytkownik informowany jest o błędzie programu przez tekst oraz dłuższy sygnał dźwiękowy. Przytrzymanie przez moment przycisku_5, odgrywa tą samą 3-tonową melodię co przy wchodzeniu do pętli while, tylko w odwrotnej kolejności, informując wraz z komunikatem tekstowym, o opuszczeniu programu i powrocie do osi głównej programu. Zapisana pętla programu przechowywana jest w pamięci programu, do czasu rozpoczęcia zapisywania nowych koordynatów, resetu programu, bądź odcięcia zasilania.

```arduino
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
```

Jeżeli użytkownik zdecyduje się przytrzymać przycisk krócej niż 600ms, wchodzi do trybu zapisywania aktualnej pozycji wszystkich osi oraz chwytaka. Robota należy ustawiać w kolejnych pozycjach przy pomocy potencjometrów. Po krótkim wciśnięciu każda zapisana pozycja sygnalizowana jest przez komunikat tekstowy i krótki dźwięk. Domyślnie zaprogramowano możliwość przechowywania do 15 pozycji, ale nic nie stoi na przeszkodzie w zwiększeniu tego limitu. Po przekroczeniu limitu zapisu użytkownik słyszy długi sygnał dźwiękowy, widzi komunikat o przekroczeniu pamięci i wszystkie dotychczasowe pozycje są kasowane pozwalając na zapisywanie od początku. Poleca się zwracanie uwagi na dotychczasową ilość wykorzystanych kroków, aby program się nie skasował, lub zwiększenie jej przy wstępnym definiowaniu programu.

```arduino
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
```

Void Poz_domowa jest funkcją, która wysyła robota do tzw. pozycji domowej po wciśnięciu przycisku_1. W tej pozycji ramię jest złożone w bezpiecznej pozycji do pracy wokół niego, odcięta jest też możliwość ręcznego sterowania. Poszczególne osie są przemieszczane po kolei, za co odpowiada funkcja jazda_do_poz_domowej(), która zostanie opisana w dalszej części. W trakcie zadania robot informuje o momencie rozpoczęcia ruchu oraz kiedy ostatnia z osi osiągnie swój cel. Jedyną możliwością powrotu do programu jest ponowne wciśnięcie przycisku_1 po skończeniu pozycjonowania robota, które jednocześnie gasi diodę led_1. Zalecane jest wywołanie tej funkcji przed odcięciem zasilania, ponieważ serwomechanizmy nie posiadają żadnych hamulców trzymających ramię po wyłączeniu programu. Pozycja domowa jest bezpieczna do wyłączenia oraz transportu robota.

```arduino
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
```

Jedną z możliwości sterowania robotem jest napisanie skryptu w wewnętrznym języku sterownika Pololu Maestro, a następnie zapisanie go w pamięci sterownika. Poniższa funkcja pozwala odtworzyć zapisany program przez port UART do mikrokontrolera. Po wciśnięciu przycisku_2, skrypt programu jest odtwarzany w pętli. Ponowne wciśnięcie przycisku wychodzi z pętli i powraca do głównej osi programu. Warto zwrócić uwagę, że w tej wersji sterownika jest tylko sześć kanałów do obsługi serwomechanizmów, dlatego w przypadku tego programu, nie ma możliwości zaprogramowania ruchu chwytaka w skrypcie.

```arduino
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
```

<div style="text-align: center;">
  Więcej o skrypcie maestro: <a href="./polulu_maestro_code.md">polulu maestro code</a>
</div>
<hr>

Funkcja void Chwytak() w przypadku wciśnięcia przycisku_3 przełącza pozycję chwytaka z otwartej na zamkniętą lub na odwrót. Użytkownik informowany jest o aktualnym stanie chwytaka przez komunikat oraz zapalanie lub zgaszenie diody led_3.

```arduino
void Chwytak() // Otwieranie lub zamykanie chwytaka po wciśnięciu przycisk_3
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
```

Funkcja void ster_pot() odpowiada za kontrolę sześciu serwomechanizmów przy pomocy potencjometrów. Polecenie map() sprzęża zakres sczytanego sygnału z potencjometru z zakresem ,w którym może się poruszać serwomechanizm. W ten sposób obrót potencjometru o konkretny % jego całego zakresu odpowiada takiemu samemu % obrotu wału serwomotoru. Polecenie maestro.setTarget() przypisuje wybranemu kanałowi zadaną wartość.

```arduino
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
```

Poniżej przedstawiono definicje trzech funkcji: Nasluch_przycisk1(), Nasluch_przycisk2() i Nasluch_przycisk3(), których zadaniem jest przełączanie diod led_1, led_2 i led_3 w przypadku wciśnięcia. Po jednym wciśnięciu dioda przełącza swój stan z niskiego na wysoki, przy drugim - z wysokiego na niski. Dodatkowo funkcje te zostały rozszerzone o niwelację zjawiska odbicia (debounce). Zjawisko to charakterystyczne jest dla fizycznych przycisków i przełączników, które podczas pojedynczego wciśnięcia mogą sporadycznie generować sygnał nawet kilkukrotnego załączenia i rozłączenia styku. Na przykładzie funkcji Nasłuch_przycisk1() można pokazać jak na poziomie programu zniwelować to zjawisko. Zmienna odczyt_1 nasłuchuje zmianę stanu przycisku_1. W przypadku wciśnięcia przycisku_1 generowana jest zmienna, która przypisuje sobie wartość wywołanego stopera. Jeżeli od poprzednio zmienionego stanu przycisku do aktualnego momentu czasu upłynie wymagana ilość czasu, stan diody led jest zmieniany. Wszystkie zmiany wewnątrz tego odcinka czasu nie są brane pod uwagę.

```arduino
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
```

Void jazda_do_poz_domowej() jest funkcją wywoływaną wewnątrz funkcji Poz_domowa() i jej celem jest przypisanie wszystkim osiom odpowiedniego koordynata przemieszczenia. Parametr pierwszy w nawiasie oznacza kanał, do którego się odnosi komenda maestro.setTarget(), drugi zaś oznacza wartość zadanego położenia (w mikrosekundach).

```arduino
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
```

Ostatnie funkcje odpowiadają za nadanie odpowiedniego przyspieszenia i prędkości danym kanałom. Parametr pierwszy w nawiasie oznacza kanał do którego się odnosi, drugi zaś wartość. Wartość “0” oznacza brak limitu przyspieszenia lub prędkości, “1” jest najwolniejszym możliwym stanem. Kolejne wyższe liczby korespondują z większymi wartościami prędkości i przyspieszeń.

```arduino
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
```
