
#include "socketTCP.h"
#include "system.h"
#include <exception>


struct portAlreadyUsedExcpetion : public std::exception {
   const char * what () const throw () {
      return "port already used";
   }
};


int getFreePort() {
    for(int i=0;i<NumberBoxMail;i++) {
        if(!portUsed->Test(i)) {
            return i;
        }
    }
    return -1;
}

//Struct utilisé pour la réémission de paquet
typedef struct DataPacket{
    const char * data;
    int size;
    TypePacket type;
    int numPacket;
}DataPacket;


//Fonction pour récupérer les données depuis le postOffice
//Reçoit un tableau: 1ère case DataPacket
//2ème case la socket
void ReSendPacket(int arg) {

    void ** tabarg = (void**) arg;
    SocketClientTCP * sock = (SocketClientTCP *) (tabarg[1]);
    DEBUG('r',"Resent Packet\n");

    sock->lock->Acquire();

    DataPacket  * d = (DataPacket *) (tabarg[0]);
    sock->nbrResend ++;
    sock->mailHeader.length = d->size;
    sock->mailHeader.type = d->type;
    sock->mailHeader.numPacket = d->numPacket;
    sock->packetHeader.length = sizeof(MailHeader) + d->size;

    postOffice->Send(sock->packetHeader, sock->mailHeader, d->data);
    if(sock->nbrResend<MAXREEMISSIONS) {
        interrupt->Schedule(ReSendPacket, arg, TEMPO ,TimerInt); //Relance le timer
    }
    else {
        sock->errorSend = true;
        DEBUG('r',"Stop sending a packet, too much resend\n");
        sock->semAck->V(); //Relache le verrou
    }
    sock->lock->Release();
}

//Fonction pour récupérer les données depuis le postOffice
void SocketGetPacket(int socket) {
    SocketClientTCP * s = (SocketClientTCP *) socket;
    s->GetPacket();
}


SocketClientTCP::SocketClientTCP() {
    semSend = new Semaphore("Socket sem send",1);
    semAck = new Semaphore("Socket sem Ack",0);
    semFinAck = new Semaphore("Socket sem FinAck",0);
    lock = new Lock("Socket lock");

    connectionClosed = false;
    errorSend = false;

    messages = new SynchList;


    //Port et adresse source
    mailHeader.from = getFreePort();
    portUsed->Mark(mailHeader.from);
    packetHeader.from = postOffice->netAddr;


    threadGetPacket = new Thread("threadGetPacket");
    threadGetPacket->Fork(SocketGetPacket,(int)this);


}

SocketClientTCP::~SocketClientTCP() {
    portUsed->Clear(mailHeader.from);//Port plus utilisé
    delete semSend;
    delete semAck;
    delete semFinAck;
    delete messages; 
}



int SocketClientTCP::Connect(NetworkAddress addrServer, int port) {
    DEBUG('r',"Call Connect\n");

    portServer = port;
    //Définit les ports
    mailHeader.to = port;

    //Définit les adresses
    packetHeader.to = addrServer;


    mailHeader.length = 0;
    packetHeader.length = sizeof(mailHeader);

    //Demande de connection
    mailHeader.type = SYN;
    postOffice->Send(packetHeader, mailHeader, NULL);
    //Attend l'accusé de réception
    DataPacket * d = new DataPacket;
    d->size = 0;
    d->data = 0;
    d->type = SYN;
    void **arg = new void*[2];
    arg[0] = (void*)d;
    arg[1] = (void*)this;
    //Lance le timer de renvoie du packet

    interrupt->Schedule(ReSendPacket, int(arg), TEMPO ,TimerInt);
    semAck->P(); //Attend l'accusé de réception (SYNACK) du serveur
    //Modification implicite de mailHeader.to vers la nouvelle socket du serveur
    //On a reçu l'acquittement du packet
    //On détruit l'interruption 

    interrupt->RemoveScheduledInterrupt(ReSendPacket, int(arg), TEMPO ,TimerInt);
    delete arg;delete d;
    if(errorSend==true) {
        DEBUG('r',"Connect failure\n");
        return 0;
    }

    //Envoie l'acceptation de la connection
    // dans le getPacket car on peut recevoir plusieurs fois SYNACK
    currentThread->Wait(100*TEMPO);
    DEBUG('r',"Connect success\n");
    return 1;
}





