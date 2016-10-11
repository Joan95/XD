/* Aplicaci� que intercanvia un missatge llegit per teclat entre dos
   ordinadors mitjan�ant el protocol UDP.  El mateix programa pot
   funcionar de client i de servidor. El programa rep els par�metres
   de connexi� per la l�nia de comandes.
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
*  Funci� que mostra una cadena de car�cters per la sortida d'error est�ndard
*************************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/************************************************************************
*  Funci� que fa de servidor UDP, rep per par�metre el PORT on escoltar�
*  Descripci�: Es queda a l'espera en el port indicat, quan rep una
*              connexi� llegeix d'un socket UDP i envia un missatge amb
*	           una resposta aleat�ria.
*************************************************************************/
void server(int numPort){
   const char *respostes[] = {"No la suficient","Jo robot!","Fa, Sol, La i Si"}; //vector de respostes
   int sockFd,n, nResp;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[256]; //buffer per rebre el missatge

   printf("\n\nMODE SERVIDOR UDP port %d\n\n",numPort); //////////////////////////////////////////////////////////////////////////////////////////////////
   printf("1.- Creant el socket\n");
   sockFd=socket(AF_INET,SOCK_DGRAM,0); // es guarda l'identificador del socket UDP principal que s'est� creant

   bzero(&servaddr,sizeof(servaddr)); // s'inicialitza la vble servaddr a 0
   servaddr.sin_family = AF_INET; // tipus d'adre�a, en aquest cas IPv4
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY); // permet enlla�ar el socket a qualsevol interface
   servaddr.sin_port=htons(numPort); // inverteix el pes dels bits del port

   printf("2.- Bind\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
   if (bind(sockFd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0) // enlla�a l'adre�a amb l'identificador del socket principal
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
*  Funci� que fa de client UDP, rep per par�metre el PORT i l'adre�a IP
*  Descripci�: Llegeix de teclat una cadena de car�cters que envia a 
*	       l'adre�a i port rebuts, espera un missatge de confirmaci� de
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
   sockFd=socket(AF_INET,SOCK_DGRAM,0);// es guarda l'identificador del socket UDP principal que s'est� creant
   if (sockFd < 0)// si hi ha hagut error creant el socket
        error("ERROR creant el socket");

   bzero(&servaddr,sizeof(servaddr)); // inicialitza la variable servaddr a 0
   servaddr.sin_family = AF_INET; // tipus d'adre�a, en aquest cas IPv4
   servaddr.sin_addr.s_addr=inet_addr(host); // copia l'adre�a del host que hem tradu�t a l'struct sockaddr_in en format x.x.x.x
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
*  Funci� principal, comprova que el nombre de par�metres sigui correcte
*************************************************************************/
int main(int argc, char**argv){
     if (argc < 3) {  //si el n�mero de par�metres �s incorrecte
         fprintf(stderr,"ERROR, El format correcte �s:\n\tdialegudp host port server (dialegudp 10.40.115.3 10000 1)");
         exit(1);
     }

     srand ( getpid() ); // llavor per als nombres aleatoris
     if (atoi(argv[3]) == 1)
	     server(atoi(argv[2])); // s'indica el port introdu�t per par�metre
     else
	     client(atoi(argv[2]),argv[1]); // s'indica el port i la ip a on es vol realitzar la connexi�

    return 0;
}
