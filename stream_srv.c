#include "stream.h"

int createSocketEcoute(char*,int);
void dialogueAvecClient();
int acceptConnect(int);
int main(){
    int sockEcoute, sockDialogue;
    //creation socket ecoute, association adressage et mise en ecoute
    sockEcoute = createSocketEcoute(IP_SVC,PORT_SVC);
    //Boucle de service
    while(1){
       //Acceptation d'un appel 
        sockDialogue = acceptConnect(sockEcoute);
     /*   //Dialogue avec un client
        dialogueAvecClt(sockDialogue);
        //Fermeture socket dialogue 
        close(sockDialogue);*/
    }
    
    close(sockEcoute);

    return 0;
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

    CHECK(listen(sock, MAX_CLTS),"Listen\t\t[FAILED]");
    return sock;
}

void dialogueAvecClient(){

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

