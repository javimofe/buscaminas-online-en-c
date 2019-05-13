#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "funcionesaux.h"


#define MSG_SIZE 250
#define MAX_CLIENTS 50


/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);



int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    //contadores
    int i,j,k;
	int recibidos;
    char identificador[MSG_SIZE];
    
    int on, ret;


/*----------------------------------------------------
        variables para sscanf               
    -----------------------------------------------------*/ 
    
    int comprobar; //1 si puede introducir la contraseña


    //inicializar las estructuras de usuarios
    for(int w = 0; w<40 ; w++){
        usuarios[w].socket = 0;
        usuarios[w].nPartida = 0;
        usuarios[w].estado = -1;
        usuarios[w].logged = 0;
        usuarios[w].nBanderas = 0;
        usuarios[w].rival = 0;
    }

    //inicializar las estructuras de las partidas
    for(int y = 0; y<20 ; y++){
        buscaminas[y].idA = 0;
        buscaminas[y].idB = 0;
        buscaminas[y].estado = -1;
        buscaminas[y].turno = 0; 
    }

    
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}
    
    // Activaremos una propiedad del socket que permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permitir· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
		from_len = sizeof (from);


		if(listen(sd,1) == -1){
			perror("Error en la operación de listen");
			exit(1);
		}
    
    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);
    
   	
    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
		while(1){
            
            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
            if(salida > 0){
                
                
                for(i=0; i<FD_SETSIZE; i++){
                    
                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {
                        
                        if( i == sd){
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes] = new_sd;
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);
                                
                                    strcpy(buffer, "Bienvenido al buscaminas\n");
                                
                                    send(new_sd,buffer,strlen(buffer),0);

                                    for(int o=0;o<40;o++){
                                        if(usuarios[o].socket == 0){
                                            usuarios[o].socket = new_sd;
                                            printf("Guardado socket %d en posicion vector %d\n",new_sd,o );
                                            break;
                                        }
                                    }
                                
                                    for(j=0; j<(numClientes-1);j++){
                                    
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer, "Nuevo Cliente conectado: %d\n",new_sd);
                                        send(arrayClientes[j],buffer,strlen(buffer),0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"Demasiados clientes conectados\n");
                                    send(new_sd,buffer,strlen(buffer),0);
                                    close(new_sd);
                                }
                                
                            }
                            
                            
                        }
                        else if (i == 0){
                            //Se ha introducido información de teclado
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){
                             
                                for (j = 0; j < numClientes; j++){
                                    send(arrayClientes[j], "Desconexion servidor\n", strlen("Desconexion servidor\n"),0);
                                    close(arrayClientes[j]);
                                    FD_CLR(arrayClientes[j],&readfds);
                                }
                                    close(sd);
                                    exit(-1);
                                
                                
                            }
                            //Mensajes que se quieran mandar a los clientes (implementar)
                            
                        } 
                        else{
                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);

                            
                            if(recibidos > 0){
                                
                                if(strcmp(buffer,"SALIR\n") == 0){
                                    
                                    salirCliente(i,&readfds,&numClientes,arrayClientes);
                                    
                                }
                                else if(strncmp(buffer, "USUARIO",7)==0)
                                {
                                    char aux[20];
                                    sscanf(buffer,"USUARIO %s",aux);
                                    
                                    comprobar=leerUsuario(encontrarJugador(i),aux);
                                }
                                else if(strncmp(buffer, "PASSWORD",8)==0)
                                {
                                    char aux1[20];
                                    sscanf(buffer,"PASSWORD %s",aux1);
                                    if(comprobar == 1){
                                        leerContrasena(encontrarJugador(i),aux1);
                                    }
                                }
                                else if(strncmp(buffer, "REGISTRO",8)==0)
                                {
                                    char nomuser[40];
                                    char cad2[40];
                                    
                                    
                                    sscanf(buffer,"REGISTRO -u %s -p %s",nomuser,cad2);
                                    strcpy(usuarios[encontrarJugador(i)].nombre,nomuser);
                                    strcpy(usuarios[encontrarJugador(i)].password,cad2);
                                    registrar(encontrarJugador(i));
                                }

                                else if(strncmp(buffer, "INICIAR-PARTIDA",15)==0)
                                {
                                    char cadena[201];

                                    if(usuarios[encontrarJugador(i)].logged == 1){
                                        //para meter al jugador en una partida
                                        for(int j=0 ; j<20 ; j++){
                                            
                                            if(!buscaminas[j].idA){
                                                buscaminas[j].idA = i;
                                                strcpy(buffer, "Se le ha asignado la posicion A,Esperando al jugador B...\n");
                                                send(i,buffer,strlen(buffer),0);
                                                usuarios[encontrarJugador(i)].AB = 'A';
                                                usuarios[encontrarJugador(i)].estado = 1;
                                                break;

                                            }
                                            if(!buscaminas[j].idB){
                                                buscaminas[j].idB = i;
                                                strcpy(buffer, "Se le ha asignado la posicion B\n");
                                                send(i,buffer,strlen(buffer),0);
                                                usuarios[encontrarJugador(i)].AB = 'B';
                                                usuarios[encontrarJugador(i)].estado = 1;
                                                usuarios[encontrarJugador(i)].rival = buscaminas[j].idA;
                                                usuarios[encontrarJugador(buscaminas[j].idA)].rival = buscaminas[j].idB;
                                                printf("rival de B: %d\n",usuarios[encontrarJugador(i)].rival);
                                                printf("rival de A: %d\n",usuarios[encontrarJugador(buscaminas[j].idA)].rival);

                                                buscaminas[j].estado = 1;
                                                printf("empieza la partida\n");
                                                crearTablerosRellenos(j);
                                                crearTableroVacio(j);
                                                mostrarTablero(buscaminas[j].tableroBombas);
                                                //Enviar tableros a idA e idB
                                                matrizToCadena(buscaminas[j].tableroPartida,cadena);
                                                send(buscaminas[j].idA,cadena,strlen(cadena),0);
                                                send(buscaminas[j].idB,cadena,strlen(cadena),0);
                                                buscaminas[j].turno = 0; // es el turno del jugador




                                                break;

                                            }

                                        }
                                        
                                    }
                                }

                                else if(strncmp(buffer, "DESCUBRIR",9)==0)
                                {		
                                		if((buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 0 && usuarios[encontrarJugador(i)].AB == 'A' ) || (buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 1 && usuarios[encontrarJugador(i)].AB == 'B' )){
                                        char cadena[201];                                        
                                        if(usuarios[encontrarJugador(i)].estado == 1)
                                        {
                                        
                                            int pos_x,pos_y;

                                            sscanf(buffer,"DESCUBRIR %d,%d",&pos_x,&pos_y);


                                            if((pos_x > 10) && (pos_x < 0) && (pos_y > 10) && (pos_y < 0)){
                                                char msg[250];
                                                strcpy(msg,"ERROR: debe introducir unas cordenadas entre 0-9\n");
                                                send(i,msg,strlen(msg),0);
                                            }
                                            else
                                            {
                                                if(buscaminas[usuarios[encontrarJugador(i)].nPartida].tableroBombas[pos_x][pos_y] != '*')
                                                {
                                                    buscaminas[usuarios[encontrarJugador(i)].nPartida].tableroPartida[pos_x][pos_y]=buscaminas[usuarios[encontrarJugador(i)].nPartida].tableroBombas[pos_x][pos_y];
                                                    revelarRecursiva(usuarios[encontrarJugador(i)].nPartida,pos_x,pos_y);
                                                    matrizToCadena(buscaminas[usuarios[encontrarJugador(i)].nPartida].tableroPartida,cadena);
                                                    send(buscaminas[usuarios[encontrarJugador(i)].nPartida].idA,cadena,strlen(cadena),0);
                                                    send(buscaminas[usuarios[encontrarJugador(i)].nPartida].idB,cadena,strlen(cadena),0);
                                                }else{
                                                    //poner que ha perdido
                                                    char msg1[250];
                                                    char msg2[250];
                                                    strcpy(msg1,"Ha perdido la partida...  =(\n");
                                                    send(i,msg1,strlen(msg1),0);
                                                    printf("%d\n",usuarios[encontrarJugador(i)].rival);
                                                    strcpy(msg2,"HA GANADO LA PARTIDA! ENHORABUENA\n");
                                                    send(usuarios[encontrarJugador(i)].rival,msg2,strlen(msg2),0);
                                                }
                                                if(buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 0){
                                                	buscaminas[usuarios[encontrarJugador(i)].nPartida].turno = 1;
                                                }
                                                else if(buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 1){
                                                	buscaminas[usuarios[encontrarJugador(i)].nPartida].turno = 0;
                                                }
                                            }   
                                    }
                                	}else{
                                		char mensj[250];
                                		strcpy(mensj,"No es su turno\n");
                                		send(i,mensj,strlen(mensj),0);
                                	}
                                }
                                else if(strncmp(buffer, "PONER-BANDERA",13)==0)
                                {
                                	if((buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 0 && usuarios[encontrarJugador(i)].AB == 'A' ) || (buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 1 && usuarios[encontrarJugador(i)].AB == 'B' )){

                                    int posUsuario=encontrarJugador(i);
                                    char cadena[201];
                                    if(usuarios[posUsuario].estado == 1){
                                        
                                        int pos_x,pos_y;
                                        int vlk;

                                        sscanf(buffer,"PONER-BANDERA %d,%d",&pos_x,&pos_y);

                                        ponerBandera(posUsuario,pos_x, pos_y);
                                        matrizToCadena(buscaminas[usuarios[posUsuario].nPartida].tableroPartida,cadena);
                                        send(buscaminas[usuarios[posUsuario].nPartida].idA,cadena,strlen(cadena),0);
                                        send(buscaminas[usuarios[posUsuario].nPartida].idB,cadena,strlen(cadena),0);                                        

                                        if(usuarios[posUsuario].nBanderas == 10){
                                            comprobarFinal(posUsuario);
                                        }

                                        if(buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 0){
	                                    	buscaminas[usuarios[encontrarJugador(i)].nPartida].turno = 1;
	                                    }
                                        else if(buscaminas[usuarios[encontrarJugador(i)].nPartida].turno == 1){
                                        	buscaminas[usuarios[encontrarJugador(i)].nPartida].turno = 0;
                                        }
                                    }
                                	}else{
                                		char mensj[250];
                                		strcpy(mensj,"No es su turno\n");
                                		send(i,mensj,strlen(mensj),0);
                                	}
                                }


                                else{
                                    //si manda algun comando mal

                                	strcpy(buffer,"Comando erróneo, introduzca un comando válido\n");
                                	send(i,buffer,strlen(buffer),0);

                                   
                                    
                                }
                                                                
                                
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                printf("Final de la partida\n");
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);

                                //mandar mensaje de que ha ganado (i es el que se ha desconectado)
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,strlen(buffer),0);


}


void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