//Envoie data qui a une taille size au destinataire de la socket
int SocketClientTCP::Write(const char *data, int size) {
    lock->Acquire();
    DEBUG('r',"Call Write\n");
    errorSend = false;



    int sizePacket = size > (int)MaxMailSize ? (int)MaxMailSize : size;
    int sizePacketTotal = 0;
    while(sizePacketTotal<size) {
        semSend->P(); //Il ne peut y avoir qu'un envoie de donnée par la socket

        Write1Packet(data+sizePacketTotal,sizePacket);
        sizePacketTotal += sizePacket;
        sizePacket = size-sizePacketTotal > (int)MaxMailSize ? (int)MaxMailSize : size-sizePacketTotal;
        semSend->V();
        if(errorSend) {
            size = 0;
        }

    }

    lock->Release();
    DEBUG('r',"End Write\n");

    return size;
}

//Envoie un paquet un packet
void SocketClientTCP::Write1Packet(const char *data, int size) {
    mailHeader.length = size;
    mailHeader.type = DATA;
    mailHeader.numPacket = numPacket;

    packetHeader.length = sizeof(mailHeader) + size;
    postOffice->Send(packetHeader, mailHeader, data);

    DataPacket * d = new DataPacket;
    d->size = size;
    d->data = data;
    d->type = DATA;
    d->numPacket = numPacket;

    void **arg = new void*[2];
    arg[0] = (void*)d;
    arg[1] = (void*)this;
    //Lance le timer de renvoie du packet
    lock->Release();
    interrupt->Schedule(ReSendPacket, int(arg), TEMPO ,TimerInt);
    semAck->P(); //Attend l'accusé de réception
    lock->Acquire();


    //On a reçu l'acquittement du packet
    //On détruit l'interruption 
    interrupt->RemoveScheduledInterrupt(ReSendPacket, int(arg), TEMPO ,TimerInt);
    delete arg;delete d;

    nbrResend=0; //Nombre de renvoie de paquets
    numPacket++;
}


//Met les données reçues dans data avec une taille max de size
int SocketClientTCP::Read(char *data, int size) {
    DEBUG('r',"Call Read\n");

    //Peut lire des données si la connection n'est pas fermée ou s'il reste encore des données à lire
    if(connectionClosed==false || !messages->IsEmpty()) {
        DataPacket * d = (DataPacket *)messages->Remove(); //Récupère les données
        if (d==NULL && connectionClosed==true) { //On a forcé le retour à cause du close
            delete d;
            DEBUG('r',"Read: force close\n");
            return 0;
        }
        int s = size > d->size ? d->size : size;
        bcopy(d->data,data,s);//Copie les données

        delete d->data;
        delete d;
        DEBUG('r',"End Read\n");

        return s;
    }
    else { //Connection fermée
        return 0;
    }
}

//Ferme la connexion
void SocketClientTCP::Close() {
    lock->Acquire();
    DEBUG('r',"Socket: close\n");
    if(connectionClosed==false) {
        mailHeader.length = 0;
        mailHeader.type = FIN;
        mailHeader.numPacket = numPacket;

        packetHeader.length = sizeof(mailHeader);
        postOffice->Send(packetHeader, mailHeader, NULL);

        DataPacket * d = new DataPacket;
        d->size = 0;
        d->data = NULL;
        d->type = FIN;
        d->numPacket = numPacket;

        void **arg = new void*[2];
        arg[0] = (void*)d;
        arg[1] = (void*)this;
        //Lance le timer de renvoie du packet
        lock->Release();
        interrupt->Schedule(ReSendPacket, int(arg), TEMPO ,TimerInt);
        semFinAck->P(); //Attend l'accusé de réception (FINACK)
        lock->Acquire();

        lock->Release();
        DEBUG('r',"Socket: close finish\n");
    }
    else {
        DEBUG('r',"Socket: close finish, the connection was already closed by the other socket\n");
    }

    delete this;
}



