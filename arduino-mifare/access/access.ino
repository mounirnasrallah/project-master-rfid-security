/**************************************************************************/
/*
  Implémentation d'un controle d'accès basé sur une MiFare Classic

  NASR ALLAH Mounir    
  18 Avril 2014 
  nasrallah.mounir@gmail.com
  http://mounirnasrallah.com
*/
/**************************************************************************/

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"



#define NR_SHORTSECTOR          (32)    // Number of short sectors on Mifare 1K/4K
#define NR_LONGSECTOR           (8)     // Number of long sectors on Mifare 4K
#define NR_BLOCK_OF_SHORTSECTOR (4)     // Number of blocks in a short sector
#define NR_BLOCK_OF_LONGSECTOR  (16)    // Number of blocks in a long sector



#define MIFARE_CMD_AUTH_A          0
#define MIFARE_CMD_AUTH_B          1


#define NBR_SECTOR                32    // Nombre de secteurs 
#define NBR_BLOCS                  4    // Nombre de blocs dans un secteur

#define DELAY_TIME  3000


// Permet de déterminer le bloc du secteur sélectionné
#define BLOCK_NUMBER_OF_SECTOR_1ST_BLOCK(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR))

#define BLOCK_NUMBER_OF_SECTOR_TRAILER(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR + NR_BLOCK_OF_SHORTSECTOR-1):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR + NR_BLOCK_OF_LONGSECTOR-1))



// Objets PN532
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

#define SELECT_SECTOR     3
#define SELECT_BLOCK      2

#define KEY_A      0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36



#define GPIO_PORTE      9



// Inititalisation 
void setup(void) {
 
  // Inititalisation de la liaison serie
  Serial.begin(115200);
 
  // Inititalisation de la base de données
  setup_db();
  
  // Inititalisation de l'écran LCD
  setup_lcd();
  
  // Inititalisation du shield "NFC"
  nfc.begin();
  
  // Test du shield 
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Shield non détecté");
    while (1); // On boucle infiniment
  }  
   
  Serial.print("Circuit integre PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.println((versiondata>>16) & 0xFF, DEC); 
  
  Serial.println("Attente d'un transpondeur ISO 14443 Type A...");
  
  // Security Mode Normal
  nfc.SAMConfig(); 
  
 pinMode(GPIO_PORTE, OUTPUT);
 digitalWrite(GPIO_PORTE,LOW);
  
  // Affichage du message d'indication de présentation de la carte
  print_lcd_wait_card();
  
}



void loop(void) {
  
  uint8_t success;                            
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };    // Buffer qui contiendra l'UID de la carte MiFare
  uint8_t uidLength;                          // Taille de l'UID
  
    
  // Affichage du message d'indication de présentation de la carte
  print_lcd_wait_card();
           
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
 
    Serial.println("Carte detecte");
    Serial.print("  UID de la carte : ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      Serial.println("Tentative d'authentification");
      
      // Clef pour l'authentification
      uint8_t keya[6] = { KEY_A };
	  
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, BLOCK_NUMBER_OF_SECTOR_TRAILER(SELECT_SECTOR), MIFARE_CMD_AUTH_A, keya);
	  
      if (success)
      {
        Serial.println("Authentification reussite");
        
        // Buffer des données récupérés
        uint8_t data[16];
		
        // Lecture des données du block 4
        success = nfc.mifareclassic_ReadDataBlock(BLOCK_NUMBER_OF_SECTOR_1ST_BLOCK(SELECT_SECTOR), data);
		
        if (success)
        {
          // Affichage des données lus
          Serial.println("Donnees du bloc :");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
          
          // On vérifie que l'identifiant est bien présent dans la base de donnée
          if(valid_user_id(data[0])){
            
           // On récupère son nom 
           char *name = get_name(data[0]);
  
          // Ouverture porte
          digitalWrite(GPIO_PORTE,HIGH);
           
          // On affiche le message de bienvenue
          print_lcd_access_ok(name);
           
          // On attend avant de relire la carte
          delay(DELAY_TIME);
         
         // Fermeture porte
          digitalWrite(GPIO_PORTE,LOW);
            
          }
          else{
            // On affiche le message de refus
            print_lcd_access_denied();
            delay(DELAY_TIME);
          }    
        }
        else
        {
          Serial.println("Echec de la lecture du bloc de donnees");
        }
      }
      else
      {
        Serial.println("Echec de l'authentification");
      }
    }
  }
}

