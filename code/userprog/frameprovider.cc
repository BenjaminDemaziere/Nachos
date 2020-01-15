
#include "frameprovider.h"

#include <strings.h>		/* for bzero */
//Constructeur du FrameProvider
FrameProvider::FrameProvider (){
  bzero (machine->mainMemory, MemorySize);
  frameMap = new BitMap(NumPhysPages);
}

//Permet de libérer le cadre d'index "index";
void FrameProvider::ReleaseFrame(int index){
  frameMap->Clear(index);
}

//Retourne le numéro du premier cadre disponible (libre)
//et l'alloue
int FrameProvider::GetEmptyFrame(){
  int i = frameMap->Find();
  bzero(machine->mainMemory+(i*PageSize),PageSize);
  return i;
}

//Retourne le nombre de cadre libre dans la mémoire
int FrameProvider::NumAvailFrame(){
  return frameMap->NumClear ();
}
