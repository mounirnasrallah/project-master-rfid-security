/**************************************************************************/
/*
  Implémentation d'un controle d'accès basé sur une MiFare Classic

  NASR ALLAH Mounir    
  18 Avril 2014 
  nasrallah.mounir@gmail.com
  http://mounirnasrallah.com
*/
/**************************************************************************/


const byte TABLE_SIZE = 5; 


char* table[TABLE_SIZE];


void setup_db(void){
  table[0]="Mounir";
  table[1]="Guenael";
  table[2]="Mehdi";
  table[3]="Bob";
  table[4]="Alice";
 
}

int valid_user_id(uint8_t id){
  if( 0 <= id <= 4){
    return 1;
  }
  return 0;
}


char* get_name(uint8_t id){
  
  return table[id];
  
}
