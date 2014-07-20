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

// Objets PN532
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

#define NR_SHORTSECTOR          (32)    // Number of short sectors on Mifare 1K/4K
#define NR_LONGSECTOR           (8)     // Number of long sectors on Mifare 4K
#define NR_BLOCK_OF_SHORTSECTOR (4)     // Number of blocks in a short sector
#define NR_BLOCK_OF_LONGSECTOR  (16)    // Number of blocks in a long sector

#define MIFARE_CMD_AUTH_A          0
#define MIFARE_CMD_AUTH_B          1

#define SELECT_SECTOR              3
#define SELECT_BLOCK               2

#define NBR_SECTOR                32    // Nombre de secteurs 
#define NBR_BLOCKS                 4    // Nombre de blocs dans un secteur

// Permet de déterminer le bloc du secteur sélectionné
#define BLOCK_NUMBER_OF_SECTOR_1ST_BLOCK(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR))

#define BLOCK_NUMBER_OF_SECTOR_TRAILER(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR + NR_BLOCK_OF_SHORTSECTOR-1):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR + NR_BLOCK_OF_LONGSECTOR-1))



#define OLD_KEY_A    0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36// 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 

#define NEW_KEY_A    0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36 

#define OLD_KEY_B    0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36 //0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  // 0x01, 0x00, 0x00, 0x00, 0x00, 0x00  // 0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36 //  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // 

#define NEW_KEY_B    0xE4, 0x87, 0x13, 0xC6, 0x27, 0x36 

#define USER_ID      0x02, 0x34, 0x86, 0xC3, 0xAA, 0x42, 0x10, 0x72, 0x94, 0x45, 0x34, 0x64, 0x25, 0x35, 0x77, 0x27 

#define ACCESS_BYTE  0xff, 0x07, 0x80, 0x69 //  0x78, 0x77, 0x88, 0x00 

#define ACCESS_BLOC  NEW_KEY_A, ACCESS_BYTE, NEW_KEY_B



// Inititalisation 
void setup(void) {
 
  // Inititalisation de la liaison serie
  Serial.begin(115200);
  
  // Inititalisation du shield "NFC"
  nfc.begin();
  
  // Test du shield 
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Shield non détecté \n");
    while (1); // On boucle infiniment
  }  
  
  Serial.print("Circuit integre PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.println((versiondata>>16) & 0xFF, DEC); 
  
  Serial.println("Attente d'un transpondeur ISO 14443 Type A...");
  
  // ????????????
  nfc.SAMConfig(); 
  
}



void loop(void) {
  
  uint8_t success;                            
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };    // Buffer qui contiendra l'UID de la carte MiFare
  uint8_t uidLength;                          // Taille de l'UID
  uint8_t buffer[16] = {USER_ID};                  // Buffer contenant les données à écrire  
  uint8_t acces_buffer[16] = {ACCESS_BLOC} ;  
  uint8_t old_key[16] = {OLD_KEY_A};
  
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
 
    Serial.println("Carte détecte");
    Serial.println("  UID de la carte : ");
    nfc.PrintHex(uid, uidLength);
    
    if (uidLength == 4)
    {
      Serial.println("Tentative d'authentification");
      
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, BLOCK_NUMBER_OF_SECTOR_TRAILER(SELECT_SECTOR), MIFARE_CMD_AUTH_A, old_key);
	  
      if (success){
         // Lecture des données du block 4
         success = nfc.mifareclassic_WriteDataBlock(BLOCK_NUMBER_OF_SECTOR_1ST_BLOCK(SELECT_SECTOR), buffer);
        
         if (success){
            // Réussite ?
            Serial.print("Donnees ecrites sur le Bloc ");
            Serial.print(SELECT_BLOCK);
            Serial.print(", secteur ");
            Serial.print(SELECT_SECTOR);
            Serial.print(" : ");
            nfc.PrintHexChar(buffer, 16);
            Serial.println(" ");
             	  
            // On attend avant de réécrire la carte
            delay(1);
         }
         else{
            Serial.println("Echec de l'ecriture");
         }
         
                
         // Ecriture dans le bloc d'accès 
          success = nfc.mifareclassic_WriteDataBlock(BLOCK_NUMBER_OF_SECTOR_TRAILER(SELECT_SECTOR), acces_buffer);
          if(success){
           Serial.print("Modification du bloc d'acces du bloc ");   
           Serial.print(BLOCK_NUMBER_OF_SECTOR_TRAILER(SELECT_SECTOR));    
           Serial.println("........ [OK] "); 
          }
          else{
            Serial.print("Impossible de modifier le bloc d'acces du secteur "); 
            Serial.println(SELECT_SECTOR);
            return;
          }
      }
      else
      {
        Serial.println("Echec de l'authentification ");
      }
      
   }
  }
}

