/* 
  Design Project - Slave Code 
  This is a wireless thermostat that consist of 4 parts, a temperature sensor,   a gas sensor, a buzzer that indicates the severity of the gas level, and   using the USART to add bluetooth functionality. 
  By: Andy Garcia 
  Written: May 1, 2020 
  Edited: July 14, 2020 
  I/O Pins 
  A0: Temperature Sensor (TMP-36) 
  A1: Gas Sensor (MQ-2) 
  A2: 
  A3: 
  A4: 
  A5: 
  D0:  
  D1: Tx for Bluetooth Module (HC-05) 
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
 
volatile unsigned int result = 0; 
 
unsigned long ave(unsigned int arr[], unsigned char k)  { 
  //Calculates the average of n temperature readings   
  unsigned long sum = 0;   
  for (unsigned char j = 0; j < k; j++){ 
    sum = sum + arr[j]; 
  } 
  sum = sum / k; return sum; 
} 
 
void setup() { 
  cli(); 
  //initialize LCD   lcd_init(); 
  //Setting ADC registers 
  ADCSRA = 0xEF; 
  ADCSRB = 0x00; 
  ADMUX = 0x40; 
  //Configure Interrupt to trigger every 350ms when TCNT1 = OCR1A 
  TIMSK1 = 0x02; 
  TCCR1A = 0x00; 
  TCCR1B = 0x0C; 
  TCCR1C = 0x00; 
  OCR1A = 21875; 
  //Setting Active Buzzer PWM 
  TCCR0A = 0x23; 
  TCCR0B = 0x0B; 
  OCR0A = 255; 
  DDRD = 0x20; 
  //Setting the USART for Bluetooth 
  //Setting up the Baud Registers 
  UBRR0H = 0x00; 
  UBRR0L = 0x67; 
  UBRR0 = 25; //Baud rate is 38400 
  UCSR0A = 0x00; 
  UCSR0B = 0x08; //Transmitter   UCSR0C = 0x0E; 
  sei(); 
} 
 
//Variable that determines which analog pin to read from static unsigned char z = 0; 
 
void loop() {   
  lcd_clrscr(); //Clear LCD   
  int temperature; //Declare temperature variable   
  unsigned int gas; //Formula to calculate gas levels   
  static unsigned long x = 0; //Start of the loop 
   
  if (z == 0){ 
  // Offset is 2 w/o smoke sensor   
  // 6.6 w/ smoke sensor   
  unsigned char offset = 4;    
  const unsigned char n = 2; //Number of samples   
  static unsigned int ADCvalues[n] = {}; //Array where the temperature reading will go   
  ADCvalues[x%n] = result; //Result is the ADC reading (temperature sensor)   
  unsigned int avg = ave(ADCvalues, n); //Temperature Formula & multiplied by 10 to get decimal   
  int temperatureCelcius = 500L * avg/1023 - 50 - offset; // Converting from celsius into Fahrenheit  
  temperature = (temperatureCelcius * 90L / 5) + 320; 
} 
if (z == 1) { 
  const unsigned char m = 5; //Number of samples 
  static unsigned int ADCvalues2[m] = {}; //Array where the gas reading will go 
  ADCvalues2[x%m] = result; //Result is the ADC reading (MQ-2 sensor)   
  unsigned int avg2 = ave(ADCvalues2, m);   
  gas = (avg2 * 122L/100 - 17073/100);     
  if (avg2 > 400) { //If Gas levels exceed 30% 
      OCR0B = avg2 * 2656L/10000; 
    }     else { 
      OCR0B = 0; 
    } 
  } 
   
  // creating a decimal point for temp reading   
  unsigned char tempLeft = temperature / 10;   
  unsigned char tempRight = temperature % 10; 
   
  // Converting values of temp into a string 
  char charBuffer[4]; 
  char charBuffer2[2]; 
  lcd_puts("Temp: ");   itoa(tempLeft, charBuffer, 10);   itoa(tempRight, charBuffer2, 10); 
 
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
 
  if (UCSR0A & 0x20){ 
    //Needs to be divided by 4 for compression     
    UDR0 = temperature / 4;      
    while (!(UCSR0A & 0x20)){ 
    } 
    //Needs to be divided by 2 for compression 
    UDR0 = gas / 2; 
  } 
  x++; 
} 
 
//For ADC  
ISR(ADC_vect) {   
  result = ADC; 
} 
 
//Timed Interrupt to toggle between A0/A1 pin 
ISR(TIMER1_COMPA_vect) { 
   
  //Toggle z   
  z ^= 0x01;   
  //Read from A0 
  if (z == 0){ 
    ADMUX = 0x40; 
  } 
  //Read from A1 
  if (z == 1){ 
    ADMUX = 0x41; 
  } 
   
}
