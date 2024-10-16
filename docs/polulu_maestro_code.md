
Poniżej przedstawiony
jest przykładowy kod skryptu użyty w tym programie, nie widać go w kodzie
programu Arduino, a jego występowanie definiują jedynie komendy
maestro.restartScript() oraz maestro.stopScript(). W pierwszym bloku wyznaczone
są przyśpieszenia wszystkich osi. Skrót sub (subsequence), oznacza podsekwencje
skryptu. Podsekwencje wykonywane są w kolejności chronologicznej, chyba, że
podsekwencja odwołuje się do innej. Kolejny blok programu zawiera zapisy
wszystkich koordynatów poszczególnych kanałów w każdym kroku. W tym bloku,
pierwsza liczba w każdym bloku, wyznacza czas opóźnienia po wykonaniu tegoż kroku.
Jeżeli w danym kroku część kanałów nie zarejestrowała zmiany w pozycji, dany
wiersz jest pomniejszony o nieużywane serwa. Na przykładzie drugiego wiersza
widać, że w danym kroku wystąpiła zmiana na kanale 0 oraz 1, po której program
oczekuje 1ms na wykonanie kolejnego kroku. Każdy krok jest kolejną podsekwencją
programu i charakteryzuje go tytuł opisany zieloną czcionką. Wystąpienie znaku
„#” otwiera komentarz w danym wierszu i przedstawiony jest kolorem niebieskim.

Warto jeszcze zwrócić
uwagę, na wielokrotnie pojawiającą się linijkę kodu moving_wait. Odwołuje się
ona do podsekwencji o tym samym tytule wywołująca funkcję get_moving_state,
która zwraca 0, gdy wszystkie serwomechanizmy dojadą do swoich pozycji, tym
samym wstrzymując program przed przedwczesnym przejściem do kolejnego kroku.

```
# Sequence 0
sub inz
begin
15 0 ACCELERATION
15 1 ACCELERATION
15 2 ACCELERATION
15 3 ACCELERATION
15 4 ACCELERATION
30 5 ACCELERATION


  1 5401 8527 5832 5246 5694 5769 frame_0..5 # Frame 0
  1 7032 7049 frame_0_1 # Frame 1
  300 5052 8681 frame_1_2 # Frame 2
  1500 2880 9454 2644 8459 3364 frame_1..5 # Frame 3
  1500 5207 9536 5129 8490 6000 frame_0_2..5 # Frame 4
  1500 3149 9617 8430 8428 8257 frame_0_2..5 # Frame 5
  300 4276 9401 5538 6000 frame_1_3..5 # Frame 6
  300 5479 5755 7908 5090 frame_0..3 # Frame 7
  300 5401 8527 5832 5246 5694 5769 frame_0..5 # Frame 8
repeat

sub frame_0..5
  5 servo
  4 servo
  3 servo
  2 servo
  1 servo
  0 servo
moving_wait
  delay
  return

sub frame_0_1
  1 servo
  0 servo
 moving_wait
  delay
  return

sub frame_1_2
  2 servo
  1 servo
  moving_wait
  delay
  return

sub frame_1..5
  5 servo
  4 servo
  3 servo
  2 servo
  1 servo
moving_wait
  delay
  return

sub frame_0_2..5
50 0 SPEED
50 1 SPEED
50 2 SPEED
50 3 SPEED
60 4 SPEED
60 5 SPEED
  5 servo
  4 servo
  3 servo
  2 servo
  0 servo
moving_wait
  delay
  return

sub frame_1_3..5
0 0 SPEED
0 1 SPEED
0 2 SPEED
0 3 SPEED
0 4 SPEED
0 5 SPEED
  5 servo
  4 servo
  3 servo
  1 servo
moving_wait
  delay
  return

sub frame_0..3
  3 servo
  2 servo
  1 servo
  0 servo
# moving_wait
  delay
  return

	sub moving_wait
  begin
    get_moving_state
  while
    # wait until it is no longer moving
  repeat
  return
```
