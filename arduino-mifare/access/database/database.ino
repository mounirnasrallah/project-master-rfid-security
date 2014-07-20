/**************************************************************************/
/*
  Implémentation d'un controle d'accès basé sur une MiFare Classic

  NASR ALLAH Mounir    
  18 Avril 2014 
  nasrallah.mounir@gmail.com
  http://mounirnasrallah.com
*/
/**************************************************************************/

#include <HashMap.h>



const byte HASH_SIZE = 4; 


HashType<int,char*> hashRawArray[HASH_SIZE];
HashMap<int,char*> hashMap = HashMap<char*,int>( hashRawArray , HASH_SIZE );

 
void setupDB(){
 
  hashMap[0](3435,"Agathe Theblouse");
  hashMap[1](78643,"Mounir Nasr Allah");
  hashMap[2](52342,"Guénael Renault");
  hashMap[3](9872,"Sarah croche");
 
  hashMap.debug();
}

char* get_name(uint8* id){

}
 
