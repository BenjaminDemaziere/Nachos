#include "copyright.h"
#include "system.h"

#ifndef SECTORPROVIDER_H
#define SECTORPROVIDER_H
class SectorProvider //Objet contenant la bitmap des secteurs libres et gérant
                    // l'allocation des secteurs.
{
  public :

    SectorProvider();//Constructeur du FrameProvider

    ~SectorProvider();

    void ReleaseSector(int index); //Permet de libérer le cadre d'index "index";

    int GetEmptySector(); //Retourne le numéro du premier cadre disponible (libre)
                         //et l'alloue

    int NumAvailSector(); //Retourne le nombre de cadre libre dans la mémoire

  private:
    BitMap  * sectorMap;
};

#endif // FRAMEPROVIDER_H
