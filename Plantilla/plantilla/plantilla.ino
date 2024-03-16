#include <EEPROM.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
Servo myservo;
//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Wire.h>              // libreria para bus I2C
#include <Adafruit_GFX.h>      // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>  // libreria para controlador SSD1306
#define ANCHO 128              // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64                // reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 34          // necesario por la libreria pero no usado
#define pot A0
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto

#include "SoftwareSerial.h"
SoftwareSerial mySerial(10, 11);  // RX, TX
//clase para detectar la pulsacion de botones
#define SPEAKERPIN 3
#define NUMWORDS 10
//define notes for buzzer
#define LOWNOTE 100
#define ALOW 440
#define CLOW 261
#define ELOW 329
#define FLOW 349
#define CHIGH 523
#define EHIGH 659
#define GHIGH 784
#define FSHIGH 740
#define AHIGH 880
#define btnRight 0
#define btnUp 1
#define btnDown 2
#define btnLeft 3
#define btnSelect 4
#define btnNone 5
void (*resetFunc)(void) = 0;  //declare reset function @ address 0
const char letterVal[] = "abcdefghijklmnopqrstuvwxyz";
char guessLetter;
char guessLast;
char guessed[25];
char* secretWord;
int guessedCount = 1;
int wordSize;
int gotOne = 0;
int alreadyGuessed = 0;
int showAsterisk = 0;
int buttonState;
int hangman = 0;
int totalRight = 0;
int sel = 0;
int prevKey = btnNone;
unsigned long lastDebounceTime = 0;
String guessWord = String(10);
// hangman graphic characters
byte topleft[] = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 };
byte topright[] = { 0x1C, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte bottomleft[] = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x1F, 0x1F };
byte bottomright[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte head[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x00, 0x00, 0x00 };
byte topbody[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x04, 0x04 };
byte bottombody[] = { 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte rightarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x05, 0x06, 0x04 };
byte leftarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x15, 0x0E, 0x04 };
byte rightleg[] = { 0x04, 0x04, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00 };
byte leftleg[] = { 0x04, 0x04, 0x0A, 0x0A, 0x11, 0x00, 0x00, 0x00 };
byte leftarrow[] = { 0x10, 0x18, 0x1C, 0x1E, 0x1E, 0x1C, 0x18, 0x10 };
byte rightarrow[] = { 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01 };


int numgrupo, nivel = 0;
bool esperar = false, ahorca = false;


class Button {
  const byte buttonPin;
  static constexpr byte debounceDelay = 30;
  const bool active;
  bool lastButtonState = HIGH;
  byte lastDebounceTime = 0;

public:

  Button(byte attachTo, bool active = LOW)
    : buttonPin(attachTo), active(active) {}
  void begin() {
    if (active == LOW)
      pinMode(buttonPin, INPUT_PULLUP);
    else
      pinMode(buttonPin, INPUT);
  }

  bool wasPressed() {

    bool buttonState = LOW;                                      // the current reading from the input pin
    byte reading = LOW;                                          // "translated" state of button LOW = released, HIGH = pressed, despite the electrical state of the input pint
    if (digitalRead(buttonPin) == active) reading = HIGH;        // if we are using INPUT_PULLUP we are checking invers to LOW Pin
    if (((millis() & 0xFF) - lastDebounceTime) > debounceDelay)  // If the switch changed, AFTER any pressing or noise
    {
      if (reading != lastButtonState && lastButtonState == LOW)  // If there was a change and and last state was LOW (= released)
      {
        buttonState = HIGH;
      }
      lastDebounceTime = millis() & 0xFF;
      lastButtonState = reading;
    }
    return buttonState;
  }
};

Button buttonv{ A1 };
//----------------------------
/* Constants - define pin numbers for LEDs,
   buttons and speaker, and also the game tones: */
const byte ledPins[] = { 2, 3, 4, 5,6 };
const byte buttonPins[] = {30, 31, 32, 33, 34 };

#define MAX_GAME_LENGTH 100


/* Global variables - store the game state */
byte gameSequence[MAX_GAME_LENGTH] = { 0 };
byte gameIndex = 0;

/**
   Set up the Arduino board and initialize Serial communication
*/
//____________________________________teclado_______________________

