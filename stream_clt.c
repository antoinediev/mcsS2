#include "stream.h"

int connectServer(char*,char*);
void dialogueAvecServ(int );
void str2rep(buffer_t, reponse*);

int main(int c, char**v){
    int sockAppel;
    if(c<3){
        printf ("usage : %s <adrIP> <port> \n",v[0]);
		exit(-1);
    }
    sockAppel = connectServer(v[1],v[2]);
    dialogueAvecServ(sockAppel);
    close(sockAppel);
    return 0;
}

void str2rep(buffer_t b, reponse* rep){
    sscanf(b,"%u##%s",&rep->code,rep->msg);
}

void req2str(requete req,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%d##%s",req.code,req.msg);
}


void dialogueAvecServ(int sockDialogue){
    buffer_t b;
    requete req;
    reponse rep;
    int i =0, len;
    int nbCarLus;

    while(1){
        memset(req.msg,0,MAX_BUFFER);
        printf(" Code requete (0 pour finir dialogue) :\n");
        scanf("%u",&req.code);
        if(rep.code==200){
            printf(" Message requete :\n");
            scanf("%s",req.msg);
        } 
        req2str(req,b);
        CHECK(write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
        printf("\tmessage requete (lg=%4lu) : #%s# envoyé \n",strlen(b),b);
        memset(b,0,MAX_BUFFER);
        if(rep.code==0) break;
        CHECK(nbCarLus = read(sockDialogue,b,MAX_BUFFER),"Problème lecture reponse");
        str2rep(b,&rep);
        printf("\tmessage de la reponse (lg=%4lu): #%s# reçu\n",strlen(b),b);
        printf("\tcode=#%u#, msg=#%s#\n",rep.code,rep.msg);
    }
    
    
}

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