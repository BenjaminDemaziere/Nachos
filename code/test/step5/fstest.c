#include "syscall.h"
//Test des appels systèmes du système de fichier
int main(){
  //Test de Listdir
  PutString("=============================================================\n");
  PutString("Affichage du contenu de la racine\n");
  UserListdir();
  PutString("=============================================================\n");
  //Test de Mkdir
  PutString("Tentative de création d'un répertoire testDir\n");
  int ret = UserMkdir("testDir");
  if (ret == 1) PutString("Erreur de création du répertoire\n");
  PutString("Affichage de la racine après création de testDir\n");
  UserListdir();
  PutString("=============================================================\n");

  //Test de Chdir
  PutString("Tentative de déplacement du répertoire courant dans testDir\n");
  ret = UserChdir("testDir");
  if (ret == 1) PutString("Erreur de déplacement dans le répertoire\n");
  PutString("Affichage du contenu de testDir\n");
  UserListdir();
  PutString("=============================================================\n");

  //Test de Mkfile
  PutString("Tentative de création d'un fichier testFile\n");
  UserMkFile("testFile",100);
  PutString("Affichage de testDir après création\n");
  UserListdir();


  PutString("=============================================================\n");

  //Test de Rmfile
  PutString("Tentative de suppression de testFile\n");
  UserRmFile("testFile");
  PutString("Affichage de testDir après suppression\n");
  UserListdir();

  return 0;
}