const byte filas = 4;
const byte columnas = 4;
byte pinesFilas[] = { 39, 41, 43, 45 };
byte pinesColumnas[] = { 47, 49, 51, 53 };
char teclas[4][4] = { { '1', '2', '3', 'A' },
                      { '4', '5', '6', 'B' },
                      { '7', '8', '9', 'C' },
                      { '*', '0', '#', 'D' } };
Keypad teclado1 = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas);
//_________________________________________________________MP3__________________________
void sendCommand(uint8_t command, uint8_t feedback = 0, uint16_t argument = 0) {
  uint8_t highByte = argument >> 8;
  uint8_t lowByte = argument & 0xFF;

  uint8_t cmdBuf[] = { 0x7E, 0xFF, 0x06, command, feedback, highByte, lowByte, 0xEF };

  for (int i = 0; i < sizeof(cmdBuf); i++) {
    mySerial.write(cmdBuf[i]);
  }
}
//_____________________________________________________________________________________________
const int segmentPins[7] = { 22, 23, 24, 25, 26, 27, 28 };
const int digitPins[4] = { 46, 48, 50, 52 };
const byte numbers[10][7] = {
  { 0, 0, 0, 0, 0, 0, 1 },  // 0
  { 1, 0, 0, 1, 1, 1, 1 },  // 1
  { 0, 0, 1, 0, 0, 1, 0 },  // 2
  { 0, 0, 0, 0, 1, 1, 0 },  // 3
  { 1, 0, 0, 1, 1, 0, 0 },  // 4
  { 0, 1, 0, 0, 1, 0, 0 },  // 5
  { 0, 1, 0, 0, 0, 0, 0 },  // 6
  { 0, 0, 0, 1, 1, 1, 1 },  // 7
  { 0, 0, 0, 0, 0, 0, 0 },  // 8
  { 0, 0, 0, 0, 1, 0, 0 }   // 9
};
int valor[4] = { 0, 0, 0, 0 };
void displayNumber(int displayIndex, int number) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], HIGH);
  }

  digitalWrite(digitPins[displayIndex], HIGH);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], numbers[number][i]);
  }
}
unsigned long cronometroUpdateTime = 0;
unsigned long displayUpdateTime = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 30000;
const unsigned long CRONOMETRO_UPDATE_INTERVAL = 1000;  // Actualiza el cronómetro cada 1000 ms (1 segundo)
unsigned long currentTime;
bool iniciar, enr1 = false, enr2 = false, enr3 = false, enr4 = false;
String leevalor;
int h = 0, m = 10, s = 0, dc = 0, countlogic=0;
;
int respuestas1[5] = { {{resp2}} };
int respuestas2[5] = { {{resp3}} };
int respuestas3[5] = { {{resp4}} };
int respuestas4[5] = { {{resp5}} };
const char* words[] = { {{resp1}} };

int stw = A2, stw2 = A3, stw3 = A4;
int intentos = 0, vidas = 3, victorias = 0, respuesta1, respuesta2, respuesta3;
unsigned long startTime, elapsedTime1;
bool jugando = false;

