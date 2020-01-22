#include "syscall.h"
//Test de la limite de 10 fichiers ouverts
int main(){
  PutString("=============================================================\n");
  PutString("Affichage du contenu de la racine\n");
  UserListdir();

  PutString("=============================================================\n");
  PutString("Création et ouverture de 5 fichiers dans le répertoire racine\n");

  UserMkFile("testFile0",10);
  UserMkFile("testFile1",10);
  UserMkFile("testFile2",10);
  UserMkFile("testFile3",10);
  UserMkFile("testFile4",10);

  int fd0 = UserOpenFile("testFile0");
  PutInt(fd0);
  if (fd0 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd1 = UserOpenFile("testFile1");
  PutInt(fd1);
  if (fd1 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd2 = UserOpenFile("testFile2");
  PutInt(fd2);
  if (fd2 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd3 = UserOpenFile("testFile3");
  PutInt(fd3);
  if (fd3 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd4 = UserOpenFile("testFile4");
  PutInt(fd4);
  if (fd4 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  PutString("Affichage du contenu de la racine\n");
  UserListdir();

  PutString("=============================================================\n");
  PutString("Création d'un répertoire testDir qui contiendra les 5 fichiers suivants\n");
  int ret = UserMkdir("testDir");
  if (ret == 1) PutString("Erreur de création du répertoire\n");
  ret = UserChdir("testDir");
  if (ret == 1) PutString("Erreur de déplacement dans le répertoire\n");

  PutString("Création et ouverture de 5 fichiers dans le répertoire testDir\n");
  UserMkFile("testFile5",10);
  UserMkFile("testFile6",10);
  UserMkFile("testFile7",10);
  UserMkFile("testFile8",10);
  UserMkFile("testFile9",10);
  int fd5 = UserOpenFile("testFile5");
  PutInt(fd5);
  if (fd5 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd6 = UserOpenFile("testFile6");
  PutInt(fd6);
  if (fd6 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd7 = UserOpenFile("testFile7");
  PutInt(fd7);
  if (fd7 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd8 = UserOpenFile("testFile8");
  PutInt(fd8);
  if (fd8 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  int fd9 = UserOpenFile("testFile9");
  PutInt(fd9);
  if (fd9 == -1){
    PutString("Ouverture impossible, anormal\n");
  }else {
    PutString("Ouverture effectuée, normal\n");
  }

  PutString("Affichage du contenu de testDir\n");
  UserListdir();
  PutString("=============================================================\n");
  PutString("Création du fichier en trop \n");
  UserMkFile("fileEnTrop",10);
  int fdEnTrop = UserOpenFile("testFile9");
  if (fdEnTrop == -1){
    PutString("Ouverture impossible\n");
  }else {
    PutString("Ouverture effectuée, échec du test\n");
  }
  PutString("=============================================================\n");

}
