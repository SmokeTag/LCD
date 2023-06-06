#ifndef __LCD8b_h
    #define __LCD8b_h

    #define  db7  (1<<PD7)
    #define  db6  (1<<PD6)
    #define  db5  (1<<PD5)
    #define  db4  (1<<PD4)
    #define  db3  (1<<PB3)
    #define  db2  (1<<PB2)
    #define  db1  (1<<PB1)
    #define  db0  (1<<PB0)
    #define  rs   (1<<PB5)
    #define  en   (1<<PB4)


    #include <stdio.h>
    #include <inttypes.h>
    #include "Arduino.h"

    void lcd_init();                                             // Inicia o display
    void send_char_pos(char character, char row, char col);      // Envia caracter e posição
    void send_char(char character);                              // Envia caracter
    void char_pos(char row, char col);                           // Define cursor position on display (row, column)
    void lcd_clear();                                            // Limpa o display (Apagar função caso n seja utilizada)
    void set_col(char address);                                  // Define a posição do cursor no display (coluna) [função estrutural]
    void send_byte(char s_byte);                                 // Envia um byte para o barramento de dados [função estrutural]
    void pulse_enable(int time);                                 // Envia um pulso de enable [função estrutural]
#endif