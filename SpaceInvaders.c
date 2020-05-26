// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your gameState
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// *** Required Hardware I/O connections*******
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;


// ********* Images *********
const unsigned char black[] ={
 0x42, 0x4D, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xFF,
 0xF0, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};

const unsigned char blank[] ={
 0x42, 0x4D, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xF0, 0x00,
 0x0F, 0x00, 0xF0, 0x00, 0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,

};
const unsigned char cursor[] ={
 0x42, 0x4D, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0xFF, 0xFF,
 0xFF, 0x00, 0xFF,

};
const unsigned char slider[] ={
 0x42, 0x4D, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00,
 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

};
const unsigned char nullImage[] ={0};
// ********* Capture image dimensions out of BMP****
unsigned long slideMove , playNow ,flag_playNow = 0, flag_slideMove = 0;
int gameState[6][7] ;
int sliderPosition = 0;
int numMoves = 0 , playerScore = 0 , AIScore = 0 ;  




void drawGrid  (void);
void PortF_Init(void);
void moveSlider(void);
void play      (int pos , int player);
int 	AITurn(int depth);
void initBourder() ;
int  Heuristics()  ;
int  minMax()      ;
int isTerminal()   ;
char isFull();
void remove( int column) ;
int Try(int column, int player);
int minimax( int depth, int turn /*1 or 2*/);
int eqMove(int move );
int checkGame(int player);
void clear();
void disableTimer();
void enableTimer();



typedef struct
{
	const unsigned char *image ;
}gameObject;

gameObject Chips[3] = {{nullImage},{ black },{ blank }};  

int main(void){
	char firstPush = 0 ;
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
	PortF_Init(); 
  Random_Init(1);
  Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();
	drawGrid() ;
	
  while(1)
 {
		slideMove   =  GPIO_PORTF_DATA_R & 0x10;
		playNow     =  GPIO_PORTF_DATA_R & 0x01;
		if( slideMove  == 0x00   && flag_slideMove == 0) 
		{
			moveSlider();
			flag_slideMove = 1 ;	
			firstPush = 1 ;
		}
		if(playNow == 0x00 && flag_playNow == 0  && firstPush == 1)
		{
			play( sliderPosition , 1);
			if ( checkGame(1) == 1)
			{
				playerScore++;	
				Nokia5110_ClearBuffer();
				Nokia5110_DisplayBuffer();
				Nokia5110_SetCursor(3,0);
				Nokia5110_OutString("YOU WON");
				Nokia5110_SetCursor(3,1);
				Nokia5110_OutString("SCORE :");
				Nokia5110_SetCursor(11 , 1);
				Nokia5110_OutChar ('0' + playerScore);
				Delay100ms(3);
				Nokia5110_ClearBuffer();
				Nokia5110_DisplayBuffer();
				clear();
				drawGrid() ;
			}
				
      play( AITurn( 2 ), 2); 
			checkGame(2); 
			if ( checkGame(2) == 2)
			{
				Nokia5110_ClearBuffer();
				Nokia5110_DisplayBuffer();
				Nokia5110_SetCursor(3,0);
				Nokia5110_OutString("YOU LOSE");
				Delay100ms(1);
				Nokia5110_ClearBuffer();
				Nokia5110_DisplayBuffer();
		  	clear();
				drawGrid() ;
			
			}	
			flag_playNow = 1 ;
		}
		if( slideMove    != 0x00 )
		{
			flag_slideMove = 0 ;
		}	
		if( playNow != 0x00 )
		{
			flag_playNow = 0 ;
		}	
  }
}
void moveSlider()
{
		initBourder();
	  if(sliderPosition == 7)
		{
			sliderPosition = 0; 
		}
		Nokia5110_PrintBMP( 13 + 8*sliderPosition  , 4, cursor, 0);
		Nokia5110_DisplayBuffer();
		sliderPosition++ ;
		
		
			
} 
void clear()
{
	int i = 0 , j = 0 ;
	for(i = 0 ; i < 7 ; i++){
		for(j = 0 ; j < 7 ; j++) {
			gameState[i][j] = 0 ; 
		}
	}
}
int	AITurn(int depth)
{
	int i =0 , move , pos ;
	int n , test = -10000-1;
	for( i = 0 ; i  < 7; i++){
        if( gameState[6-1][i] == 0 ) {
            Try( i , 2);
            n = minimax( depth , 2 );
            if ( -n > test ) {
                test = -n;
                move = i;
            }
            remove(i);
        }
			}
				pos = move ;
			do{
			  move = eqMove(pos);
				i++;
			}while ( gameState[i][move] > 0 );
			  
	
    return move ;
	
}
void drawGrid()
{
	int i = 0 , j =0;
	for(i = 0 ; i < 7 ; i++){
		initBourder() ;
		for(j = 0 ; j < 7 ; j++) {
		if ( gameState[j][i] > 0 ){
			Nokia5110_PrintBMP(13 +(8*i) ,7 +(8*j), Chips[gameState[i][j]].image, 0); 	
			Nokia5110_DisplayBuffer();
			numMoves++;
	 }
	}
 }
}
void initBourder()
{ 
	int i ;
	for(i = 0 ; i < 7 ; i++){
		
		Nokia5110_PrintBMP(12 + (i * 8) , 8 , slider , 0);	
		Nokia5110_DisplayBuffer();
	}
}

