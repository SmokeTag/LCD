#include "LCD8b.h"


void lcd_init() {
   //Set outputs
   DDRB |= db0;
   DDRB |= db1;
   DDRB |= db2;
   DDRB |= db3;
   DDRB |= rs;
   DDRB |= en;
   DDRD |= db4;
   DDRD |= db5;
   DDRD |= db6;
   DDRD |= db7;

   //inicializa saídas               --------------------------------------------
   PORTB &= ~en;
   PORTB &= ~rs;
   send_byte(0x00);
   delayMicroseconds(20);
   
   // LCD CLEAR
   //send_byte(0x01);
   //pulse_enable(45);

   // FUNCTION SET
   send_byte(0x38); //8-bit; 2-lines; 5x8-dots
   pulse_enable(50);

    // DISPLAY ON/OFF CONTROL
   send_byte(0x0C); // Display on; Cursor off; Blinking off
   pulse_enable(50);

   // HABILITA INCREMENTO, DESLIGA SCROLL
   //send_byte(0x06);
   //pulse_enable(50);
  
} //end lcd_init


void send_char_pos(char character, char row, char col) {
  char_pos(row, col);
  send_char(character); 
} //end send_char_pos


void send_char(char character) {
   send_byte(character); 
   PORTB |= rs;
   delayMicroseconds(5);
   pulse_enable(50);
   PORTB &= ~rs;
} //end send_char_pos


void char_pos(char row, char col) {
   col -= 1;
   switch(row) {
      case 1: set_col(col);      break; 
      case 2: set_col(col+0x40); break;
   } //end switch
} //end char_pos


void lcd_clear() {
   send_byte(0x01);
   pulse_enable(100);
} //end lcd_clear


void set_col(char address) {
  send_byte(address);
  PORTD |= db7; 
  pulse_enable(40);
} //end set_col


void send_byte(char s_byte) {
  PORTB &= ~(db0 | db1 | db2 | db3);
  PORTD &= ~(db4| db5 | db6 | db7);

  PORTB |= s_byte & (db3 | db2 | db1 | db0);
  PORTD |= s_byte & (db4 | db5 | db6 | db7);
} //end send_byte


void pulse_enable(int time) {
   PORTB |= en;
   delayMicroseconds(1);
   PORTB &= ~en;
   delayMicroseconds(time);
} //end pulse_enable