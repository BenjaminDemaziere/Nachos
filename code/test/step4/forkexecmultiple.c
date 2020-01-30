#include "syscall.h"
//Création de nombreux processus avec plusieurs threads chacuns

int main(){
  PutString("Les affichages sont de la forme \"xxy\" avec x le numéro du processus réalisant l'affichage et y le numéro du thread à l'intérieur de ce processus\n");
  PutString("\nCréation du processus n° 1. \n");
  ForkExec("./userpages0");
  PutString("\nCréation du processus n° 2. \n");
  ForkExec("./userpages1");
  PutString("\nCréation du processus n° 3. \n");
  ForkExec("./userpages2");
  PutString("\nCréation du processus n° 4. \n");
  ForkExec("./userpages3");
  PutString("\nCréation du processus n° 5. \n");
  ForkExec("./userpages4");
  PutString("\nCréation du processus n° 6. \n");
  ForkExec("./userpages5");
  PutString("\nCréation du processus n° 7. \n");
  ForkExec("./userpages6");
  PutString("\nCréation du processus n° 8. \n");
  ForkExec("./userpages7");
  PutString("\nCréation du processus n° 9. \n");
  ForkExec("./userpages8");
  PutString("\nCréation du processus n° 10. \n");
  ForkExec("./userpages9");

  return 0;
}
