
#include "copyright.h"

#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include "network.h"
#include "synchlist.h"
#include "post.h"




#define TEMPO 10000 //Temps de réémission en temps horloge
#define MAXREEMISSIONS 1000 //Nombre de réémission max d'un paquet en tcp



//Renvoie un port(mailbox) non utilisé
int getFreePort();


class SocketClientTCP  {
    public:
        SocketClientTCP();
        ~SocketClientTCP();

        //Ouvre la connexion vers le serveur d'adresse addrServer
        void Connect(NetworkAddress addrServer, MailBoxAddress port);


        //Envoie data qui a une taille size au destinataire de la socket
        void Write(const char *data, int size);

        void Write1Packet(const char *data, int size);

        //Met les données reçue dans data avec une taille max de size
        //Renvoie le nombre d'octets lus
        int Read(char *data, int size);

        //Ferme la connexion
        void Close();

        //Fonction interne à ne pas appeler
        void GetPacket();


        Semaphore * semSend; //Sémaphore d'attente d'envoie de données 
        Semaphore * semAck; //Sémaphore d'attente de reception d'un acquittement
        Lock *lock;

        PacketHeader  packetHeader;
        MailHeader mailHeader; 

        Thread * threadGetPacket;

        SynchList *messages; //Liste des messages de données

        int numPacket; //Le numéro du prochain paquet data a envoyé
        int numPacketReceived; //Le numéro du prochain paquet data qu'on doit recevoir

        int nbrResend; //Nombre de fois qu'on a essayé d'envoyer le paquet
        int synAckReceived;
        int portServer;
};

class SocketServerTCP  {
    public:
        SocketServerTCP();
        ~SocketServerTCP();

        //Attend qu'une demande de connection soit faite sur le port
        //Retourne une socket client
        SocketClientTCP * Accept(int port);

        void GetPacket();

        int port;

        Thread * threadGetPacket;
        Semaphore * semAck;
        Semaphore * semSyn;
        MailHeader mailHeader;
        PacketHeader packetHeader;
};









#endif
