
#include "copyright.h"

#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include "network.h"
#include "synchlist.h"
#include "post.h"




#define TEMPO 10000 //Temps de réémission en temps horloge
#define MAXREEMISSIONS 1000 //Nombre de réémission max d'un paquet en tcp



//Renvoie un port(mailbox) non utilisé
//-1 si aucun disponible
int getFreePort();


class SocketClientTCP  {
    public:
        SocketClientTCP();
        ~SocketClientTCP();

        //Ouvre la connexion vers le serveur d'adresse addrServer
        //Renvoie 1 si la connexion s'est ouverte
        //0 si la connexion a echoué
        int Connect(NetworkAddress addrServer, MailBoxAddress port);


        //Envoie data qui a une taille size au destinataire de la socket
        //renvoie 0 si l'envoie a echoué
        int Write(const char *data, int size);

        //Méthode interne
        void Write1Packet(const char *data, int size);

        //Met les données reçue dans data avec une taille max de size
        //Renvoie le nombre d'octets lus, 0 si la connection a été fermée
        int Read(char *data, int size);

        //Ferme la connexion
        void Close();

        //Fonction interne qui récupère les paquets depuis le post office
        void GetPacket();


        Semaphore * semSend; //Sémaphore d'attente d'envoie de données 
        Semaphore * semAck; //Sémaphore d'attente de reception d'un acquittement
        Semaphore * semFinAck;
        Lock *lock;

        PacketHeader  packetHeader;
        MailHeader mailHeader; 

        Thread * threadGetPacket;

        SynchList *messages; //Liste des messages de données

        int numPacket; //Le numéro du prochain paquet data a envoyé
        int numPacketReceived; //Le numéro du prochain paquet data qu'on doit recevoir

        int nbrResend; //Nombre de fois qu'on a essayé d'envoyer le paquet
        int synAckReceived; //Si on a reçu un synAck
        int portServer;

        bool connectionClosed; //Si la connexion a été fermée par la socket distante
        bool errorSend; //Si on a atteint max réémission
};

class SocketServerTCP  {
    public:
        SocketServerTCP(int portS);
        ~SocketServerTCP();

        //Attend qu'une demande de connexion soit faite sur le port
        //Retourne une socket client
        SocketClientTCP * Accept();

        //Ferme la connexion!
        void Close();

        //Méthode interne
        void GetPacket();

        int port;

        Thread * threadGetPacket;
        Semaphore * semAck;
        Semaphore * semSyn;
        MailHeader mailHeader;
        PacketHeader packetHeader;
};









#endif
