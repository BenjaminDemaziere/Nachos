
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




//Fonction pour récupérer les données depuis le postOffice
//Reçoit un tableau: 1ère case DataPacket
//2ème case la socket
void ReSendPacket(int arg) {

    void ** tabarg = (void**) arg;
    SocketClientTCP * sock = (SocketClientTCP *) (tabarg[1]);
    // DEBUG('r',"Resent Packet\n");
    if(sock->connectionClosed==false) {
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
    postOffice->StopReceive(mailHeader.from); //Sert à arréter le thread threadGetPacket
    // currentThread->Yield();
}



int SocketClientTCP::Connect(NetworkAddress addrServer, int port) {
    DEBUG('r',"Call Connect\n");

    lock->Acquire();

    portServer = port;
    //Définit les ports
    mailHeader.to = port;

    //Définit les adresses
    packetHeader.to = addrServer;


    mailHeader.length = 0;
    packetHeader.length = sizeof(mailHeader);


    //Attend l'accusé de réception
    DataPacket * d = new DataPacket;
    d->size = 0;
    d->data = 0;
    d->type = SYN;

    SendPacket(d);
    //Attend un synAck


    if(errorSend==true) {
        DEBUG('r',"Connect failure\n");
        return 0;
    }
    lock->Release();

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
    DataPacket * d = new DataPacket;
    d->size = size;
    d->data = data;
    d->type = DATA;
    d->numPacket = numPacket;

    SendPacket(d);

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
    if(connectionClosed==false) {
        DEBUG('r',"Socket: close\n");

        DataPacket * d = new DataPacket;
        d->size = 0;
        d->data = NULL;
        d->type = FIN;
        d->numPacket = numPacket;

        //Envoye un paquet FIN
        SendPacket(d);

        DEBUG('r',"Socket: close finish\n");
    }
    else {
        DEBUG('r',"Socket: close finish, the connection was already closed by the other socket\n");
    }
    lock->Release();

    delete this;
}

//Fonctionne interne d'envoie de paquet
void SocketClientTCP::SendPacket(DataPacket * dataPacket) {
    errorSend = false;

    mailHeader.length = dataPacket->size;
    mailHeader.type = dataPacket->type;
    mailHeader.numPacket = dataPacket->numPacket;

    packetHeader.length = sizeof(mailHeader);
    postOffice->Send(packetHeader, mailHeader, dataPacket->data);

    void **arg = new void*[2];
    arg[0] = (void*)dataPacket;
    arg[1] = (void*)this;
    nbrResend=0; //Nombre de renvoie de paquets

    //Lance le timer de renvoie du packet
    lock->Release();
    interrupt->Schedule(ReSendPacket, int(arg), TEMPO ,TimerInt);

    if (dataPacket->type == FIN) {
        semFinAck->P();
    }
    else {
        semAck->P();
    }

    lock->Acquire();
    interrupt->RemoveScheduledInterrupt(ReSendPacket, int(arg), TEMPO ,TimerInt);
    numPacket++;

    delete arg;delete dataPacket;
}

void HelpClose(int arg) {
    SocketClientTCP * sock = (SocketClientTCP *)arg;
    DEBUG('r',"HelpClose\n");
    sock->connectionClosed = true;
    sock->messages->StopRemove();

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
        if (postOffice->HasStopReceive(mailHeader.from)) { //Si on a quitté le receive on termine le thread
            DEBUG('r',"GetPacket: the thread finishes\n");
            currentThread->Finish();
        }

        if(mailHdr.type==ACK) {
            semAck->V(); //On a reçu un acquittement, on peut renvoyer un nouveau packet
        }
        else if(mailHdr.type==SYNACK) {
            lock->Acquire();
            DEBUG('r',"Receive SYNACK\n");

            if(mailHdr.numPacket == numPacketReceived) {  //Si c'est le paquet à recevoir
                //Le serveur nous donne le port de la socket qu'il vient d'ouvrir
                //Il faut donc mettre le port vers lequel on envoie les données
                semAck->V();
                numPacketReceived++;
            }
            else {
                DEBUG('r',"Paquet doublon FIN\n");
            }
            mailHeader.type = ACK;
            mailHeader.to = portServer;

            postOffice->Send(packetHeader, mailHeader, &dummy);
            mailHeader.to = mailHdr.from; //Le port de la socket client du serveur
            lock->Release();
        }
        else if(mailHdr.type==FINACK) {
            DEBUG('r',"Receive FINACK\n");

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
            if(mailHdr.numPacket == numPacketReceived) {  //Si c'est le paquet à recevoir
                //Dès qu'on est sûr de ne plus recevoir 
                interrupt->Schedule(HelpClose,(int)this,TEMPO*4,TimerInt);
                numPacketReceived++;
            }
            else {
                interrupt->RemoveScheduledInterrupt(HelpClose, (int)this, TEMPO*4 ,TimerInt);
                interrupt->Schedule(HelpClose,(int)this,TEMPO*4,TimerInt);
            }
            //Envoie un acquittement de fin de connection
            mailHdr.length = 0;
            mailHdr.type = FINACK;
            mailHdr.from = mailHeader.from;
            mailHdr.to = mailHeader.to;
            pktHdr.to = packetHeader.to;
            pktHdr.from = packetHeader.from;
            packetHeader.length = sizeof(mailHdr);
            postOffice->Send(pktHdr, mailHdr, &dummy);
            lock->Release();
        }
        else if(mailHdr.type==DATA) { //Met les données et la taille dans la liste de message
            if(mailHdr.numPacket == numPacketReceived) {  //Si c'est le paquet à recevoir
                DataPacket * d = new DataPacket();
                d->data = data;
                d->size = mailHdr.length;
                messages->Append(d);
                numPacketReceived++;
                DEBUG('r',"Paquet data reçu\n");
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
    semAck = new Semaphore("SocketServer sem Ack",0);
    semSyn = new Semaphore("SocketServer sem Syn",0);
    inAccept = false;

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
    postOffice->StopReceive(port); //Sert à arréter le thread threadGetPacket
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
    threadGetPacket = new Thread("thread server socket");
    threadGetPacket->Fork(SocketServerGetPacket,(int)this);

    /*
        Attente de demande de connection du client (SYN)
    */
    semSyn->P();
    inAccept=true;

    

    //On crée la socket
    SocketClientTCP * socketClient = new SocketClientTCP;
    socketClient->numPacketReceived = 1; //On doit recevoir le packet 1
    socketClient->numPacket = 1;

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
    inAccept=false;

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
        if (postOffice->HasStopReceive(port)) { //Si on a quitté le receive
            DEBUG('r',"GetPacket: the thread finishes\n");
            currentThread->Finish();
        }

        if(mailHdr.type==ACK) {
            DEBUG('r',"Server receive ACK\n");

            semAck->V(); //On a reçu un acquittement, on peut renvoyer un nouveau packet
        }
        else if(mailHdr.type==SYN) {
            DEBUG('r',"Server receive SYN\n");
            if(inAccept==false) {
                //port et adresse du client
                mailHeader.to = mailHdr.from;
                packetHeader.to = pktHdr.from;
                semSyn->V();
            }
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