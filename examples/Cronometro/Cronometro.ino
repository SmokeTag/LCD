// =====================================================================================
// --- Libriries ---
#include <LCD8b.h>


// =====================================================================================
// --- Mapeamento de Hardware ---
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


// =====================================================================================
// --- Variáveis Globais ---
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


// =====================================================================================
// --- Final do Programa ---