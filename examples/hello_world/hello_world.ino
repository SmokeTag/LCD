// =====================================================================================
// --- Libriries ---
#include <LCD8b.h>


void setup() {
  lcd_init();
  lcd_print_slow("Hello World!", 100);
  delay(1200);
} //end setup
 
void loop() {
  lcd_clear();
  delay(1000);
  lcd_print("Hello World!");
  delay(1000);
} //end loop


// =====================================================================================
// --- Final do Programa ---