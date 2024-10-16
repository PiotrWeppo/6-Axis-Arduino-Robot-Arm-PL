# 6-osiowe Ramię Robotyczne Arduino [PL]

---

<p align="center">
      <img src="./images/robot.gif" alt="View of the Robot" />
      <br>Projekt był częścią pracy końcowej inżynierskiej.
</p>

<div align="center">

![GitHub](https://img.shields.io/github/license/PiotrWeppo/Arduino_6_Axis_Robotic_Arm)

[Kod programu](#kod-programu) •
[Schemat pracy programu](#schemat-pracy-programu) •
[Schemat połączeń elektrycznych](#schemat-połączeń-elektrycznych) •
[Licencja](#licencja)

</div>


## Kod programu

---

<div style="text-align: center;">
  <a href="./docs/arduino_code_explanation.md">Kod programu Arduino</a>
</div>

## Schemat pracy programu

---

 Na poniższym schemacie w sposób uproszczony przedstawiono działanie programu. 
Warto zwrócić uwagę na dwie kwestie; program nie posiada funkcji która go wyłącza, jedynym sposobem na zakończenie działania programu jest fizyczne użycia przełącznika zasilania lub wciśnięcie przycisku grzybkowego stopu awaryjnego. Drugim zagadnieniem jest występowanie na płytce mikrokontrolera przycisku RESET. Wciśnięcie go w dowolnym momencie pracy programu powoduje zatrzymanie wszystkich procesów i ponowną inicjalizację pracy robota jak przy włączeniu zasilania.

<div style="text-align: center;">
  <img src="./images/schemat blokowy pracy programu.png" alt="Schemat blokowy pracy programu" />
</div>

## Lista części

| Tytuł                                                        | Sklep         | Sztuk |
|--------------------------------------------------------------|---------------|-------|
| Serwo PowerHD HD-1501MG - standard                           | Botland       | 4     |
| Serwo TowerPro SG-92R - micro                                | Botland       | 1     |
| Sterownik serw Micro Maestro USB 6-kanałowy - Pololu 1350    | Botland       | 1     |
| Przedłużacz do serw 60cm                                     | Botland       | 3     |
| Przedłużacz do serw 30cm                                     | Botland       | 3     |
| Zasilacz montażowy POS-100-5-C - 5V/18A/90W                  | Botland       | 1     |
| Płytka drukowana uniwersalna dwustronna 20x80mm              | Botland       | 1     |
| Potencjometr obrotowy 10kΩ liniowy 1/8W                      | Botland       | 6     |
| Tact Switch 12x12mm z nasadką - grzybek czarny - 5szt        | Botland       | 2     |
| Przełącznik bezpieczeństwa ON-OFF - grzybek - 35mm           | Botland       | 1     |
| Przełącznik dźwigniowy ON-OFF KN3(C)-201 250V/6A             | Botland       | 1     |
| Mosiężny gwint - insert do druku 3D - różne rozmiary - zestaw 50 sztuk | Botland       | 1     |
| Przewód zasilający z wtyczką JST - BEC                       | Botland       | 2     |
| Przewody połączeniowe żeńsko-męskie justPi 10cm - 40szt.     | Botland       | 1     |
| Łożysko z flanszą MF 104 ZZ - 4x10 / 11,2x4 mm MF104 ZZ      | Poltech S.C.  | 3     |
| Łożysko z flanszą F 693 ZZ - 3x8 / 9,5x4 mm F693 ZZ          | Poltech S.C.  | 4     |
| Dystans mosiężny M3 W/W 10m (10szt)                          | Allegro       | 1     |
| Dystans mosiężny M3 W/W 40mm /2172                           | Allegro       | 8     |
| GALKA POTENCJOMETRA 16mm CZARNO-CZER GC16 1SZT               | Allegro       | 6     |
| Przewód silikonowy 16 AWG czarny 1mb                         | Allegro       | 1     |
| Przewód zasilający kabel czarny z wtyczką 1,5m 3x1           | Allegro       | 1     |
| Serwomechanizm TowerPro MG-90S metalowe zębatki RC           | Allegro       | 2     |
| UCF205 Łożysko samonastawne w oprawie UCF 205                | Allegro       | 1     |
| Insert gwintowany Bossard M3                                 | Allegro       | 4     |
| Orczyk do Serwo MG995 - aluminium 25T – okrągły              | Allegro       | 4     |
| Rura PVC fi25                                                | Leroy Merlin  | 1     |
| Cięcie laserowe + gięcie elementów z blachy                  |               | 1     |
| Komplet śrub, podkładek i nakrętek                           |               | 1     |
| Koszt zużytego filamentu do druku 3D                         |               | 1     |

## Schemat połączeń elektrycznych

---

Na poniższych zdjęciach przedstawiono połączenie podzespołów elektrycznych robota.

<p align="center">
  <img src="./images/schemat elektryczny.png" alt="Schemat połączeń elektrycznych" />
</p>

Osoba planująca konstruowanie tego manipulatora, powinna mieć na uwadze, że płytka Cytron Maker Uno (w przeciwieństwie do Arduino Uno), nie zawiera gniazda zasilacza, dlatego jedyną możliwością zasilenia płytki jest użycie przewodu Micro USB podłączonego do komputera, albo źródła prądu z kompatybilnym gniazdem, np. powerbanku. Opcjonalnie, co zostało zrobione w tym projekcie, taki kabel można rozciąć od strony wtyczki USB A. W środku powinny znajdować się cztery przewody, spośród których czerwony i czarny odpowiadają za zasilanie. Te przewody można podłączyć do zasilacza, a resztę uciąć. W ten sposób płytka będzie mogła korzystać ze wspólnego zasilania z napędami.
Warto zwrócić uwagę, że układ zawiera dwa systemy zabezpieczeń. Pierwszym jest przełącznik dźwigniowy, przez którego przeprowadzone są wszystkie przewody układu i który przy podłączania kabla od zasilania powinien być ustawiony w pozycji wyłączonej. Dodatkowym zabezpieczeniem jest przycisk grzybkowy awaryjny, który jest normalnie zamknięty. W przypadku jakiejkolwiek niebezpiecznej sytuacji, należy od razu wcisnąć przycisk, który odcina prąd od zasilacza.



## Licencja

---

Copyright © 2024 Piotr Weppo

This project is [MIT](https://choosealicense.com/licenses/mit/) licenced.