//Méthode interne qui récupère les paquets depuis le postOffice et met les messages de données dans la liste
//Elle s'occupe aussi d'alerter write qu'un acquittement a eu lieu
void SocketClientTCP::GetPacket() {
    PacketHeader pktHdr;
    MailHeader mailHdr;
    char dummy;      

    while(1) {

        char * data = new char[MaxMailSize];

        postOffice->Receive(mailHeader.from,&pktHdr,&mailHdr,data); //Récupère les données

        if(mailHdr.type==ACK) {
            semAck->V(); //On a reçu un acquittement, on peut renvoyer un nouveau packet
        }
        else if(mailHdr.type==SYNACK) {
            lock->Acquire();

            //Le serveur nous donne le port de la socket qu'il vient d'ouvrir
            //Il faut donc mettre le port vers lequel on envoie les données

            if(synAckReceived==0) {//C'est le premier SYNACK qu'on reçoit
                semAck->V();
                synAckReceived=1;
            }
            mailHeader.type = ACK;
            mailHeader.to = portServer;

            postOffice->Send(packetHeader, mailHeader, &dummy);
            mailHeader.to = mailHdr.from; //Le port de la socket client du serveur

            lock->Release();
        }
        else if(mailHdr.type==FINACK) {
            lock->Acquire();
            //Reçoit aquittement de fin de connection
            semFinAck->V();
            lock->Release();
            //La connexion est finie, on libère le thread
            currentThread->Finish();
        }
        //Reçoit une fin de connection de la part de l'autre socket
        else if(mailHdr.type==FIN) {
            lock->Acquire();
            DEBUG('r',"Receive FIN\n");
            //Envoie un acquittement de fin de connection
            mailHdr.length = 0;
            mailHdr.type = FINACK;
            mailHdr.from = mailHeader.from;
            mailHdr.to = mailHeader.to;
            pktHdr.to = packetHeader.to;
            pktHdr.from = packetHeader.from;
            packetHeader.length = sizeof(mailHdr);
            postOffice->Send(pktHdr, mailHdr, &dummy);

            //La connection est fermée 
            connectionClosed = true;
            messages->StopRemove(); //Débloque l'appel sur le Read

            lock->Release();
        }
        else if(mailHdr.type==DATA) { //Met les données et la taille dans la liste de message

            if(mailHdr.numPacket == numPacketReceived) { 

                DataPacket * d = new DataPacket();
                d->data = data;
                d->size = mailHdr.length;
                messages->Append(d);
                numPacketReceived++;
                DEBUG('r',"Paquet reçu\n");

            }
            else {
                DEBUG('r',"Paquet doublon reçu et jeté\n");
            }
            //Envoie d'un acquittement
            //On n'utilise des variables locales pour envoyer l'acquittement
            //Cela évite de prendre le verrou
            mailHdr.length = 0;
            mailHdr.type = ACK;
            mailHdr.from = mailHeader.from;
            mailHdr.to = mailHeader.to;
            pktHdr.to = packetHeader.to;
            pktHdr.from = packetHeader.from;
            packetHeader.length = sizeof(mailHdr);
            postOffice->Send(pktHdr, mailHdr, &dummy);
        }

        else { //Ne devrait pas arriver
            DEBUG('r',"Paquet inconnu reçu\n");
        }
    }
}


/*
    Socket serveur
*/
//Fonction pour récupérer les données depuis le postOffice pour le serveur
void SocketServerGetPacket(int socket) {
    SocketServerTCP * s = (SocketServerTCP *) socket;
    s->GetPacket();
}
void ReSendPacketServer(int arg) {
    DEBUG('r',"Resent Packet server \n");
    void ** tabarg = (void**) arg;
    SocketServerTCP * sock = (SocketServerTCP *) (tabarg[1]);
    DataPacket  * d = (DataPacket *) (tabarg[0]);
    sock->mailHeader.length = d->size;
    sock->mailHeader.type = d->type;
    sock->packetHeader.length = sizeof(MailHeader) + d->size;
    postOffice->Send(sock->packetHeader, sock->mailHeader, d->data);
    interrupt->Schedule(ReSendPacketServer, arg, TEMPO ,TimerInt); //Relance le timer
}

