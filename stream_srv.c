#include "stream.h"

int createSocketEcoute(char*,int);
void dialogueAvecClient(int);
int acceptConnect(int);
int main(){
    int sockEcoute, sockDialogue;
    pid_t pidClient;
    //creation socket ecoute, association adressage et mise en ecoute
    sockEcoute = createSocketEcoute(IP_SVC,PORT_SVC);
    //Boucle de service
    while(1){
       //Acceptation d'un appel 
        sockDialogue = acceptConnect(sockEcoute);
        // création fork service 
        CHECK(pidClient = fork(),"Probleme creation fork");
        if(pidClient == 0){
            //fermeture socket d'écoute
            close(sockEcoute);
            //Dialogue avec un client
            dialogueAvecClient(sockDialogue);
            //Fermeture socket dialogue 
            close(sockDialogue);
            //Fin du client
            exit(0);
        }
        //fermeture socket dialogue
        close(sockDialogue);
    }
    
    close(sockEcoute);
    return 0;
}

void str2req(buffer_t b, requete* req){
    sscanf(b,"%u##%s",&req->code,req->msg);
}

void rep2str(reponse rep,buffer_t b){
    memset(b,0,MAX_BUFFER);
    sprintf(b,"%d##%s",rep.code,rep.msg);
}

int createSocketEcoute(char *ipSvc, int portSvc){
    struct sockaddr_in server;
    int sock,len;
    time_t now;
    //Creation de la socket 
    CHECK(sock = socket(AF_INET, SOCK_STREAM,0),"Create socket\t[FAILED]");
    //preparation adressage serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(portSvc);
    server.sin_addr.s_addr = inet_addr(ipSvc);
    bzero(server.sin_zero,8);
    //Association de l'adressage avec la socket
    CHECK(bind(sock, (struct sockaddr *)&server, sizeof server),"Bind\t\t[FAILED]");

    //Recuperation des infos de la socket 
    len = sizeof(server);
    CHECK(getsockname(sock,(struct sockaddr *)&server, (socklen_t *)&len),"Get socket name\t[FAILED]");
    printf("SRV[%i] : %s\tService is started at adress %s:%4i in socket %i\n",getpid(),ctime(&now),inet_ntoa(server.sin_addr),htons(server.sin_port),sock);
    //Mise à l'écoute de la socket 
    CHECK(listen(sock, MAX_CLTS),"Listen\t\t[FAILED]");
    return sock;
}

void lireRequete(int sockDialogue, requete *req){
    buffer_t b;
    int nbCarLus;

    memset(b,0,MAX_BUFFER);
    CHECK(nbCarLus = read(sockDialogue,b,MAX_BUFFER),"Problème lecture requete");
    printf("message de la requete reçue (ld=%4lu): #%s#\n",strlen(b),b);
    str2req(b,req);
    printf("\tcode:#%u#, msg=#%s#\n", req->code,req->msg);
}

void traiterRequete200(requete req, reponse *rep){
    int i,len;
    len=strlen(req.msg)-1;
    for(i=len;i>=0;i--) rep->msg[len-i] = req.msg[i];
    rep->msg[len+1]='\0';

    rep->code = req.code+len;
}

void traiterRequete(requete req, reponse *rep){
    int i,len;
    
    if(req.code == 200) {
        traiterRequete200(req,rep);
        return;
    }
    for(i=0;i< sizeof(REQ2REP)/sizeof(req2rep);i++) {
        if(req.code == REQ2REP[i].codeReq){
            strcpy(rep->msg, REQ2REP[i].msgRep);
        }
    }
    rep->code = req.code+strlen(rep->msg);
}


void ecrireReponse(int sockDialogue, reponse rep){
    buffer_t b;
    int nbCarLus;

    rep2str(rep,b);
    CHECK(nbCarLus = write(sockDialogue,b,strlen(b)+1),"Problème envoie requete");
    printf("message reponse (lg=%4lu) : #%s# envoyé\n",strlen(b),b);
    printf("\tcode=#%u#, message=#%s\n",rep.code,rep.msg);
}

void dialogueAvecClient(int sockDialogue){
    buffer_t b;
    requete req;
    reponse rep;
    int nbCarLus;
    int i,len;

    while(1){
        lireRequete(sockDialogue, &req);
        if(req.code==0) break;
        traiterRequete(req,&rep);
        ecrireReponse(sockDialogue,rep);
        
    }
}

int acceptConnect(int sockEcoute){
    int sock,len;
    struct sockaddr_in clt,server;
    time_t now;
    len = sizeof(clt);
    CHECK(sock=accept(sockEcoute,(struct sockaddr *)&clt, (socklen_t *)&len),"Accept\t\t[FAILED]");
    len = sizeof(server);
    CHECK(getsockname(sock,(struct sockaddr *)&server, (socklen_t *)&len),"Get socket name\t[FAILED]");
    printf("SRV[%i] : %s\tClient %s:%4i is connected with server %s:%4i using socket %i\n",getpid(),ctime(&now),inet_ntoa(clt.sin_addr),htons(clt.sin_port),inet_ntoa(server.sin_addr),htons(server.sin_port),sock);

    return sock;
}

