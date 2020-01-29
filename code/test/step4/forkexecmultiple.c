#include "syscall.h"
//Création de nombreux processus avec plusieurs threads chacuns

main(){
  PutString("Les affichages sont de la forme \"xxy\" avec x le numéro du processus réalisant l'affichage et y le numéro du thread à l'intérieur de ce processus\n");
  PutString("\nCréation du processus n° 1. \n");
  ForkExec("./step4/userpages0");
  PutString("\nCréation du processus n° 2. \n");
  ForkExec("./step4/userpages1");
  PutString("\nCréation du processus n° 3. \n");
  ForkExec("./step4/userpages2");
  PutString("\nCréation du processus n° 4. \n");
  ForkExec("./step4/userpages3");
  PutString("\nCréation du processus n° 5. \n");
  ForkExec("./step4/userpages4");
  PutString("\nCréation du processus n° 6. \n");
  ForkExec("./step4/userpages5");
  PutString("\nCréation du processus n° 7. \n");
  ForkExec("./step4/userpages6");
  PutString("\nCréation du processus n° 8. \n");
  ForkExec("./step4/userpages7");
  PutString("\nCréation du processus n° 9. \n");
  ForkExec("./step4/userpages8");
  PutString("\nCréation du processus n° 10. \n");
  ForkExec("./step4/userpages9");

  return 0;
}
