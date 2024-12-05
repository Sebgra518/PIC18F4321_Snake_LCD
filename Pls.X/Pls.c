#include "12864_lcd.h"
#include <xc.h>
#include "pic18f4321-Config.h"
#include <stdbool.h>

#define _XTAL_FREQ 8000000

#define UPBUTTON PORTAbits.RA0
#define DOWNBUTTON PORTAbits.RA1
#define LEFTBUTTON PORTAbits.RA2
#define RIGHTBUTTON PORTAbits.RA3
#define EXITBUTTON PORTAbits.RA4
#define SELECTBUTTON PORTAbits.RA5

unsigned char LCDStore0[17] = "                ";
unsigned char LCDStore1[17] = "                ";
unsigned char LCDStore2[17] = "                ";
unsigned char LCDStore3[17] = "                ";
unsigned char LCDStore4[17] = "                ";
unsigned char LCDStore5[17] = "                ";
unsigned char LCDStore6[17] = "                ";
unsigned char LCDStore7[17] = "                ";

unsigned char snakeHeadXPos, snakeHeadYPos;
unsigned char snakeTailX[32], snakeTailY[32];
unsigned char snakeTailLen;

unsigned char fruitX;
unsigned char fruitY;

unsigned char score, key;

unsigned char speed = 0;

bool looseScreen = false;
bool game = false;
bool mainMenu = true;
bool gameover = false;

void displaySpeed(){
    switch(speed){
        case 0:
             LCD_GraphicsDisplayString(6,"Speed: 0");
             break;
        case 1:
             LCD_GraphicsDisplayString(6,"Speed: 1");
             break;
        case 2:
             LCD_GraphicsDisplayString(6,"Speed: 2");
             break;
        case 3:
             LCD_GraphicsDisplayString(6,"Speed: 3");
             break;
        case 4:
             LCD_GraphicsDisplayString(6,"Speed: 4");
             break;
        case 5:
             LCD_GraphicsDisplayString(6,"Speed: 5");
             break;
    }
}

void drawGraphics(){
    LCD_GraphicsDisplayString(0,LCDStore0);
    LCD_GraphicsDisplayString(1,LCDStore1);
    LCD_GraphicsDisplayString(2,LCDStore2);
    LCD_GraphicsDisplayString(3,LCDStore3);
    LCD_GraphicsDisplayString(4,LCDStore4);
    LCD_GraphicsDisplayString(5,LCDStore5);
    LCD_GraphicsDisplayString(6,LCDStore6);
    LCD_GraphicsDisplayString(7,LCDStore7);
}
unsigned char random(unsigned char lower_bound,unsigned char upper_bound){
    return rand() % (upper_bound - lower_bound + 1) + lower_bound;
}
void input() {
    if(LEFTBUTTON == 1)
        key = 1;
    else if(RIGHTBUTTON == 1)
        key = 2;
    else if(UPBUTTON == 1)
        key = 3;
    else if(DOWNBUTTON == 1)
        key = 4;
    else if (EXITBUTTON == 1)
        gameover = true;
}
void placeChar(unsigned char x, unsigned char y, unsigned char c){
    switch (y) {
        case 0:
              LCDStore0[x] = c;
          break;
        case 1:
              LCDStore1[x] = c;
          break;
        case 2:
              LCDStore2[x] = c;
          break;
        case 3:
              LCDStore3[x] = c;
          break;
        case 4:
              LCDStore4[x] = c;
          break;
        case 5:
              LCDStore5[x] = c;
          break;
        case 6:
              LCDStore6[x] = c;
          break;
        case 7:
              LCDStore7[x] = c;
          break;
    }
    
}
void setup() {
    
    TRISA = 0x0F;
    
    // Flag to signal the gameover
    gameover = 0;

    // Initial coordinates of the snake
    snakeHeadXPos = 7;
    snakeHeadYPos = 4;
    
    // Initial coordinates of the fruit
    fruitX = random(0,15);
    fruitY = random(0,7);
    
    while (fruitX == 0)
        fruitX = random(0,15);

    while (fruitY == 0)
        fruitY = random(0,7);

    // Score initialzed
    score = 0;
    snakeTailLen = 0;
    key = 0;
}
void clearAllChar(){
    for(unsigned char i = 0; i < 8; i++){
         for(unsigned char j = 0; j < 16; j++){
             placeChar(j,i,' ');
        }
    }
}
void logic() {
    
    // Updating the coordinates for continous
    // movement of snake
    unsigned char prevX = snakeTailX[0];
    unsigned char prevY = snakeTailY[0];
    unsigned char prev2X, prev2Y;
    snakeTailX[0] = snakeHeadXPos;
    snakeTailY[0] = snakeHeadYPos;
    for (unsigned char  i = 1; i < snakeTailLen; i++) {
        prev2X = snakeTailX[i];
        prev2Y = snakeTailY[i];
        snakeTailX[i] = prevX;
        snakeTailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    
    // Changing the direction of movement of snake
    switch (key) {
    case 1:
        snakeHeadXPos--;
        break;
    case 2:
        snakeHeadXPos++;
        break;
    case 3:
        snakeHeadYPos--;
        break;
    case 4:
        snakeHeadYPos++;
        break;
    default:
        break;
    }

    // If the game is over
    if (snakeHeadXPos < 0 || snakeHeadXPos >= 16 || snakeHeadYPos < 0 || snakeHeadYPos >= 8)
        gameover = 1;
        
    // Checks for collision with the tail (o)
    for (unsigned char i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == snakeHeadXPos && snakeTailY[i] == snakeHeadYPos)
            gameover = 1;
    }

    // If snake reaches the fruit
    // then update the score
    if (snakeHeadXPos == fruitX && snakeHeadYPos == fruitY) {
        fruitX = random(0,15);
        fruitY = random(0,7);
        while (fruitX == 0)
            fruitX = random(0,15);

        // Generation of new fruit
        while (fruitY == 0)
            fruitY = random(0,7);
         score += 1;
         snakeTailLen++;
    }
}

