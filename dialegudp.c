/* Aplicació que intercanvia un missatge llegit per teclat entre dos
   ordinadors mitjançant el protocol UDP.  El mateix programa pot
   funcionar de client i de servidor. El programa rep els paràmetres
   de connexió per la línia de comandes.
   Format: dialegtcp ip port server (dialegtcp 10.40.115.3 10000 1).
   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/************************************************************************
*  Funció que mostra una cadena de caràcters per la sortida d'error estàndard
*************************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/************************************************************************
*  Funció que fa de servidor UDP, rep per paràmetre el PORT on escoltarà
*  Descripció: Es queda a l'espera en el port indicat, quan rep una
*              connexió llegeix d'un socket UDP i envia un missatge amb
*	           una resposta aleatòria.
*************************************************************************/
void server(int numPort){
   const char *respostes[] = {"No la suficient","Jo robot!","Fa, Sol, La i Si"}; //vector de respostes
   int sockFd,n, nResp;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[256]; //buffer per rebre el missatge

   printf("\n\nMODE SERVIDOR UDP port %d\n\n",numPort); //////////////////////////////////////////////////////////////////////////////////////////////////
   printf("1.- Creant el socket\n");
   sockFd=socket(AF_INET,SOCK_DGRAM,0); // es guarda l'identificador del socket UDP principal que s'està creant

   bzero(&servaddr,sizeof(servaddr)); // s'inicialitza la vble servaddr a 0
   servaddr.sin_family = AF_INET; // tipus d'adreça, en aquest cas IPv4
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY); // permet enllaçar el socket a qualsevol interface
   servaddr.sin_port=htons(numPort); // inverteix el pes dels bits del port

   printf("2.- Bind\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
   if (bind(sockFd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0) // enllaça l'adreça amb l'identificador del socket principal
      error("ERROR en el binding");

   printf("3.- Esperant missatge...\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
   len = sizeof(cliaddr);
   n = recvfrom(sockFd,mesg,256,0,(struct sockaddr *)&cliaddr,&len); //espera a rebre un missatge, BLOQUEJANT
   if (n < 0) // si no s'ha pogut llegir cap byte
        error("ERROR llegint del socket");

   mesg[n]=0;
   printf("Missatge rebut: %s\n",mesg);

   printf("4.- Enviant missatge...\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

   nResp = rand() % 3; // genera un aleatori entre 0 i 2

   n = sendto(sockFd,respostes[nResp],strlen(respostes[nResp]),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr)); //envia un missatge al client
   if (n < 0)  // si s'ha pogut escriure el missatge
        error("ERROR escrivint al socket");

   printf("Missatge enviat: %s\n", respostes[nResp]); //mostra el missatge rebut

   printf("Finalitzat\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

}


/************************************************************************
*  Funció que fa de client UDP, rep per paràmetre el PORT i l'adreça IP
*  Descripció: Llegeix de teclat una cadena de caràcters que envia a 
*	       l'adreça i port rebuts, espera un missatge de confirmació de
*	       que el servidor ha rebut la cadena
*************************************************************************/
void client(int portNum, char* host)
{
   const char *preguntes[] = {"Tens por?","Ets un robot?","Do, Re, Mi..."};
   int sockFd,n, nPreg;
   struct sockaddr_in servaddr;
   char recvline[256]; // buffer per rebre el missatge UDP

   printf("\n\nMODE CLIENT UDP\n\n"); //////////////////////////////////////////////////////////////////////////////////////////////////
   printf("1.- Creant el socket\n");
   sockFd=socket(AF_INET,SOCK_DGRAM,0);// es guarda l'identificador del socket UDP principal que s'està creant
   if (sockFd < 0)// si hi ha hagut error creant el socket
        error("ERROR creant el socket");

   bzero(&servaddr,sizeof(servaddr)); // inicialitza la variable servaddr a 0
   servaddr.sin_family = AF_INET; // tipus d'adreça, en aquest cas IPv4
   servaddr.sin_addr.s_addr=inet_addr(host); // copia l'adreça del host que hem traduït a l'struct sockaddr_in en format x.x.x.x
   servaddr.sin_port=htons(portNum); // assigna el port a l'struct sockaddr_in

   nPreg = rand() %3;

   printf("\n2.- Enviant missatge: %s\n", preguntes[nPreg]);  //////////////////////////////////////////////////////////////////////////////////////////////////
   sendto(sockFd,preguntes[nPreg],strlen(preguntes[nPreg]),0,(struct sockaddr *)&servaddr,sizeof(servaddr)); // enviament del paquet UDP

   printf("3.- Esperant missatge...\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
   n=recvfrom(sockFd,recvline,256,0,NULL,NULL); //espera a rebre un missatge, BLOQUEJANT
   recvline[n]=0;
   printf("Missatge rebut: %s",recvline); // es mostra el missatge rebut

   printf("\nFinalitzat\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

}

/************************************************************************
*  Funció principal, comprova que el nombre de paràmetres sigui correcte
*************************************************************************/
int main(int argc, char**argv){
     if (argc < 3) {  //si el número de paràmetres és incorrecte
         fprintf(stderr,"ERROR, El format correcte és:\n\tdialegudp host port server (dialegudp 10.40.115.3 10000 1)");
         exit(1);
     }

     srand ( getpid() ); // llavor per als nombres aleatoris
     if (atoi(argv[3]) == 1)
	     server(atoi(argv[2])); // s'indica el port introduït per paràmetre
     else
	     client(atoi(argv[2]),argv[1]); // s'indica el port i la ip a on es vol realitzar la connexió

    return 0;
}
