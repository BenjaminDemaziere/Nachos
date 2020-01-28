// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

/*
./nachos-mynetwork  -m 0 -o 1 -l 0.7 -d r
./nachos-mynetwork  -m 1 -o 0 -l 0.7 -d r
*/
#include <exception>


#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "socketTCP.h"
#include "interrupt.h"

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[MaxMailSize];


void sendReceive(int farAddr) {
    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;		
    outMailHdr.to = 0;
    outMailHdr.from = 0;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    printf("Send message\n");
    postOffice->Send(outPktHdr, outMailHdr, data); 

    // Wait for the first message from the other machine
    printf("Rec message");
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ack) + 1;
    postOffice->Send(outPktHdr, outMailHdr, ack); 

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);
}



// int i;
// void p(int dummy) {
//     i++;
//     if(i%100000==0) {
//         printf("Interruption\n");
//     }
// }

void
MailTest(int farAddr)
{

    // for(int i=0;i<10;i++)
    //     sendReceive(farAddr);
    // // Timer * t = new Timer(p,(int)t,0);
    // // interrupt->SetLevel(IntOff);
    // // currentThread->Sleep();

    // if(farAddr==0) {
    //     printf("0000\n");

    //     SocketClientTCP * s = new SocketClientTCP();
    //     s->mailHeader.to = 0;
    //     s->mailHeader.from = 0;
    //     s->packetHeader.to = 0;
    //     s->packetHeader.from = 1;


    //     const char * buf = "PTDR\n";

    //     for(int i=0;i<10;i++) {
    //         s->Write(buf,6);
    //         printf("Envoie\n");
    //     }

    // }
    // else if(farAddr==1) {
    //     printf("1111\n");

    //     SocketClientTCP * s = new SocketClientTCP();
    //     s->portSrc = 0;
    //     s->mailHeader.to = 0;
    //     s->mailHeader.from = 0;
    //     s->packetHeader.to = 1;
    //     s->packetHeader.from = 0;

    //     char t[200];
    //     for(int i=0;i<10;i++) {
    //         s->Read(t,6);

    //         printf("%s",t);
    //     }

    // }




    if(farAddr==0) {
        printf("Client\n");

        SocketClientTCP * s = new SocketClientTCP();
        int ret = s->Connect(0,1);
        if(ret==1) {
            printf("Connect\n");


            const char * buf = "PTDR T QUI BRO\n";
            for(int i=0;i<10;i++) {
                s->Write(buf,16);
            }

            s->Close();
        }
        else {
            printf("Can't connect\n");
        }

    }
    //Le serveur est à l'adresse 0
    else if(farAddr==1) {
        printf("Serveur\n");
            SocketServerTCP * sserveur = new SocketServerTCP(2);
            SocketServerTCP * serveur = NULL;

        try{
            serveur = new SocketServerTCP(1);
            SocketClientTCP * s = serveur->Accept();

            printf("Accept\n");
            char t[200];
            int ret;
            for(int i=0;i<1000 && ret!=0;i++) {
                ret = s->Read(t,38);
                printf("%s",t);
            }
        }
        catch(const std::exception& e)
        {
            printf("Erreur port déjà utilisé: %d\n",(int)serveur);
        }
        



    }





    // Then we're done!
    interrupt->Halt();
}
