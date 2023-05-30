// =====================================================================================
// --- Mapeamento de Hardware ---
#define  db7  (1<<PD7)      // LCD
#define  db6  (1<<PD6)      // LCD
#define  db5  (1<<PD5)      // LCD
#define  db4  (1<<PD4)      // LCD
#define  db3  (1<<PB3)      // LCD
#define  db2  (1<<PB2)      // LCD
#define  db1  (1<<PB1)      // LCD
#define  db0  (1<<PB0)      // LCD

#define  rs   (1<<PB5)      // LCD
#define  en   (1<<PB4)      // LCD

#define  bt_pause (1<<PC4)  // Botão conectado ao pino A0
#define  bt_reset (1<<PC5)  // Botão conectado ao pino A1


// =====================================================================================
// --- Constantes do Projeto ---
const int debounce_time = 30;
const int reset_time = 800;
const int regret_time = 6000;

// =====================================================================================
// --- Protótipo das Funções ---
void read_bt_pause();                                           // Lê botão fazendo debounce digital
void read_bt_reset();                                           // Lê um botão fazendo debounce digital
void lcd_update();                                              // Atualiza o display (somente cronometro)
void char_pos(char row, char col);                              // Define a posição do cursor no display (linha, coluna)
void set_col(char address);                                     // Define a posição do cursor no display (coluna) [função estrutural]
void send_char_pos(char character, char row, char col);         // Envia caracter e posição
void send_char(char character);                                 // Envia caracter
void lcd_init();                                                // Inicia o display
void lcd_clear();                                               // Limpa o display (Apagar função caso n seja utilizada)
void send_byte(char s_byte);                                    // Envia um byte para o barramento de dados [função estrutural]
void pulse_enable();                                            // Envia um pulso de enable [função estrutural]


// =====================================================================================
// --- Variáveis Globais ---
// Interrupções externas

// Cronômetro
volatile unsigned long tempo_anterior = 0;
volatile int horas = 0;
volatile int minutos = 0;
volatile int segundos = 0;
volatile int milissegundos = 0;
volatile char dez, uni;

// Flags
volatile bool pause_flag = 0x01;


// =====================================================================================
// --- Configurações Iniciais ---
void setup()
{
// Inicializa LCD
  lcd_init();
  send_char_pos('0',1,1);
  send_char(':');
  send_char('0');
  send_char('0');
  send_char(':');
  send_char('0');
  send_char('0');
  send_char(':');
  send_char('0');
  send_char('0');
  send_char(255);
  send_char(255);
  send_char(255);
  send_char(255);
  send_char(255);
  send_char(255);
  send_char(255);
  send_char(255);


  DDRC &= ~bt_pause;                                            // Configura bt_pause como entrada
  DDRC &= ~bt_reset;                                            // Configura bt_reset como entrada
  PORTC |= bt_pause;                                            // Habilita pull-up no bt_pause
  PORTC |= bt_reset;                                            // Habilita pull-up no bt_reset

  PCICR |= (1<<PCIE1);                                          // Habilita interrupções no port C
  PCMSK1 |= bt_pause;                                           // Habilita interrupções no bt_pause
  PCMSK1 |= bt_reset;                                           // Habilita interrupções no bt_reset
  sei();                                                        // Habilita interrupções globais

} //end setup
 
// =====================================================================================
// --- Loop Infinito ---
void loop()
{

  if (!(pause_flag)) /* Possível implementação de screenshot do tempo!? */
  {

    unsigned long tempo_atual = millis();
    unsigned long tempo_decorrido = tempo_atual - tempo_anterior;
    milissegundos += tempo_decorrido;
    tempo_anterior = tempo_atual;

    if (milissegundos >= 1000) {
      milissegundos -= 1000;
      segundos++;

      if (segundos >= 60) {
        segundos -= 60;
        minutos++;

        if (minutos >= 60) {
          minutos -= 60;
          horas++;

        } //end if minutos
      } //end if segundos
    } //end if milissegundo
    lcd_update();
  } // end if pause_flag
} //end loop


// =====================================================================================
// --- Desenvolvimento das Interrupções ---


ISR(PCINT1_vect)
{
  read_bt_pause();
  read_bt_reset();
} //end ISR (port C)


// =====================================================================================
// --- Desenvolvimento das Funções ---


