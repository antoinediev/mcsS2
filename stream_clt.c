#include "stream.h"

/** Prototypes **/ 
int connectServer(char*,char*);
void dialogueAvecServ(int );
void str2rep(buffer_t, reponse*);

/* MAIN */
int main(int c, char**v){
    int sockAppel;
    //verification des parametres
    if(c<3){
        printf ("usage : %s <adrIP> <port> \n",v[0]);
		exit(-1);
    }
    //ouverture de la socket
    sockAppel = connectServer(v[1],v[2]);
    //dialogue avec le serveur via la socket 
    dialogueAvecServ(sockAppel);
    //fermeture de la socket
    close(sockAppel);
    return 0;
}
/**
 * str2rep
 * Deserialisation de la chaine de caractère
 * 
 */
void str2rep(buffer_t b, reponse* rep){
    //prend en compte les espaces dans la chaine de caractere de reponse
    sscanf(b,"%u##%[^\t\n]",&rep->code,rep->msg);
}

/**
 * req2str
 * serialisation de la reponse
 * 
 */
void req2str(requete req,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%d##%s",req.code,req.msg);
}

/**
 * dialogueAvecServ
 * Dialogue avec le serveur après avoir recuperé le code et le message
 * 
 */
void dialogueAvecServ(int sockDialogue){
    buffer_t b;
    requete req;
    reponse rep;
    int i =0, len;
    int nbCarLus;
    while(1){
        memset(req.msg,0,MAX_MSG);
        printf(" Code requete (0 pour finir dialogue) :\n");
        //recupere le code
        scanf("%u",&req.code);
        if(req.code==200){
            //recupere le message si code egal à 200
            printf(" Message requete :\n");
            scanf("%s",req.msg);
        } else {
            sprintf(req.msg,"");
        }
        //Serialisation de la requete 
        req2str(req,b);
        //envoie de la requete
        CHECK(write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
        printf("\tmessage requete (lg=%4lu) : #%s# envoyé \n",strlen(b),b);
        memset(b,0,MAX_BUFFER);
        // Code zero pour arreter le dialogue
        if(req.code==0) break;
        //recuperation de la reponse
        CHECK(nbCarLus = read(sockDialogue,b,MAX_BUFFER),"Problème lecture reponse");
        //desarialisation de la reponse
        str2rep(b,&rep);
        //affichage de la reponse
        printf("\tmessage de la reponse (lg=%4lu): #%s# reçu\n",strlen(b),b);
        printf("\tcode=#%u#, msg=#%s#\n",rep.code,rep.msg);
    }  
}
/**
 * connectServer
 * Connection au serveur via les paramêtre passé a la commande
 *
 */
int connectServer(char *hostAddr, char *portNum){
    struct hostent *host;
    buffer_t msgUsage;
    int sock,len;
    struct sockaddr_in serv, myAddr;
    time_t now;
    //Resolution DNS par adresse 
    len = sizeof(myAddr);
    //sprintf(msgUsage,"%s is unknow host : can't resolve\n",hostAddr);
    //CHECKp(host = gethostbyaddr(hostAddr, len , AF_INET),msgUsage);
    // ou resolution DNS par nom
    //CHECKp(host = gethostbyname(hostName),msgUsage);
    //Creation socket
    CHECK(sock=socket(AF_INET,SOCK_STREAM,0),"Can't create socket");
    //Preparation serveur
    serv.sin_family = AF_INET;
    serv.sin_port = htons(atoi(portNum));
    
    //bcopy(host->h_addr,(char *) &serv.sin_addr, host->h_length);
    
    serv.sin_addr.s_addr = inet_addr(hostAddr);
    //serv.sin_addr.s_addr = inet_addr(host->h_addr);

    bzero(serv.sin_zero,8); //ie : memset(serv.sin_zero,8,0)
    //Connexion au serveur 
    len = sizeof(serv);
    CHECK(connect(sock, (struct sockaddr *)&serv,len),"Can't connect");
    //Quel est mon adressage ? 
    len = sizeof(myAddr);
    CHECK(getsockname(sock, (struct sockaddr *)&myAddr,(socklen_t *)&len),"Can't get socket name");
    CHECK(time(&now),"Get time\t[FAILED]");
    printf("client[%i]: %s\n\tUsed address by the client is %s\n\tAllocated port for the client is %i\n",getpid(),ctime(&now),inet_ntoa(myAddr.sin_addr),ntohs(myAddr.sin_port));
    return sock;
}