void setup() {
  // Configuración inicial
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial1.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
  }
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
 for (byte i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
 
  // The following line primes the random number generator.
  // It assumes pin A0 is floating (disconnected):
  randomSeed(analogRead(A0));
  pinMode(stw, INPUT);
  pinMode(stw2, INPUT);
  pinMode(stw3, INPUT);
  pinMode(pot, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("Bienvenido");

  //_________________________________
  // inicializa bus I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // inicializa pantalla con direccion 0x3C
  //_____________________MODULO MP3___________________________________________________
  sendCommand(0x3F, 0, 0);  // Inicializa el módulo
  delay(500);
  sendCommand(0x06, 0, 10);  // volumen de 0 a 30
  //sendCommand(0x0E);         // Pausa todos los sonidos en caso que se reinicie
  sendCommand(0x03, 0, 1);
  delay(100);
  lcd.clear();
  lcd.print("Numero de grupo");
  lcd.setCursor(0, 1);
  lcd.print("a jugar:");
  myservo.attach(A5);
  esperar = true;
  myservo.write(100);
  while (esperar == true) {
    encender_leds();
    char tecla_presionada = teclado1.getKey();

    if (tecla_presionada == '1' || tecla_presionada == '2' || tecla_presionada == '3' || tecla_presionada == '4' || tecla_presionada == '5') {
      lcd.setCursor(10, 1);
      numgrupo = tecla_presionada - '0';
      lcd.print(tecla_presionada);
    }
    if (tecla_presionada == '#') {
      if (numgrupo != 0) {
        esperar = false;
        jugando = true;
      }
    }
  }
  vidas = 4;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("  INICIANDO");
  sendCommand(0x03, 0, 1);
  sendCommand(0x03, 0, 1);
  delay(13000);

  startTime = millis();
  cronometroUpdateTime = millis() + CRONOMETRO_UPDATE_INTERVAL;
  displayUpdateTime = millis() + DISPLAY_UPDATE_INTERVAL;
}

void loop() {
  {{pin1}} 
  {{pin2}} 
  {{pin3}} 
  {{pin4}} 
  {{pin5}} 
  {{pin6}}
  
  victoria();
}
void encender_leds() {

  displayNumber(0, valor[0]);
  displayNumber(1, valor[1]);
  displayNumber(2, valor[2]);
  displayNumber(3, valor[3]);
}

void victoria() {
  victorias++;
  sendCommand(0x03, 0, 6);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FELICIDADES");
  delay(9000);
  jugando = false;
  delay(2000);
  asm volatile("jmp 0x00");
}
void reloj() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  // Variables para el temporizador
  unsigned long timerStartTime = millis();
  unsigned long timerDuration = {{tiempo}} * 60 * 1000;  // Duración del temporizador en milisegundos (5 minutos)
  unsigned long remainingTime = timerDuration - (currentTime - timerStartTime);

  // Si el temporizador ha terminado, puedes reiniciarlo automáticamente
  if (remainingTime <= 0) {
    timerStartTime = millis();      // Reinicia el temporizador
    remainingTime = timerDuration;  // Reinicia el tiempo restante
  }

  sendCommand(0x03, 0, 3);
  s = (elapsedTime / 1000) % 60;
  m = (elapsedTime / 60000) % 60;
  h = elapsedTime / 3600000;

  // Si el temporizador está activo, muestra el tiempo restante del temporizador
  if (remainingTime > 0) {
    h = remainingTime / 3600000;
    m = (remainingTime / 60000) % 60;
    s = (remainingTime / 1000) % 60;
  }

  oled.clearDisplay();       // Limpia la pantalla
  oled.setTextColor(WHITE);  // Establece el color del texto en blanco (pantalla monocromo)

  // Escribe en pantalla el tiempo transcurrido o el tiempo restante del temporizador
  oled.setCursor(10, 30);  // Ubica el cursor en coordenadas 10,30
  oled.setTextSize(2);     // Establece el tamaño del texto en 2

  if (h < 10) {
    oled.print("0");
  }
  oled.print(h);
  oled.print(":");

  if (m < 10) {
    oled.print("0");
  }
  oled.print(m);
  oled.print(":");

  if (s < 10) {
    oled.print("0");
  }
  oled.print(s);
  oled.display();
}
void verifica_vidas() {
  sendCommand(0x03, 0, 2);
  lcd.print("   RESPUESTA");
  lcd.setCursor(0, 1);
  lcd.print("   ICORRECTA");
  delay(1000);
  lcd.clear();
  if (vidas > 0) {
    vidas--;
  }

  if (vidas == 0) {
    sendCommand(0x03, 0, 4);
    delay(6000);
    asm volatile("jmp 0x00");
  }
  
}