void read_bt_pause()
{
  static bool bt_flag = 0x00;                                   // Inicia flag de precionamento do botão
  static unsigned long bt_bounce = 0;                           // Inicial a variável bt_bounce

  if(!(PINC & bt_pause) && !bt_flag && (millis() - bt_bounce > debounce_time))          // Verifica se o botão foi pressionado (Aqui o único propósito é evitar o bounce do botão)
  {
    bt_bounce = millis();                                       // Atualiza o tempo desde o último bounce
    bt_flag = 0x01;                                             // Ativa a flag
    return;
  }

  if(PINC & bt_pause && bt_flag && (millis() - bt_bounce > debounce_time) && (millis() - bt_bounce < reset_time))      // Verifica se o botão foi solto, se a flag esta ativa e se o tempo desde o ultimo bouce é maior que debounce_time
  {
    bt_flag = 0x00;
    tempo_anterior = millis();
    bt_bounce = tempo_anterior + 35;
    pause_flag ^= 0x01;                                         // Inverte o valor da pause_flag
    return;
  }

  if (PINC & bt_pause && bt_flag && pause_flag && (millis() - bt_bounce > reset_time) && (millis() - bt_bounce < regret_time))
  {
    bt_flag = 0x00;
    bt_bounce = millis();

    horas = 0;
    minutos = 0;
    segundos = 0;
    milissegundos = 0;
    lcd_update();
    return;
  }
    if (PINC & bt_pause && bt_flag && (millis() - bt_bounce > reset_time))
  {
    bt_flag = 0x00;
    bt_bounce = millis();
    return;
  }
}


void read_bt_reset()
{
  static bool bt_flag = 0x00;                             // Inicia flag de precionamento do botão
  static unsigned long bt_bounce = 0;                           // Inicial a variável bt_bounce

  if(!(PINC & bt_reset))      // Verifica se o botão foi pressionado (Aqui o único propósito é evitar o bounce do botão)
  {
    bt_bounce = millis();  
    bt_flag = 0x01;                                       // Ativa a flag
    pause_flag = 0x01;                                          // Pausa o Cronometro
  }

  if(PINC & bt_reset && bt_flag && (millis() - bt_bounce > debounce_time))      // Verifica se o botão foi solto, se a flag esta ativa e se o tempo desde o ultimo bouce é maior que debounce_time
  {
    bt_flag = 0x00;
    horas = 0;
    minutos = 0;
    segundos = 0;
    milissegundos = 0;
    lcd_update();
  }
}


void lcd_update()
{
  send_char_pos(horas+48,1,1);
  dez = minutos/10;
  uni = minutos%10;
  send_char_pos(dez+48,1,3);
  send_char(uni+48);
  dez = segundos/10;
  uni = segundos%10;
  send_char_pos(dez+48,1,6);
  send_char(uni+48);
  dez = milissegundos/100;
  uni = milissegundos%100/10;
  send_char_pos(dez+48,1,9);
  send_char(uni+48);
}


void set_col(char address)
{
  PORTB &= ~rs;
  send_byte(address);
  PORTD |= db7; 
  pulse_enable();
   
} //end set_col


void char_pos(char row, char col)
{
   col -= 1;

   switch(row)
   {
      case 1: set_col(col);      break; 
      case 2: set_col(col+0x40); break;
    
   } //end switch
  
} //end char_pos


// =====================================================================================
// --- Função para Envio de Caracteres ---


void send_char(char character)
{

   send_byte(character); 
   PORTB |= rs;
   delayMicroseconds(200);
   pulse_enable();
   PORTB &= ~rs;
  
} //end send_char_pos


void send_char_pos(char character, char row, char col)
{

  char_pos(row, col);
  send_char(character); 
  
} //end send_char_pos


// =====================================================================================
// --- Função para Inicializar LCD ---
void lcd_init()
{

   //configura saídas
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

   //inicializa saídas
   PORTB &= ~en;
   PORTB &= ~rs;
   send_byte(0x00);
   delayMicroseconds(200);
   
   // LIMPA LCD
   send_byte(0x01);
   pulse_enable();

   // MODO DE 8 BITS
   send_byte(0x38);
   pulse_enable();

   // LIGA LCD, DESLIGA CURSOR, DESLIGA BLINK
   send_byte(0x0C); //desliga cursor
   pulse_enable();

   // HABILITA INCREMENTO, DESLIGA SCROLL
   send_byte(0x06);
   pulse_enable();
  
} //end lcd_init


void lcd_clear()
{

   // LIMPA LCD
   PORTB &= ~rs;
   send_byte(0x01);
   pulse_enable();

} //end lcd_clear


void send_byte(char s_byte)
{
  PORTB &= ~(db0 | db1 | db2 | db3);
  PORTD &= ~(db4| db5 | db6 | db7);

  PORTB |= s_byte & (db3 | db2 | db1 | db0);
  PORTD |= s_byte & (db4 | db5 | db6 | db7);
} //end send_byte


void pulse_enable()
{
   PORTB &= ~en;
   delayMicroseconds(800);
   PORTB |= en;
   delayMicroseconds(200);
   PORTB &= ~en;
   delayMicroseconds(800);  
  
} //end pulse_enable


// =====================================================================================
// --- Final do Programa ---