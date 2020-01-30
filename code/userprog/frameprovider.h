#include "copyright.h"
#include "system.h"

#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H
class FrameProvider //Objet contenant la bitmap de la mémoire et gérant
                    // l'allocation des cadres.
{
  public :

    FrameProvider();//Constructeur du FrameProvider

    ~FrameProvider();

    void ReleaseFrame(int index); //Permet de libérer le cadre d'index "index";

    int GetEmptyFrame(); //Retourne le numéro du premier cadre disponible (libre)
                         //et l'alloue

    int NumAvailFrame(); //Retourne le nombre de cadre libre dans la mémoire

  private:
    BitMap  * frameMap;
};

#endif // FRAMEPROVIDER_H