SocketServerTCP::SocketServerTCP(int portS) {
    semAck = new Semaphore("Socket sem Ack",0);
    semSyn = new Semaphore("Socket sem Syn",0);
    threadGetPacket=NULL;
    //Regarde si le port est valide
    port = portS;
    if(port >= NumberBoxMail || portUsed->Test(port)) {
        throw portAlreadyUsedExcpetion();
    }
    portUsed->Mark(port); //port utilisé
}

SocketServerTCP::~SocketServerTCP() {
    portUsed->Clear(mailHeader.from);//Port plus utilisé
    delete semAck;
    delete semSyn;
}

void SocketServerTCP::Close() {
    DEBUG('r',"Call close server\n");

    delete this;
}

SocketClientTCP * SocketServerTCP::Accept() {
    DEBUG('r',"Call Accept");

    //Port et adresse du serveur
    mailHeader.from = port;
    packetHeader.from = postOffice->netAddr;

    mailHeader.length = 0;
    packetHeader.length = sizeof(mailHeader);

    //Thread pour récupérer les paquets
    if(threadGetPacket==NULL) {
        threadGetPacket = new Thread("thread server socket");
        threadGetPacket->Fork(SocketServerGetPacket,(int)this);
    }

    /*
        Attente de demande de connection du client (SYN)
    */
    semSyn->P();

    //On crée la socket
    SocketClientTCP * socketClient = new SocketClientTCP;
    //On configure la socket avec les bons port et adresse
    //Le port local est déjà configuré à la création de la socket
    socketClient->mailHeader.to = mailHeader.to;
    socketClient->packetHeader.from = packetHeader.from;
    socketClient->packetHeader.to = packetHeader.to;

    /*
        Envoie SynAck
    */
    mailHeader.type = SYNACK;

    //On envoie au client le port qu'il devra utiliser pour communiquer avec la socket du serveur
    mailHeader.from = socketClient->mailHeader.from;

    DataPacket * d = new DataPacket;
    d->size = 0;
    d->data = 0;
    d->type = SYNACK;

    void **arg = new void*[2];
    arg[0] = (void*)d;
    arg[1] = (void*)this;
    //Lance le timer de renvoie du packet
    postOffice->Send(packetHeader,mailHeader,NULL);
    interrupt->Schedule(ReSendPacketServer, int(arg), TEMPO ,TimerInt);


    semAck->P(); //Attend l'accusé de réception (Ack) du client

    interrupt->RemoveScheduledInterrupt(ReSendPacketServer, int(arg), TEMPO ,TimerInt);
    delete arg;delete d;

    mailHeader.from = port; //remet le bon port





    DEBUG('r',"Accept Success");

    return socketClient;
    //La connection est ouverte
}

//Méthode interne qui récupère les paquets depuis le postOffice
void SocketServerTCP::GetPacket() {
    while(1) {
        

        PacketHeader pktHdr;
        MailHeader mailHdr;
        char data[MaxMailSize];

        postOffice->Receive(port,&pktHdr,&mailHdr,data); //Récupère les données

        if(mailHdr.type==ACK) {
            DEBUG('r',"Server receive ACK\n");

            semAck->V(); //On a reçu un acquittement, on peut renvoyer un nouveau packet
        }
        else if(mailHdr.type==SYN) {
            DEBUG('r',"Server receive SYN\n");

            //port et adresse du client
            mailHeader.to = mailHdr.from;
            packetHeader.to = pktHdr.from;
            semSyn->V();
        }
        else { //Ne devrait pas arriver
            DEBUG('r',"Server, Unknow packet\n");
        }
    }
}


int CompareRemoveInterrput(void* element,void* elementList) {
    PendingInterrupt *inter = (PendingInterrupt *) element;
    PendingInterrupt *interList = (PendingInterrupt *) elementList;
    return inter->handler==interList->handler && inter->arg==interList->arg &&
    inter->type==interList->type ;
    //Peut pas comparer fromNow
}

//Enlève une interruption
void Interrupt::RemoveScheduledInterrupt(VoidFunctionPtr handler, int arg, long long fromNow, IntType type) {
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    PendingInterrupt *toOccur = new PendingInterrupt(handler, arg, fromNow, type);
    pending->RemoveElement(toOccur, CompareRemoveInterrput);
    (void) interrupt->SetLevel (oldLevel);
}