void draw() {
    clearAllChar();
    placeChar(snakeHeadXPos,snakeHeadYPos,'O');
    placeChar(fruitX,fruitY,'a');
    
    
    for (unsigned char i = 0; i < 8; i++) {
        for (unsigned char j = 0; j <= 16; j++) {
                unsigned char prTail = 0;
                for (unsigned char k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == j && snakeTailY[k] == i) {
                        placeChar(j,i,'#');
                        prTail = 1;
                    }
                }
        }
            
    }
    
    drawGraphics();
}

void main(void) {
    ADCON1 = 0x0F;
    OSCCON = 0x70;
    
    LCD_Init();
    LCD_EnableGraphics();
    LCD_ClearGraphics();

    LCD_GraphicsDisplayString(0,"*****SNAKE******");
   
    LCD_GraphicsDisplayString(2,"BY:  SEBASTIAN  ");
    LCD_GraphicsDisplayString(3,"     ANU        ");
    LCD_GraphicsDisplayString(5,"Start - Select  ");
    
showSpeed:
    displaySpeed();
    
    while(!SELECTBUTTON){
        if(UPBUTTON && speed < 5){
            speed++;
            while(UPBUTTON);
            goto showSpeed;
        } else if(DOWNBUTTON && speed > 0){
            speed--;
            while(DOWNBUTTON);
            goto showSpeed;
        } 
    }
    
    
    
    
startGame:
    setup();
    
    while(!gameover){
           draw();
           input();
           logic();
           
           switch (speed){
               case 0:
                __delay_ms(0);
                break;
                case 1:
                __delay_ms(100);
                break;
                case 2:
                __delay_ms(200);
                break;
                case 3:
                __delay_ms(300);
                break;
                case 4:
                __delay_ms(400);
                break;
                case 5:
                __delay_ms(500);
                break;
           }
     }
    LCD_ClearGraphics();
    LCD_GraphicsDisplayString(1,"***************");
    LCD_GraphicsDisplayString(2,"    GAME OVER   ");
    LCD_GraphicsDisplayString(3,"***************");
    LCD_GraphicsDisplayString(5,"Retry - Select");
    LCD_GraphicsDisplayString(6,"Menu - Exit");
    
    while(!EXITBUTTON){
       if(SELECTBUTTON){
           goto startGame;
       } 
    }
    
    return; 
}