void play(int pos , int player) 
{
	int i = 0 ;
	for( i = 5 ; i > 0 ; i--)
	{
		if( gameState[i][pos] > 0 )
			continue;
		
		gameState[i][pos] = player ;
		Nokia5110_PrintBMP( 13 +(8 * (pos-1)) ,6 + (8 * i) , Chips[player].image, 0);
		Nokia5110_DisplayBuffer();
		break;
	}
	
}
int Heuristics()
{
	int heuristics = 0 ; 
	int i , j ;
	for( i = 0 ; i < 6 ; i++)
  {
		for( j = 0 ; j < 5 ; j++)
		{
			if(gameState[i][j] == gameState[i+1][j] == 0)
			{
				heuristics += 10 ;
			}
			if(gameState[i][j] == gameState[i+1][j] == gameState[i+2][j] == 0)
			{
				heuristics += 100 ;
			}
			if(gameState[i][j] == gameState[i+1][j] == gameState[i+2][j] == gameState[i+2][j] == 0)
			{
				heuristics += 1000 ;
			}
			if(gameState[i][j] == gameState[i+1][j] == 1)
			{
				heuristics -= 10 ;
			}
			if(gameState[i][j] == gameState[i+1][j] == gameState[i+2][j] == 1)
			{
				heuristics -= 100 ;
			}
			if(gameState[i][j] == gameState[i+1][j] == gameState[i+2][j] == gameState[i+2][j] == 1)
			{
				heuristics -= 10000 ;
			} //
			if(gameState[i][j] == gameState[i][j+1] == 0)
			{
				heuristics += 10 ;
			}
			if(gameState[i][j] == gameState[i][j+1] == gameState[i][j+2] == 0)
			{
				heuristics += 100 ;
			}
			if(gameState[i][j] == gameState[i][j+1] == gameState[i][j+2] == gameState[i][j+3] == 0)
			{
				heuristics += 10000 ;
			}
			if(gameState[i][j] == gameState[i][j+1] == 1)
			{
				heuristics -= 10 ;
			}
			if(gameState[i][j] == gameState[i][j+1] == gameState[i][j+2] == 1)
			{
				heuristics -= 100 ;
			}
			if(gameState[i][j] == gameState[i][j+1] == gameState[i][j+2] == gameState[i][j+3] == 1)
			{
				heuristics -= 10000 ;
			}//
			if( ! j+3 > 6  && gameState[i][j] == gameState[i+1][j+1] == 0)
			{
				heuristics += 100 ;
			}
			if(! j+3 > 6   && gameState[i][j] == gameState[i+1][j+1] == gameState[i+2][j+2] == 0)
			{
				heuristics += 100 ;
			}
			if(! j+3 > 6   && gameState[i][j] == gameState[i+1][j+1] == gameState[i+2][j+2] == gameState[i+3][j+3] == 0)
			{
				heuristics += 10000 ;
			}
			if(! j+3 > 6   && gameState[i][j] == gameState[i+1][j+1] == 1)
			{
				heuristics -= 100 ;
			}
			if(! j+3 > 6   && gameState[i][j] == gameState[i+1][j+1] == gameState[i+2][j+2] == 1)
			{
				heuristics -= 100 ;
			}
			if(! j+3 > 6   && gameState[i][j] == gameState[i+1][j+1] == gameState[i+2][j+2] == gameState[i+3][j+3] == 1)
			{
				heuristics -= 10000 ;
			}//
			if( ! j-3 < 0  && gameState[i][j] == gameState[i+1][j-1] == 0)
			{
				heuristics += 10 ;
			}
			if(! j-3 <  0   && gameState[i][j] == gameState[i+1][j-1] == gameState[i+2][j-2] == 0)
			{
				heuristics += 100 ;
			}
			if(! j-3 <  0   && gameState[i][j] == gameState[i+1][j-1] == gameState[i+2][j-2] == gameState[i+3][j-3] == 0)
			{
				heuristics += 10000 ;
			}
			if(! j-3 <  0   && gameState[i][j] == gameState[i+1][j-1] == 1)
			{
				heuristics -= 10 ;
			}
			if(! j-3 <  0   && gameState[i][j] == gameState[i+1][j-1] == gameState[i+2][j-2] == 1)
			{
				heuristics -= 100 ;
			}
			if(! j-3 <  0   && gameState[i][j] == gameState[i+1][j-1] == gameState[i+2][j-2] == gameState[i+3][j-3] == 1)
			{
				heuristics -= 10000 ;
			}
		}
	}
	return heuristics ;
}
int checkGame(int player) {
    int i, j;
 
    
    for(i=0; i<6; i++){
        for(i=0; i<6; i++)
        for(j=0; j<=7-4; j++){
            if(gameState[i][j]== player && gameState[i][j+1]== player && gameState[i][j+2]== player && gameState[i][j+3]== player)
                return player;
        }
 
    
    for(i=0; i<=6-4; i++)
        for(j=0; j<7; j++){
            if(gameState[i][j]== player && gameState[i+1][j]== player && gameState[i+2][j]== player && gameState[i+3][j]== player )
                return player;
        }
 
  
    for(i=6-1; i>=4-1; i--)
        for(j=0; j<=7-4; j++){
            if(gameState[i][j]== player && gameState[i-1][j+1]== player && gameState[i-2][j+2]== player && gameState[i-3][j+3]== player)
                return player;
        }
 
    
    for(i=0; i<=6-4; i++)
        for(j=0; j<=7-4; j++){
            if(gameState[i][j]== player && gameState[i+1][j+1]== player && gameState[i+2][j+2]== player && gameState[i+3][j+3]== player)
                return player;
        }
 
 
   
    for(i=0; i<7; i++)
        if(gameState[6-1][i]==0)
            return 0;
        }
 
    return 3; 
}
char isFull()
{
	if (numMoves >= 42)
	{
		return 1 ;
	}
	else
	{
		return 1 ;
	}
}
void remove( int column) {
    int i;
    for (i=6-1; i>=0; i--){
        if (gameState[i][column] != 0) {
            gameState[i][column] = 0;
            break;
        }
    }
}
int Try(int column, int player) {
 
    int i;
 
    for(i = 0; i < 6 ; i++) {
        if(gameState[i][column] == 0) {
            gameState[i][column] = player;
            return i+1;
        }
    }
		return -1;
}
int minimax( int depth, int turn /*1 or 2*/) {
    int isterminal ;
    int col, best;
    int n;
		isterminal    =  checkGame(3 - turn);
    if(isterminal) {
        if(isterminal ==3)
            return 0;
        if(isterminal ==turn)
            return 10000;
        else
            return -10000;
    }
 
    if(depth==0)
        return ((turn==1) ? Heuristics() : -Heuristics());
 
    best = -10000;
 
    for(col=0; col < 7; col++) {    
        if(gameState[6-1][col] == 0 ) {   
            Try( col, turn);
            n = minimax( depth-1, 3-turn);
            if(turn==1) {
                if ( -n > best ) best = -n;
            } else { 
                if ( -n > best ) best = -n;
            }
            remove( col );
        }
			}
    return best;
}

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0 
}
// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
int eqMove(int move )
{
	int i ;
	do{
		move = (Random()%7)+1 ;
		i++;
	}while ( gameState[i][move] > 0 );
	return move ;
}
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
  Semaphore = 1; // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}