/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

// define input pins these are the pins on the arduino they never change so #define
#define INTPIN    3       // only pins 2 and 3 can be interupt pins on UNO  
#define UPPIN     4       // these are pins connected tp relevant switch
#define DWNPIN    5
#define LFTPIN    6
#define RHTPIN    7

// define directions
#define DIRUP     1       // these values is what the "snake" looks at to decide-
#define DIRDOWN   2       // the direction the snake will travel
#define DIRLEFT   3
#define DIRRIGHT  4

// set button variables

// volitile cos we need it to update with the interupt so can be any bit of cycle value 
// is never higher than 4 so only need 8bit int to save resources
volatile uint8_t buttonpressed=0;        
bool butup=0;
bool butdown=0;   // we use this to set true to "detect" which direction pressed
bool butleft=0;
bool butright=0;

int x=0;

// world ints

uint8_t worldMinX=0;        // these set the limits of the play area
uint8_t worldMaxX=128;
uint8_t worldMinY=10;
uint8_t worldMaxY=63;

void interruptpressed() {
    delay(200);  // slight delay for added "bounce" protection
    updatedirection();
}
// ------------------ update the direction value from button press -----------------
void updatedirection() {
    // Serial.println("updatingdirection");
    butup=digitalRead(UPPIN);       // check which input went high and set relevant bool true
    butdown=digitalRead(DWNPIN);
    butleft=digitalRead(LFTPIN);
    butright=digitalRead(RHTPIN);
    // these if statemeents look at which input went high and enters the relevant value in "buttonpressed"
    // variable, this variable dictates the direction of movement
    if(butup==true)
     {
        buttonpressed=DIRUP;
         Serial.println("UP pressed");
    //     Serial.println(buttonpressed);
        butup=false;
        //tone(SND,1500,10);
    }
    if(butdown==true)
    {
        buttonpressed=DIRDOWN;
         Serial.println("DOWN pressed");
    //     Serial.println(buttonpressed);
        butdown=false;
        //tone(SND,1500,10);
    }
    
    if(butleft==true)
    {
        buttonpressed=DIRLEFT;
        Serial.println("LEFT pressed");
    //    Serial.println(buttonpressed);
        butleft=false;
        //tone(SND,1500,10);
    }
    if(butright==true)
    {
         buttonpressed=DIRRIGHT;
         Serial.println("RIGHT pressed");
    //     Serial.println(buttonpressed);
          butright=false;
          //tone(SND,1500,10);
    }
}


void drawBall(int x, int y){
  display.clearDisplay();

  for(int16_t i=0; i<3; i++) {
    display.fillCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
  }
}
     
void setup() {
  delay(100);
   Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  pinMode(INTPIN,INPUT);  // set the correct ports to inputs
  pinMode(UPPIN,INPUT);
  pinMode(DWNPIN,INPUT);
  pinMode(LFTPIN,INPUT);
  pinMode(RHTPIN,INPUT);

  attachInterrupt(digitalPinToInterrupt(INTPIN),interruptpressed,RISING); 
  Serial.println("Setup Passed");
//  waitForPress();    // display the snake start up screen
}

void loop() {
  display.clearDisplay();
  // put your main code here, to run repeatedly:
  delay(1);
  display.drawPixel(x, 0, WHITE);
  display.display();
  x++;
  if (x > worldMaxX) {
    x = 0;
  }
}
