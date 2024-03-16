
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
Servo myservo;
// Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Wire.h>             // libreria para bus I2C
#include <Adafruit_GFX.h>     // libreria para pantallas graficas
#include <Adafruit_SSD1306.h> // libreria para controlador SSD1306
#define ANCHO 128             // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64               // reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 34         // necesario por la libreria pero no usado
#define pot A0
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET); // crea objeto

#include "SoftwareSerial.h"
SoftwareSerial mySerial(10, 11); // RX, TX
// clase para detectar la pulsacion de botones
#define NUMWORDS 10
// define notes for buzzer
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
void (*resetFunc)(void) = 0; // declare reset function @ address 0
const char letterVal[] = "abcdefghijklmnopqrstuvwxyz";
char guessLetter;
char guessLast;
char guessed[25];
char *secretWord;
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
byte topleft[] = {0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
byte topright[] = {0x1C, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
byte bottomleft[] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x1F, 0x1F};
byte bottomright[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte head[] = {0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x00, 0x00, 0x00};
byte topbody[] = {0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x04, 0x04};
byte bottombody[] = {0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte rightarm[] = {0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x05, 0x06, 0x04};
byte leftarm[] = {0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x15, 0x0E, 0x04};
byte rightleg[] = {0x04, 0x04, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00};
byte leftleg[] = {0x04, 0x04, 0x0A, 0x0A, 0x11, 0x00, 0x00, 0x00};
byte leftarrow[] = {0x10, 0x18, 0x1C, 0x1E, 0x1E, 0x1C, 0x18, 0x10};
byte rightarrow[] = {0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01};

int numgrupo, nivel = 0;
bool esperar = false, ahorca = false;

class Button
{
  const byte buttonPin;
  static constexpr byte debounceDelay = 30;
  const bool active;
  bool lastButtonState = HIGH;
  byte lastDebounceTime = 0;

public:
  Button(byte attachTo, bool active = LOW)
      : buttonPin(attachTo), active(active) {}
  void begin()
  {
    if (active == LOW)
      pinMode(buttonPin, INPUT_PULLUP);
    else
      pinMode(buttonPin, INPUT);
  }

  bool wasPressed()
  {

    bool buttonState = LOW; // the current reading from the input pin
    byte reading = LOW;     // "translated" state of button LOW = released, HIGH = pressed, despite the electrical state of the input pint
    if (digitalRead(buttonPin) == active)
      reading = HIGH;                                           // if we are using INPUT_PULLUP we are checking invers to LOW Pin
    if (((millis() & 0xFF) - lastDebounceTime) > debounceDelay) // If the switch changed, AFTER any pressing or noise
    {
      if (reading != lastButtonState && lastButtonState == LOW) // If there was a change and and last state was LOW (= released)
      {
        buttonState = HIGH;
      }
      lastDebounceTime = millis() & 0xFF;
      lastButtonState = reading;
    }
    return buttonState;
  }
};

Button buttonv{A1};
//----------------------------
/* Constants - define pin numbers for LEDs,
   buttons and speaker, and also the game tones: */
const byte ledPins[] = {2, 3, 4, 5, 6};
const byte buttonPins[] = {30, 31, 32, 33, 34};

#define MAX_GAME_LENGTH 100

/* Global variables - store the game state */
byte gameSequence[MAX_GAME_LENGTH] = {0};
byte gameIndex = 0;

/**
   Set up the Arduino board and initialize Serial communication
*/
//____________________________________teclado_______________________

const byte filas = 4;
const byte columnas = 4;
byte pinesFilas[] = {39, 41, 43, 45};
byte pinesColumnas[] = {47, 49, 51, 53};
char teclas[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}};
Keypad teclado1 = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas);
//_________________________________________________________MP3__________________________
void sendCommand(uint8_t command, uint8_t feedback = 0, uint16_t argument = 0)
{
  uint8_t highByte = argument >> 8;
  uint8_t lowByte = argument & 0xFF;

  uint8_t cmdBuf[] = {0x7E, 0xFF, 0x06, command, feedback, highByte, lowByte, 0xEF};

  for (int i = 0; i < sizeof(cmdBuf); i++)
  {
    mySerial.write(cmdBuf[i]);
  }
}
//_____________________________________________________________________________________________
const int segmentPins[7] = {22, 23, 24, 25, 26, 27, 28};
const int digitPins[4] = {46, 50,48, 52};
const byte numbers[10][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0}, // 2
    {0, 0, 0, 0, 1, 1, 0}, // 3
    {1, 0, 0, 1, 1, 0, 0}, // 4
    {0, 1, 0, 0, 1, 0, 0}, // 5
    {0, 1, 0, 0, 0, 0, 0}, // 6
    {0, 0, 0, 1, 1, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0}, // 8
    {0, 0, 0, 0, 1, 0, 0}  // 9
};
int valor[4] = {0, 0, 0, 0};
void displayNumber(int displayIndex, int number)
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(digitPins[i], LOW);
  }
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(segmentPins[i], HIGH);
  }

  digitalWrite(digitPins[displayIndex], HIGH);
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(segmentPins[i], numbers[number][i]);
  }
}
unsigned long cronometroUpdateTime = 0;
const unsigned long CRONOMETRO_UPDATE_INTERVAL = 1000; // Actualiza el cronómetro cada 1000 ms (1 segundo)
bool iniciar, enr1 = false, enr2 = false, enr3 = false, enr4 = false;
String leevalor;
int h = 0, m = 0, s = 0, dc = 0, countlogic = 0, countlogic2 = 0;
int respuestas1[5] = { 49,9,0,13,1 };
int respuestas2[5] = { 0420,0006,0005,0572,0112 };
int respuestas3[5] = { B011,B101,B011,B011,B110 };
int respuestas4[5] = { 3142657,2315674,1267354,7152643,4127365 };
const char* words[] = { "retener","condenar","aceptar","excluir","eventual" };

