//fully working 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <OneWire.h>
#include <QMC5883LCompass.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1      // Reset pin # (or -1 if sharing Arduino reset pin)
#define TDS_SENSOR_PIN 36  // Analog pin for TDS sensor

#define SCREEN_ADDRESS 0x3C

const int button1Pin = 2;  // the number of the pushbutton pin to switch on
const int button2Pin = 5;  // the number of the pushbutton pin for sos
const int ledPin = 4;      // the number of the LED pin

OneWire oneWire(TDS_SENSOR_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define OLED_SDA 21  // SDA pin for OLED display
#define OLED_SCL 22  // SCL pin for OLED display

QMC5883LCompass compass;


// variables will change:
int button1State = 0;  // variable for reading the pushbutton status
int button2State = 0;

int switchMode = 3; // switching the button

int switchScreen = 0;



void setup() {

  Serial.begin(9600);
  // Initialize device with i2c 0x0D address
  compass.init();

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Clear the display buffer
  display.clearDisplay();

  // Set text color to white
  display.setTextColor(SSD1306_WHITE);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
}

void loop() {

  float tdsValue = analogRead(TDS_SENSOR_PIN);  // Read TDS sensor value
  float voltage = tdsValue/4096.0*3.3;
  float ppm = (133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage)*0.5;
  
  switchScreen ++;
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Read compass values
  compass.read();
  float azimut = compass.getAzimuth();
  if (azimut < 0) {
    azimut = azimut + 360;
  }

  // read the state of the pushbutton value:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (button1State == HIGH) {
    // increment switchMode
    switchMode++;
    switchScreen = 1;
    // reset switchMode if it's greater than 3
    if (switchMode > 3) {
      switchMode = 1;
    }
    // delay to debounce the button press
    delay(500);
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (button2State == HIGH) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    for (int j = 0; j < 3; j++) {
      // SOS signal: 3 short blinks, 3 long blinks, 3 short blinks
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(300);
        digitalWrite(ledPin, LOW);
        delay(300);
      }
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(700);
        digitalWrite(ledPin, LOW);
        delay(300);
      }
      for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(300);
        digitalWrite(ledPin, LOW);
        delay(300);
      }
      delay(1000);  // wait for a second before next possible SOS signal
    }

  } else {
    switch (switchMode) {
    case 1:
    //insert display code
      if (switchScreen == 1){
          display.clearDisplay();
          // Set text cursor position
          display.setFont(&FreeSansBold12pt7b);

          display.setTextSize(1);
          centerText("Compass",50);
          display.println("");
          display.display();
          delay(2000);
          display.clearDisplay();
          
      } else{
          // Clear the display buffer
          display.clearDisplay();
          // Set text size
          display.setFont(&FreeSansBold18pt7b);
          // Write direction
          if ((azimut < 22.5) || (azimut > 337.5)) centerText("N",33);
          if ((azimut > 22.5) && (azimut < 67.5)) centerText("NE",33);
          if ((azimut > 67.5) && (azimut < 112.5)) centerText("E",33);
          if ((azimut > 112.5) && (azimut < 157.5)) centerText("SE",33);
          if ((azimut > 157.5) && (azimut < 202.5)) centerText("S",33);
          if ((azimut > 202.5) && (azimut < 247.5)) centerText("SW",33);
          if ((azimut > 247.5) && (azimut < 292.5)) centerText("W",33);
          if ((azimut > 292.5) && (azimut < 337.5)) centerText("NW",33);

          display.setFont(&FreeSans9pt7b);
          display.setCursor(0, 55);
          display.print(azimut);
          display.println("  degrees");

          // Display the buffer
          display.display();
          break;
      }
    case 2:
      if ((switchScreen == 1) && (switchMode == 2)){
          display.clearDisplay();               // Clear the buffer
          display.setTextColor(SSD1306_WHITE);  // Set text color to white
          display.setCursor(0, 0);              // Set cursor position to top-left
          // Print TDS sensor value to OLED display
          display.setFont(&FreeSansBold12pt7b);
          centerText("Water",25);
          centerText("Purity",60);
          display.display();
          delay(2000);

          display.clearDisplay();
      } else if((switchScreen != 1) && (switchMode == 2)){
          
          display.clearDisplay();
          display.setFont(&FreeSansBold18pt7b);
          if (ppm <= 1) {
            centerText("Poor",25);
          } else if (ppm <= 15) {
            centerText("Ideal",25);
          } else if (ppm <= 35) {
            centerText("Good",25);
          } else if (ppm <= 60) {
            centerText("Fair",25);
          } else if (ppm > 60) {
            centerText("Poor",25);
          }
          display.setFont(&FreeSans9pt7b);
          display.setCursor(0, 55);
          display.print(ppm);
          display.println(" ppm");
          display.display();  // Display values


      break;
      }
    case 3:
      display.clearDisplay();
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      break;

    }
  }

delay(250);
}

void centerText(const char* text, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h); // Calculate the width/height of the text
  int x = (SCREEN_WIDTH - w) / 2; // Calculate the x position
  
  display.setCursor(x, y);
  display.println(text);
}