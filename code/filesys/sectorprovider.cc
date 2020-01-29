
#include "sectorprovider.h"

#include <strings.h>		/* for bzero */
//Constructeur du SectorProvider
SectorProvider::SectorProvider (){
  //bzero (machine->mainMemory, MemorySize);
  sectorMap = new BitMap(NumSectors);
}

SectorProvider::~SectorProvider(){
  delete sectorMap;
}

//Permet de libérer le secteur d'index "index";
void SectorProvider::ReleaseSector(int index){
  sectorMap->Clear(index);
}

//Retourne le numéro du premier secteur disponible (libre)
//et l'alloue
int SectorProvider::GetEmptySector(){
  int i = sectorMap->Find();
  //bzero(machine->mainMemory+(i*PageSize),PageSize);
  return i;
}

//Retourne le nombre de secteurs libre dans la mémoire
int SectorProvider::NumAvailSector(){
  return sectorMap->NumClear ();
}