int stw = A2, stw2 = A3, stw3 = A4;
int intentos = 0, vidas = 3, respuesta1, respuesta2, respuesta3, st = 0;
bool jugando = false;

void setup()
{
  m = 15;
  // Configuración inicial
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial1.begin(9600);

  for (int i = 0; i < 4; i++)
  {
    pinMode(digitPins[i], OUTPUT);
  }
  for (int i = 0; i < 7; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
  }
  for (byte i = 0; i < 5; i++)
  {
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
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C); // inicializa pantalla con direccion 0x3C
  //_____________________MODULO MP3___________________________________________________
  sendCommand(0x3F, 0, 0); // Inicializa el módulo
  delay(500);
  sendCommand(0x06, 0, 10); // volumen de 0 a 30
  // sendCommand(0x0E);         // Pausa todos los sonidos en caso que se reinicie
  sendCommand(0x03, 0, 1);
  delay(100);
  lcd.clear();
  lcd.print("Numero de grupo");
  lcd.setCursor(0, 1);
  lcd.print("a jugar:");
  myservo.attach(A5);
  esperar = true;
  myservo.write(100);
  while (esperar == true)
  {
    encender_leds();
    char tecla_presionada = teclado1.getKey();

    if (tecla_presionada == '1' || tecla_presionada == '2' || tecla_presionada == '3' || tecla_presionada == '4' || tecla_presionada == '5')
    {
      lcd.setCursor(10, 1);
      numgrupo = tecla_presionada - '0';
      lcd.print(tecla_presionada);
    }
    if (tecla_presionada == '#')
    {
      if (numgrupo != 0)
      {
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

  cronometroUpdateTime = millis() + CRONOMETRO_UPDATE_INTERVAL;
}

void loop()
{
  //reto 1 
  //reto 2 
  //reto 3 
  //reto 4 
  //reto 5 
  //reto 6
  victoria();
}
void encender_leds()
{

  displayNumber(0, valor[0]);
  displayNumber(1, valor[1]);
  displayNumber(2, valor[2]);
  displayNumber(3, valor[3]);
}

void victoria()
{
  sendCommand(0x03, 0, 6);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FELICIDADES");
  delay(9000);
  jugando = false;
  delay(2000);
  asm volatile("jmp 0x00");
}
void reloj()
{

  sendCommand(0x03, 0, 3);

  st = s + (m * 60) + (h * 60 * 60);
  st--;
  h = (st / 3600);
  m = (st / 60) % 60;
  s = st % 60;

  oled.clearDisplay();      // limpia pantalla
  oled.setTextColor(WHITE); // establece color al único disponible (pantalla monocromo)

  // escribe en pantalla el tiempo restante
  oled.setCursor(10, 30); // ubica cursor en coordenadas 10,30
  oled.setTextSize(2);    // establece tamaño de texto en 2

  if (h < 10)
  {
    oled.print("0");
  }
  oled.print(h);
  oled.print(":");

  if (m < 10)
  {
    oled.print("0");
  }
  oled.print(m);
  oled.print(":");

  if (s < 10)
  {
    oled.print("0");
  }
  oled.print(s);
  oled.display();
  if (st == 0)
  {
    vidas = 0;
    verifica_vidas();
  }
}
void verifica_vidas()
{
  sendCommand(0x03, 0, 2);
  lcd.print("   RESPUESTA");
  lcd.setCursor(0, 1);
  lcd.print("   ICORRECTA");
  delay(1000);
  lcd.clear();
  if (vidas > 0)
  {
    vidas--;
  }

  if (vidas == 0)
  {
    sendCommand(0x03, 0, 4);
    delay(6000);
    asm volatile("jmp 0x00");
  }
}

int octalToDecimal(int octal)
{
  int decimal = 0;
  int base = 1;

  while (octal > 0)
  {
    int lastDigit = octal % 10;
    decimal += lastDigit * base;
    base *= 8;
    octal /= 10;
  }

  return decimal;
}
// ahorcado game
void draw_hangman(int var)
{
  switch (var)
  {
  case 1:
    lcd.createChar(1, head); // head
    break;
  case 2:
    lcd.createChar(1, topbody); // body
    lcd.createChar(3, bottombody);
    break;
  case 3:
    lcd.createChar(1, rightarm); // right arm
    break;
  case 4:
    lcd.createChar(1, leftarm); // left arm
    break;
  case 5:
    lcd.createChar(3, rightleg); // right leg
    break;
  case 6:
    lcd.createChar(3, leftleg); // left leg
    break;
  case 7:
    gameOver(0);
  default:
    break;
  }
}
void gameOver(int whatToDo)
{
  // decide whether win, lose or restart game
  switch (whatToDo)
  {
  case 0: // GAME OVER
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("GAME");
    lcd.setCursor(6, 1);
    lcd.print("OVER");
    // buzzer sound
    vidas = 0;
    verifica_vidas();
    break;
  case 1: // WINNER
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("EXCELENTE");
    lcd.setCursor(4, 1);
    lcd.print("MAESTRO!");
    sendCommand(0x03, 0, 5);
    ahorca = false;
    // buzzer sound
    jugando = false;
  }
  delay(2000);
}
int getKey()
{
  char b = teclado1.getKey();
  if (!b)
    return btnNone;
  delay(8);
  if (b == '6')
    return btnRight;
  if (b == '2')
    return btnUp;
  if (b == '8')
    return btnDown;
  if (b == '4')
    return btnLeft;
  if (b == '#')
    return btnSelect;
}
int inkeys()
{
  int k = getKey();
  while (k == btnNone)
  {
    k = getKey();
    encender_leds();
    unsigned long currentTime = millis();
    if (currentTime >= cronometroUpdateTime)
    {
      reloj();
      cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
    }
    delay(10);
  }
  delay(200); // debounce
  return k;
}

void newWord()
{
  // pick a random word from the list
  int pick = random(NUMWORDS);
  const char *pickWord = words[numgrupo - 1];
  guessWord = pickWord;
  // secretWord = guessWord.getChars();
  wordSize = guessWord.length();
  Serial.println(guessWord); // print the word to serial for cheaters like me ;)
}
void draw_board()
{
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
  for (int x = 0; x < wordSize; x++)
  {
    lcd.print("_");
  }
}

/**
   Lights the given LED and plays a suitable tone
*/
void lightLedAndPlayTone(byte ledIndex)
{
  digitalWrite(ledPins[ledIndex], HIGH);
  sendCommand(0x03, 0, 9);
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
}

/**
   Plays the current sequence of notes that the user has to repeat
*/
void playSequence()
{
  for (int i = 0; i < gameIndex; i++)
  {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

/**
    Waits until the user pressed one of the buttons,
    and returns the index of that button
*/
byte readButtons()
{
  while (true)
  {
    for (byte i = 0; i < 5; i++)
    {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW)
      {
        return i;
      }
    }
    delay(1);
  }
}

/**
  Play the game over sequence, and report the game score
*/
void gameOver2()
{
  countlogic++;
  if (countlogic == 4)
  {
    vidas = 0;
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
bool checkUserSequence()
{
  for (int i = 0; i < gameIndex; i++)
  {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton)
    {
      return false;
    }
  }

  return true;
}

/**
   Plays a hooray sound whenever the user finishes a level
*/
void playLevelUpSound()
{
  sendCommand(0x03, 0, 9);
}

/**
   The main game loop
*/
void reto6(){
                    
  gameSequence[gameIndex] = random(0, 4);
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH) {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  playSequence();
  if (!checkUserSequence()) {
    countlogic2=0;
    gameOver2();
  }

  delay(300);

  if (gameIndex > 0) {
    playLevelUpSound();
    delay(300);
    countlogic2++;
    if(countlogic2==6){
      jugando=false;
    }
  }
}
void reto5(){
  
                    
                    int correct4 = respuestas4[numgrupo - 1];
                    enr4 = true;
                    leevalor = "";
                    int dato = 0;
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("patrones de palabras");
                    lcd.setCursor(0, 1);
                    lcd.print("Orden:");
                    while (enr4 == true) {
                      encender_leds();
                      unsigned long currentTime = millis();
                      if (currentTime >= cronometroUpdateTime) {
                        reloj();
                        cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
                      }
                      char tecla_presionada = teclado1.getKey();
                      if (tecla_presionada == '1' || tecla_presionada == '2' || tecla_presionada == '3' || tecla_presionada == '4' || tecla_presionada == '5' || tecla_presionada == '6' || tecla_presionada == '7' || tecla_presionada == '8' || tecla_presionada == '9' || tecla_presionada == '0') {
                        lcd.setCursor(6, 1);
                        leevalor += tecla_presionada;
                        dato = leevalor.toInt();
                        lcd.print(leevalor);
                        lcd.print("          ");
                      }
                      if (tecla_presionada == '#') {
                        if (dato != 0) {
                          if (dato == correct4) {
                  
                            sendCommand(0x03, 0, 5);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("   RESPUESTA");
                            lcd.setCursor(0, 1);
                            lcd.print("    CORRECTA");
                            delay(1000);
                            jugando=false;
                            enr4 = false;
                          } else {
                            verifica_vidas();
                            delay(1000);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("patrones de palabras");
                            lcd.setCursor(0, 1);
                            lcd.print("Orden:");
                            dato = 0;
                            leevalor = "";
                            
                          }
                        }
                      }
                    }
}
void reto4(){
  
                    enr3 = true;
                    
                    int count = 0;
                    int correct3 = respuestas3[numgrupo - 1];
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("    Resuelve");
                    lcd.setCursor(0, 1);
                    lcd.print("Sinonimo/antonimo");
                    bool estadoAnterior1 = digitalRead(stw);
                    bool estadoAnterior2 = digitalRead(stw2);
                    bool estadoAnterior3 = digitalRead(stw3);
                    while (enr3 == true) {
                       encender_leds();
                      unsigned long currentTime = millis();
                  
                      if (currentTime >= cronometroUpdateTime) {
                        reloj();
                        cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
                      }
                      bool estadoActual1 = digitalRead(stw);   // Lee el estado del primer switch
                      bool estadoActual2 = digitalRead(stw2);  // Lee el estado del segundo switch
                      bool estadoActual3 = digitalRead(stw3);  // Lee el estado del tercer switch
                   
                      // Comprobar si el estado actual es diferente al anterior
                      if (estadoActual1 != estadoAnterior1) {
                        estadoAnterior1 = estadoActual1;  // Actualizar el estado anterior
                        count++;
                      }
                  
                      if (estadoActual2 != estadoAnterior2) {
                        estadoAnterior2 = estadoActual2;  // Actualizar el estado anterior
                        count++;
                      }
                  
                      if (estadoActual3 != estadoAnterior3) {
                        estadoAnterior3 = estadoActual3;  // Actualizar el estado anterior
                        count++;
                      }
                       if (analogRead(A1) == 0) {
                        int resultado = (estadoActual1 << 2) | (estadoActual2 << 1) | estadoActual3;
                           
                        if (resultado == correct3) {
                          sendCommand(0x03, 0, 5);
                          lcd.clear();
                          lcd.setCursor(0, 0);
                          lcd.print("  RESPUESTA");
                          lcd.setCursor(0, 1);
                          lcd.print("  CORRECTA");
                          delay(1000);
                          jugando=false;
                          enr3 = false;
                        } else {
                          
                          verifica_vidas();
                          delay(1000);
                          lcd.clear();
                          lcd.setCursor(0, 0);
                          lcd.print("    Resuelve");
                          lcd.setCursor(0, 1);
                          lcd.print("Sinonimo/antonimo");
                          count = 0;
                        }
                      }
                    }
        
}
void reto3(){
  
                    enr2 = true;
                    dc=0;
                    int correct2 = respuestas2[numgrupo - 1];
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("    Resuelve");
                    lcd.setCursor(0, 1);
                    lcd.print("Problema matematico");
                    while (enr2 == true) {
                      encender_leds();
                      unsigned long currentTime = millis();
                  
                      if (currentTime >= cronometroUpdateTime) {
                        reloj();
                        cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
                      }
                      if (buttonv.wasPressed()) {
                        dc++;
                  
                        if (dc > 3) {
                          String concatenado = "";
                          for (int i = 0; i < 4; i++) {
                            concatenado += String(valor[i]);
                          }
                          int resultado = octalToDecimal(concatenado.toInt());
                          if (resultado == correct2) {
                            sendCommand(0x03, 0, 5);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("   RESPUESTA");
                            lcd.setCursor(0, 1);
                            lcd.print("    CORRECTA");
                            delay(1000);
                            jugando=false;
                            enr2 = false;
                          } else {
                            verifica_vidas();
                            delay(1000);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("    Resuelve");
                            lcd.setCursor(0, 1);
                            lcd.print("Problema matematico");
                            dc = 0;
                            
                          }
                        }
                      }
                  
                      valor[dc] = map(analogRead(pot), 0, 1000, 0, 9);
                    }
}
void reto2()
{

  int dato;
  int correct = 1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Patron numerico");
  lcd.setCursor(0, 1);
  lcd.print("RSTA:");
  enr1 = true;
  while (enr1 == true)
  {
    encender_leds();
    unsigned long currentTime = millis();
    if (currentTime >= cronometroUpdateTime)
    {
      reloj();
      cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
    }
    char tecla_presionada = teclado1.getKey();
    if (tecla_presionada == '1' || tecla_presionada == '2' || tecla_presionada == '3' || tecla_presionada == '4' || tecla_presionada == '5' || tecla_presionada == '6' || tecla_presionada == '7' || tecla_presionada == '8' || tecla_presionada == '9' || tecla_presionada == '0')
    {
      lcd.setCursor(6, 1);
      leevalor += tecla_presionada;
      dato = leevalor.toInt();
      lcd.print(leevalor);
      lcd.print("          ");
    }
    if (tecla_presionada == '#')
    {
      if (dato != 0)
      {
        if (dato == correct)
        {
          sendCommand(0x03, 0, 5);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   RESPUESTA");
          lcd.setCursor(0, 1);
          lcd.print("    CORRECTA");
          delay(1000);
          jugando=false;
          enr1 = false;
        }
        else
        {
          verifica_vidas();
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Patron numerico");
          lcd.setCursor(0, 1);
          lcd.print("RSTA:");
          dato = 0;
          leevalor = "";
        }
      }
    }
  }
}
void reto1()
  {

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("AHORCADO");
    delay(2000);

    randomSeed(analogRead(5));
    newWord();

    draw_board();
    ahorca = true;
    while (ahorca == true)
    {
      encender_leds();
      unsigned long currentTime = millis();
      if (currentTime >= cronometroUpdateTime)
      {
        reloj();
        cronometroUpdateTime += CRONOMETRO_UPDATE_INTERVAL;
      }

      int potVal = sel;
      guessLetter = letterVal[potVal];

      if (guessLetter != guessLast)
      {
        guessLast = guessLetter;
        showAsterisk = 0;
        // cycle through all guessed letters and determine whether to show * or the letter
        for (int x = 0; x < guessedCount; x++)
        {
          if (guessLetter == guessed[x])
          {
            showAsterisk = 1;
          }
        }
        // print letters to the left of selected letter
        lcd.setCursor(3, 0);
        for (int x = 5; x >= 1; x--)
        {
          if (potVal - x >= 0)
          {
            lcd.print(letterVal[potVal - x]);
          }
          else
          {
            lcd.print("|");
          }
        }
        // print left arrow
        lcd.write(4);
        // print the letter
        if (showAsterisk == 0)
        {
          lcd.setCursor(9, 0);
          lcd.print(guessLetter);
          alreadyGuessed = 0;
        }
        // print a *
        else
        {
          lcd.setCursor(9, 0);
          lcd.print("*");
          alreadyGuessed = 1;
        }
        // print right arrow
        lcd.write(5);
        // print letters to the right of selected letter
        lcd.setCursor(11, 0);
        for (int x = 1; x <= 5; x++)
        {
          if (potVal + x <= 25)
          {
            lcd.print(letterVal[potVal + x]);
          }
          else
          {
            lcd.print("|");
          }
        }
      }
      int k = inkeys();
      if (k == btnSelect)
      {
        gotOne = 0;
        if (alreadyGuessed == 0)
        {
          alreadyGuessed = 1;
          lcd.setCursor(9, 0);
          lcd.print("*");
          char buf[wordSize + 1];
          guessWord.toCharArray(buf, wordSize + 1);
          for (int i = 0; i < wordSize; i++)
          {
            if (buf[i] == guessLetter)
            {
              lcd.setCursor(i + 3, 1);
              lcd.print(guessLetter);
              gotOne = 1;
              totalRight = totalRight + 1;
            }
          }
          // add letter to guessed letter array
          guessed[guessedCount] = guessLetter;
          guessedCount++;
          // none of the letters match, draw the next body part on the hangman
          if (gotOne == 0)
          {
            
            hangman++;
            draw_hangman(hangman);
          }
          else
          {
            // letter is in word, sound buzzer
            
          }
          // all letters have been guessed...WIN!
          if (totalRight == wordSize)
          {
            gameOver(1);
          }
        }
        // this letter has already been guessed, sound buzzer
      }
      else if (k == btnRight)
      {
        if (sel > 24)
        {
          sel = 0;
        }
        else
        {
          sel++;
        }
      }
      else if (k == btnLeft)
      {
        if (sel <= 0)
        {
          sel = 25;
        }
        else
        {
          sel--;
        }
      }
    }
  }