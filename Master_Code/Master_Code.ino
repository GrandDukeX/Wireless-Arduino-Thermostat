/* 
  Design Project - Master Code 
  This is a wireless thermostat that consist of 4 parts, a temperature sensor,   a gas sensor, a buzzer that indicates the severity of the gas level, and   using the USART to add Bluetooth functionality. 
  By: Andy Garcia 
  Written: May 1, 2020 
  Edited: July 14, 2020 
  I/O Pins   
  A0:  
  A1:  
  A2: 
  A3: 
  A4: 
  A5: 
  D0: Rx for Bluetooth Module (HC-05) 
  D1:  
  D2:   
  D3: 
  D4:  
  D5:   
  D6:   
  D7:   
  D8:  LCD Register Select (RS) 
  D9:  LCD Enable (E) 
  D10: LCD Data Bit 4 (DB4) 
  D11: LCD Data Bit 5 (DB5) 
  D12: LCD Data Bit 6 (DB6)   
  D13: LCD Data Bit 7 (DB7) 
*/ 
 
// include the library that contains all of the LCD functions and constants 
#include "hd44780.h" 
 
void setup() { 
cli(); 
//Initialize LCD lcd_init(); 
//Setting the USART for Bluetooth 
//Setting up the Baud Registers 
UBRR0H = 0x00; 
UBRR0L = 0x67; 
UBRR0 = 25; 
UCSR0A = 0x00; 
UCSR0B = 0x90; //Reciever   
UCSR0C = 0x0E; 
sei(); 
} 
 
volatile int temperature = 0; //Declare temperature variable 
volatile unsigned int gas = 0; //Formula to calculate gas levels 
void loop() {   
  lcd_clrscr(); //Clear LCD 
 
  // creating a decimal point for temp reading   
  unsigned char tempLeft = temperature / 10;   
  unsigned char tempRight = temperature % 10; 
 
// Converting values of temp into a string 
char charBuffer[4]; 
char charBuffer2[2]; 
lcd_puts("Temp: "); 
itoa(tempLeft, charBuffer, 10);   
itoa(tempRight, charBuffer2, 10); 
 
// Displaying values of temperature on LCD   
lcd_puts(charBuffer);   
lcd_puts(".");   
lcd_puts(charBuffer2);   
lcd_putc((char)223);   
lcd_puts("F"); 
 
// creating a decimal point for gas reading   
unsigned char gasLeft = gas / 10 ;   
unsigned char gasRight = gas % 10; 
 
// Converting values of CO2 into a string 
lcd_goto(40); lcd_puts("CO2: "); 
itoa(gasLeft, charBuffer, 10); 
itoa(gasRight, charBuffer2, 10); 
 
// Displaying values of CO2 on LCD   
lcd_puts(charBuffer);   
lcd_puts(".");   
lcd_puts(charBuffer2);   
lcd_puts("%"); 
_delay_ms(24); 
} 
 
ISR(USART_RX_vect) 
{ 
  //Needs to be multiplied by 2 for uncompression   
  gas = UDR0 * 2;   while (!(UCSR0A & 0x20)){ 
  } 
  //Needs to be multiplied by 4 for uncompression   
  temperature = UDR0 * 4; 
} 
