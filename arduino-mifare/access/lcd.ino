/**************************************************************************/
/*
  Implémentation d'un controle d'accès basé sur une MiFare Classic

  NASR ALLAH Mounir    
  18 Avril 2014 
  nasrallah.mounir@gmail.com
  http://mounirnasrallah.com
*/
/**************************************************************************/

#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);



// Initialisation de l'écran LCD
void setup_lcd() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Initialisation...");
  delay(1000);
}



// Affichage sur l'écran LCD
void print_lcd_wait_card() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Presentez");
  lcd.setCursor(0, 1);
  lcd.print("   votre badge");
   delay(1000);
}

// Affichage sur l'écran LCD
void print_lcd_access_denied() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Accès refusé !");
}

// Affichage sur l'écran LCD
void print_lcd_access_ok(char* name) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bonjour");
  lcd.setCursor(0, 1);
  lcd.print(name);
}

