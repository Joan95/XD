/* Aplicació que intercanvia un missatge aleatori entre dos
   ordinadors mitjançant el protocol TCP.  El mateix programa pot
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
*  Funció que fa de servidor TCP, rep per paràmetre el PORT on escoltarà
*  Descripció: Es queda a l'espera en el port indicat, quan rep una
*              connexió llegeix d'un socket TCP i envia un missatge amb
*	           una resposta aleatòria del vector de respostes.
*************************************************************************/
void server(int numPort){
     const char *respostes[] = {"No la suficient","Jo robot!","Fa, Sol, La i Si"}; // vector de respostes
     int sockFd, nouSockFd;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n, nResp;
     char buffer[256];  // buffer per rebre el missatge TCP

     printf("\n\nMODE SERVIDOR port %d\n\n",numPort);//////////////////////////////////////////////////////////////////////////////////////
     printf("1.- Creant el socket\n");

     sockFd = socket(AF_INET, SOCK_STREAM, 0); // es guarda l'identificador del socket TCP principal que s'està creant
     if (sockFd < 0) // si hi ha hagut error creant el socket
        error("ERROR creant el socket");


     bzero((char *) &serv_addr, sizeof(serv_addr)); // s'inicialitza la vble serv_addr a 0
     serv_addr.sin_family = AF_INET; // tipus d'adreça, en aquest cas IPv4
     serv_addr.sin_addr.s_addr = INADDR_ANY; // permet enllaçar el socket a qualsevol interface
     serv_addr.sin_port = htons(numPort); // inverteix el pes dels bits del port

     printf("2.- Bind\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
     if (bind(sockFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) // enllaça l'adreça amb l'identificador del socket principal
              error("ERROR en el binding");


     printf("3.- Listen\n");  ////////////////////////////////////////////////////////////////////////////////////////////////
     listen(sockFd,5); // espera a rebre una connexió, com a màxim pot tenir 5 connexions pendents, BLOQUEJANT
     clilen = sizeof(cli_addr); // mida en bytes de la vble cli_addr

     printf("4.- Accept\n");  ////////////////////////////////////////////////////////////////////////////////////////////////
     nouSockFd = accept(sockFd, (struct sockaddr *) &cli_addr, &clilen); // enllaça la connexió rebuda a un nou identificador
							//de socket, plena la vble cli_addr amb les dades del client rebut

     if (nouSockFd < 0) // si no s'ha pogut crear el nou identificador
          error("ERROR en l'accept");

     printf("5.- Esperant missatge...\n");  ////////////////////////////////////////////////////////////////////////////////////////////////
     bzero(buffer,256); // inicialitza la vble buffer a 0
     n = read(nouSockFd,buffer,255); // lectura del socket sobre la vble buffer, com a molt 256 bytes, BLOQUEJANT

     if (n < 0) // si no s'ha pogut llegir cap byte
        error("ERROR llegint del socket");

     printf("Missatge rebut: %s\n",buffer); // es mostra el missatge rebut per pantalla

     printf("6.- Escrivint missatge...\n");  ////////////////////////////////////////////////////////////////////////////////////////////////
     nResp=rand() % 3; // número aleatori per triar una resposta
     n = write(nouSockFd,respostes[nResp],strlen(respostes[nResp]));  //escriptura en el socket de la resposta aleatòria

     printf("Missatge enviat: %s\n",respostes[nResp]);

     if (n < 0)  // si s'ha pogut escriure el missatge
        error("ERROR writing to socket");

     //Tanca la comunicació//////////////////////////////////////////////////////////////////////////////////////////////////
     close(nouSockFd); // del socket per atendre la connexió
     close(sockFd); // del socket principal

     printf("Finalitzat\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

}

/************************************************************************
*  Funció que fa de client TCP, rep per paràmetre el PORT i l'adreça IP
*  Descripció: Genera una pregunta aleatòria que envia a l'adreça i port 
*  rebuts, espera un missatge de confirmació de que el servidor ha rebut
*  la cadena
*************************************************************************/
void client(int portno, char * host){
    const char *preguntes[] = {"Tens por?","Ets un robot?","Do, Re, Mi..."}; // vector de preguntes
    int sockFd, n, nPreg;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256]; //buffer per enviar i rebre el missatge TCP

    printf("\n\nMODE CLIENT\n\n");//////////////////////////////////////////////////////////////////////////////////////
    printf("1.- Creant el socket\n");

    sockFd = socket(AF_INET, SOCK_STREAM, 0); // es guarda l'identificador del socket TCP principal que s'està creant

    if (sockFd < 0)// si hi ha hagut error creant el socket
        error("ERROR creant el socket");

    server = gethostbyname(host); // tradueix el nom del host rebut en una adreça ip

    if (server == NULL) { // si no s'ha pogut traduïr el nom en ip
        fprintf(stderr,"ERROR, no es troba adreça ip per a aquest host %s\n", host);
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); // inicialitza a 0 la vble serv_addr
    serv_addr.sin_family = AF_INET; // tipus d'adreça, en aquest cas IPv4
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length); // copia l'adreça del host que hem traduït a l'struct sockaddr_in
    serv_addr.sin_port = htons(portno); //assigna el port a l'struct sockaddraddr_in

    printf("2.- Connectant\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

    if (connect(sockFd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //si es pot connectar al servidor
        error("ERROR connectant amb el el servidor");

    nPreg = rand() % 3; // número aleatori entre 0 i 2

    printf("\n3.- Enviant missatge: %s\n",preguntes[nPreg]);  //////////////////////////////////////////////////////////////////////////////////////////////////
    n = write(sockFd,preguntes[nPreg],strlen(preguntes[nPreg])); // envia al servidor la pregunta aleatòria generada
    if (n < 0) // si ha pogut enviar algun byte del missatge
         error("ERROR escrivint en el socket");

    printf("4.- Esperant missatge...\n");  //////////////////////////////////////////////////////////////////////////////////////////////////
    bzero(buffer,256);
    n = read(sockFd,buffer,255); // llegeix la resposta del servidor sobre la vble buffer
    if (n < 0) // si s'ha pogut llegir algún byte del servidor, BLOQUEJANT
         error("ERROR reading from socket");
    printf("Missatge rebut: %s\n",buffer); // mostra el missatge llegit
    close(sockFd);  // es tanca la connexió amb el server

    printf("Finalitzat\n");  //////////////////////////////////////////////////////////////////////////////////////////////////

}

/************************************************************************
*  Funció principal, comprova que el nombre de paràmetres sigui correcte
*************************************************************************/
int main(int argc, char *argv[]){

     if (argc < 3) {  //si el número de paràmetres és incorrecte
         fprintf(stderr,"ERROR, El format correcte és:\n\tdialegtcp host port server (dialegtcp 10.40.115.3 10000 1)");
         exit(1);
     }

     srand ( getpid() ); // llavor per als nombres aleatoris
     if (atoi(argv[3]) == 1){
	     server(atoi(argv[2])); // s'indica el port introduït per paràmetre
     }else{
	     client(atoi(argv[2]),argv[1]); // s'indica el port i la ip a on es vol realitzar la connexió
     }

     return 0;
}