int octalToDecimal(int octal) {
  int decimal = 0;
  int base = 1;

  while (octal > 0) {
    int lastDigit = octal % 10;
    decimal += lastDigit * base;
    base *= 8;
    octal /= 10;
  }

  return decimal;
}
//ahorcado game
void draw_hangman(int var) {
  switch (var) {
    case 1:
      lcd.createChar(1, head);  // head
      break;
    case 2:
      lcd.createChar(1, topbody);  // body
      lcd.createChar(3, bottombody);
      break;
    case 3:
      lcd.createChar(1, rightarm);  // right arm
      break;
    case 4:
      lcd.createChar(1, leftarm);  // left arm
      break;
    case 5:
      lcd.createChar(3, rightleg);  // right leg
      break;
    case 6:
      lcd.createChar(3, leftleg);  // left leg
      break;
    case 7:
      gameOver(0);
    default:
      break;
  }
}
void gameOver(int whatToDo) {
  // decide whether win, lose or restart game
  switch (whatToDo) {
    case 0:  // GAME OVER
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("GAME");
      lcd.setCursor(6, 1);
      lcd.print("OVER");
      //buzzer sound
      buzz(ELOW, 500);   // GAME OVER!
      buzz(CLOW, 1000);  // sound buzzer
      vidas = 0;
      verifica_vidas();
      break;
    case 1:  // WINNER
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("EXCELENTE");
      lcd.setCursor(4, 1);
      lcd.print("MAESTRO!");
      sendCommand(0x03, 0, 5);
      ahorca = false;
      // buzzer sound
      buzz(ALOW, 150);
      buzz(CHIGH, 150);
      buzz(EHIGH, 150);
      buzz(AHIGH, 150);
      delay(150);
      buzz(GHIGH, 150);
      buzz(AHIGH, 500);
      jugando = false;
      ahorca = false;
  }
  delay(2000);
}
void buzz(int frequencyInHertz, long timeInMilliseconds) {
  Serial.println(frequencyInHertz);
  long delayAmount = (long)(1000000 / frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds * 1000) / (delayAmount * 2));
  for (int x = 0; x < loopTime; x++) {
    digitalWrite(SPEAKERPIN, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(SPEAKERPIN, LOW);
    delayMicroseconds(delayAmount);
  }
  delay(20);
}
int getKey() {
  char b = teclado1.getKey();
  if (!b) return btnNone;
  delay(8);
  if (b == '6') return btnRight;
  if (b == '2') return btnUp;
  if (b == '8') return btnDown;
  if (b == '4') return btnLeft;
  if (b == '#') return btnSelect;
}
int inkeys() {
  int k = getKey();
  while (k == btnNone) {
    k = getKey();
    encender_leds();
    unsigned long currentTime = millis();
    if (currentTime >= cronometroUpdateTime) {
      reloj();
      cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
    }
    delay(10);
  }
  delay(200);  //debounce
  return k;
}


void newWord() {
  //pick a random word from the list
  int pick = random(NUMWORDS);
  const char* pickWord = words[numgrupo - 1];
  guessWord = pickWord;
  //secretWord = guessWord.getChars();
  wordSize = guessWord.length();
  Serial.println(guessWord);  // print the word to serial for cheaters like me ;)
}
void draw_board() {
  // define the custom characters
  lcd.createChar(0, topleft);
  lcd.createChar(1, topright);
  lcd.createChar(2, bottomleft);
  lcd.createChar(3, bottomright);
  lcd.createChar(4, leftarrow);
  lcd.createChar(5, rightarrow);
  // draw blank hangman table
  lcd.clear();
  lcd.home();
  lcd.write(byte(0));
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.write(3);
  // print underlines
  lcd.setCursor(3, 1);
  for (int x = 0; x < wordSize; x++) {
    lcd.print("_");
  }
}

/**
   Lights the given LED and plays a suitable tone
*/
void lightLedAndPlayTone(byte ledIndex) {
  digitalWrite(ledPins[ledIndex], HIGH);
  sendCommand(0x03, 0, 9);
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
}

/**
   Plays the current sequence of notes that the user has to repeat
*/
void playSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

/**
    Waits until the user pressed one of the buttons,
    and returns the index of that button
*/
byte readButtons() {
  while (true) {
    for (byte i = 0; i < 5; i++) {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW) {
        return i;
      }
    }
    delay(1);
  }
}

/**
  Play the game over sequence, and report the game score
*/
void gameOver2() {
  countlogic++;
  if(countlogic==4){
    vidas=0;
    verifica_vidas();
  }
  Serial.print("Game over! your score: ");
  Serial.println(gameIndex - 1);
  gameIndex = 0;
  delay(200);
  sendCommand(0x03, 0, 7);
  delay(300);

  delay(500);
}

/**
   Get the user's input and compare it with the expected sequence.
*/
bool checkUserSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
  }

  return true;
}

/**
   Plays a hooray sound whenever the user finishes a level
*/
void playLevelUpSound() {
  sendCommand(0x03, 0, 9);
}

/**
   The main game loop
*/