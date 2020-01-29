#include "syscall.h"
//Test de la limite de 10 fichiers ouverts
int main(){

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
  PutString("Tentative de réouverture de fichiers\n");
  int fdAgain = UserOpenFile("testFile3");
  PutInt(fdAgain);
  if (fdAgain == -1){
    PutString("Ouverture impossible, normal\n");
  }else {
    PutString("Ouverture effectuée, anormal\n");
  }

  int fdAgain2 = UserOpenFile("testFile4");
  PutInt(fdAgain2);
  if (fdAgain2 == -1){
    PutString("Ouverture impossible, normal\n");
  }else {
    PutString("Ouverture effectuée, anormal\n");
  }

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

  PutString("=============================================================\n");
  PutString("Création de fichiers en trop \n");
  UserMkFile("fileEnTrop",10);
  int fdEnTrop = UserOpenFile("fileEnTrop");
  PutInt(fdEnTrop);
  if (fdEnTrop == -1){
    PutString("Ouverture impossible, normal\n");
  }else {
    PutString("Ouverture effectuée, échec du test\n");
  }
  UserMkFile("fileEnTrop2",10);
  int fdEnTrop2 = UserOpenFile("fileEnTrop2");
  PutInt(fdEnTrop2);
  if (fdEnTrop2 == -1){
    PutString("Ouverture impossible, normal\n");
  }else {
    PutString("Ouverture effectuée, échec du test\n");
  }
  PutString("=============================================================\n");
  PutString("Fermeture des fichiers\n");
  UserCloseFile(fd0);
  PutString("1\n");
  UserCloseFile(fd1);
  PutString("2\n");

  UserCloseFile(fd2);
  PutString("3\n");

  UserCloseFile(fd3);
  PutString("4\n");

  UserCloseFile(fd4);
  PutString("5\n");

  UserCloseFile(fd5);
  PutString("6\n");

  UserCloseFile(fd6);
  PutString("7\n");

  UserCloseFile(fd7);
  PutString("8\n");

  UserCloseFile(fd8);
  PutString("9\n");

  UserCloseFile(fd9);
  PutString("10\n");


}
