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

// world ints

uint8_t worldMinX=0;        // these set the limits of the play area
uint8_t worldMaxX=128;
uint8_t worldMinY=0;
uint8_t worldMaxY=63;

// ball x,y is center of ball, with 
uint8_t ballX=worldMaxX/2;
uint8_t ballY=worldMaxY/2;
int balldX=1;
int balldY=1;
uint8_t ballRadius=3;

// player paddle
int paddleX=5;
int paddleY=worldMaxY/2;
uint8_t paddleW=3;
uint8_t paddleH=10;

// opponent paddle
int oppX=worldMaxX-5;
int oppY=worldMaxY/2;

uint8_t playerScore = 0;
uint8_t oppScore = 0;

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


void drawBall(int x, int y) {

  for(int16_t i=0; i<ballRadius; i++) {
    display.fillCircle(x, y, i, SSD1306_WHITE);
  }
}

bool checkCollision() {
  // check if ball hits top/bottom wall or paddles
  bool collide = false;

  // top/bot wall
  if (ballY <= 4 || ballY >= 59) {
    balldY = balldY*-1;
    collide= true;
  }

  // temp left right wall
  if (ballX <= 4 || ballX >= 124) {
    balldX *= -1;
    collide=true;
  }

  // check for paddle- ball must be travelling left (-ve)
  if ((ballX >= paddleX && ballX <= (paddleX+paddleW)) && (ballY >= paddleY && ballY <= (paddleY+paddleH)) && (balldX < 0)) {
    balldX *= -1;
    Serial.println("PADDLE");
    collide=true;
  }
  
  return collide;
}

bool Score() {
  if (ballX <= (worldMinX + ballRadius + 1)) {
    // opponent scores
    oppScore++;
    Serial.println("Opponent Scored");
    return true;
  }

  if (ballX >= (worldMaxX - ballRadius - 1)) {
    // player scores
    playerScore++;
    Serial.println("SCOREEE");
    return true;
  }

  return false;
}

void updateDisplay()  // draw scores and outlines  
{
       // Serial.println("Update Display");
//
//        display.fillRect(0,0, display.width()-1,8,BLACK);
        display.setTextSize(0);
        display.setTextColor(WHITE);
//       
//        // draw scores
    display.setCursor((worldMaxX/2-10),1);
    display.print(String(playerScore, DEC));
    display.setCursor((worldMaxX/2+10),1);
    display.print(String(oppScore, DEC));
//        display.setCursor(66,1);
//        display.print("High:");
//        display.print(String(highscore ,DEC));
//        // draw play area
//        //        pos  1x,1y, 2x,2y,colour
//        display.drawLine(0,0,127,0,WHITE); // very top border
//        display.drawLine(63,0,63,9,WHITE); // score seperator
//        display.drawLine(0,9, 127,9,WHITE); // below text border
//        display.drawLine(0,63,127,63,WHITE); // bottom border
//        display.drawLine(0,0,0,63,WHITE); // left border
//        display.drawLine(127,0,127,63,WHITE); //right border

    // draw ball
    drawBall(ballX, ballY);
    // draw paddles
    display.fillRect(paddleX, paddleY, paddleW, paddleH, WHITE);

}

void updateGame()     // this updates the game area display
{
    display.clearDisplay();

    switch(buttonpressed) // move paddle
          {
            case DIRUP:
                if (paddleY > worldMinY) {
                  paddleY-=1;
                }
                break;
            case DIRDOWN:
                if (paddleY < worldMaxY - paddleH) {
                  paddleY+=1;
                }
                break;
          }
    // check if button is still held
    if (!digitalRead(INTPIN)) {
      buttonpressed = false;
    }

    // check collision
    bool collide = checkCollision();  
//    Serial.print("Collide: ");
//    Serial.print(collide);
//    Serial.print(", ballY: ");
//    Serial.print(ballY);
//    Serial.print(", balldY: ");
//    Serial.println(balldY);

    // check for score
    bool scored = Score();
//          if (checkScored()) {
//            
//          }

    // move ball
    ballX += balldX;
    ballY += balldY;

    updateDisplay();
    display.display();
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
  updateGame();
}
