#include "funcionesaux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* CREA EL PRIMER TABLERO DE LA PARTIDA EL CUAL ESTARÁ RELLENO EXCLUSIVAMENTE DE -*/
void crearTableroVacio(int posPartida){
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<10; j++)
		{
			buscaminas[posPartida].tableroPartida[i][j]='-';
		}
	}
}		

/* CREAR EL TABLERO CON LAS BOMBAS*/
void crearTablerosRellenos(int x)
{
	srand(time(NULL));

	//Rellenar todo el tablero de guiones
	for(int i=0; i< 20; i++)
	{
		for(int j=0; j<20; j++)
		{
			buscaminas[x].tableroBombas[i][j]='-';
		}
	}

	//Colocar aleatoriamente 20 bombas
	int pos_i,pos_j;
	for(int i=0; i<20; i++)
	{
		do{
			pos_i=rand()%10;
			pos_j=rand()%10;

		}while(buscaminas[x].tableroBombas[pos_i][pos_j]=='*');

		buscaminas[x].tableroBombas[pos_i][pos_j]='*';
	}
	//Termino de rellenar todo el tablero
	int cont;
	for(int i=0; i< 20; i++)
	{	
		for(int j=0; j<20; j++)
		{
			if(buscaminas[x].tableroBombas[i][j]!='*')
			{
				cont=0;
				//i-1,j-1
				if(i!=0 && j!=0){
					if(buscaminas[x].tableroBombas[i-1][j-1]=='*'){
						cont++;
					}
				}
				//i-1,j
				if(i!=0){
					if(buscaminas[x].tableroBombas[i-1][j]=='*'){
						cont++;
					}
				}
				//i-1,j+1
				if(i!=0 && j!=9){
					if(buscaminas[x].tableroBombas[i-1][j+1]=='*'){
						cont++;
					}
				}
				//i,j-1
				if(j!=0){
					if(buscaminas[x].tableroBombas[i][j-1]=='*'){
						cont++;
					}
				}
				//i,j+1
				if(j!=9){
					if(buscaminas[x].tableroBombas[i][j+1]=='*'){
						cont++;
					}
				}
				//i+1,j-1
				if(i!=9 && j!=0){
					if(buscaminas[x].tableroBombas[i+1][j-1]=='*'){
						cont++;
					}
				}
				//i+1,j
				if(i!=9){
					if(buscaminas[x].tableroBombas[i+1][j]=='*'){
						cont++;
					}
				}
				//i+1,j+1
				if(i!=9 && j!=9){
					if(buscaminas[x].tableroBombas[i+1][j+1]=='*'){
						cont++;
					}
				}

				buscaminas[x].tableroBombas[i][j]=cont+'0';
			}
		}
	}
}

/* FUNCION PARA ESCRIBIR EN EL FICHERO EL USUARIO Y LA CONTRASEÑA QUE NOS PASEN*/
void registrar(int nUser){
	FILE *fich = NULL;
	char mensaje[40];
	fich=fopen("users.txt","a");
	fprintf(fich,"%s %s \n",usuarios[nUser].nombre,usuarios[nUser].password);
	fclose(fich);
	strcpy(mensaje, "Usuario registrado correctamente\n");
    send(usuarios[nUser].socket,mensaje,strlen(mensaje),0);
}

int leerUsuario(int nUser,char user[40]) // devuelve 1 si el usuario es correcto
{
	char mensaje[40];
	FILE *fich=NULL;
	char usuario[40];
	char contrasena[40];
	int bandera = 0;
	fich=fopen("users.txt","r");
	if(fich==NULL){//control de errores al abrir el fichero
		printf("ERROR al abrir el fichero para comprobar los credenciales\n");
		exit(-1);
	}
	while(fscanf(fich,"%s %s",usuario,contrasena) == 2){
		if(strcmp(usuario,user) == 0){
			strcpy(mensaje, "Usuario encontrado, prosiga con la contraseña.\n");
            send(usuarios[nUser].socket,mensaje,strlen(mensaje),0); 
			bandera = 1;
			strcpy(usuarios[nUser].nombre,user);
		}
	}
	if(bandera == 0){
		strcpy(mensaje, "Usuario no encontrado, pruebe a registrarse\n");
        send(usuarios[nUser].socket,mensaje,strlen(mensaje),0); 
	}
	fclose(fich);
	return bandera;
}
int leerContrasena(int nUser,char pass[40]) // devuelve 1 si es correcta y 0 si es incorrecta
{
	char mensaje[40];
	FILE *fich=NULL;
	char usuario[40];
	char contrasena[40];
	int bandera = 0;
	fich=fopen("users.txt","r");
	if(fich==NULL){//control de errores al abrir el fichero
		printf("ERROR al abrir el fichero para comprobar los credenciales\n");
		exit(-1);
	}

	while(fscanf(fich,"%s %s",usuario,contrasena) == 2){
		if(strcmp(usuario,usuarios[nUser].nombre) == 0){
			if(strcmp(contrasena,pass) == 0){
				strcpy(mensaje, "Contraseña correcta.\n");
            	send(usuarios[nUser].socket,mensaje,strlen(mensaje),0); 
				usuarios[nUser].logged = 1;
				bandera = 1;
				return 1;
			}
		}
	}
	if(bandera == 0){
		strcpy(mensaje, "Contraseña incorrecta\n");
        send(usuarios[nUser].socket,mensaje,strlen(mensaje),0); 
		return 0;
	}
	fclose(fich);

	return -1;
}
void revelarRecursiva(int posPartida,int i, int j){
	//i-1,j-1
	if(i!=0 && j!=0){
		if(buscaminas[posPartida].tableroPartida[i-1][j-1] == '-' && buscaminas[posPartida].tableroBombas[i-1][j-1] != '*'){
			buscaminas[posPartida].tableroPartida[i-1][j-1] = buscaminas[posPartida].tableroBombas[i-1][j-1];
			if(buscaminas[posPartida].tableroPartida[i-1][j-1] == '0')
				revelarRecursiva(posPartida,i-1,j-1);
		}
	}
	//i-1,j
	if(i!=0){
		if(buscaminas[posPartida].tableroPartida[i-1][j] == '-' && buscaminas[posPartida].tableroBombas[i-1][j] != '*'){
			buscaminas[posPartida].tableroPartida[i-1][j] = buscaminas[posPartida].tableroBombas[i-1][j];
			if(buscaminas[posPartida].tableroPartida[i-1][j] == '0')
				revelarRecursiva(posPartida,i-1,j);
		}
	}
	//i-1,j+1
	if(i!=0 && j!=9){
		if(buscaminas[posPartida].tableroPartida[i-1][j+1] == '-' && buscaminas[posPartida].tableroBombas[i-1][j+1] != '*'){
			buscaminas[posPartida].tableroPartida[i-1][j+1] = buscaminas[posPartida].tableroBombas[i-1][j+1];
			if(buscaminas[posPartida].tableroPartida[i-1][j+1] == '0')
				revelarRecursiva(posPartida,i-1,j+1);
		}

	}
	//i,j-1
	if(j!=0){
		if(buscaminas[posPartida].tableroPartida[i][j-1] == '-' && buscaminas[posPartida].tableroBombas[i][j-1] != '*'){
			buscaminas[posPartida].tableroPartida[i][j-1] = buscaminas[posPartida].tableroBombas[i][j-1];
			if(buscaminas[posPartida].tableroPartida[i][j-1] == '0')
				revelarRecursiva(posPartida,i,j-1);
		}
		
	}
	//i,j+1
	if(j!=9){
		if(buscaminas[posPartida].tableroPartida[i][j+1] == '-' && buscaminas[posPartida].tableroBombas[i][j+1] != '*'){
			buscaminas[posPartida].tableroPartida[i][j+1] = buscaminas[posPartida].tableroBombas[i][j+1];
			if(buscaminas[posPartida].tableroPartida[i][j+1] == '0')
				revelarRecursiva(posPartida,i,j+1);
		}
		
	}
	//i+1,j-1
	if(i!=0 && j!=0){
		if(buscaminas[posPartida].tableroPartida[i+1][j-1] == '-' && buscaminas[posPartida].tableroBombas[i+1][j-1] != '*'){
			buscaminas[posPartida].tableroPartida[i+1][j-1] = buscaminas[posPartida].tableroBombas[i+1][j-1];
			if(buscaminas[posPartida].tableroPartida[i+1][j-1] == '0')
				revelarRecursiva(posPartida,i+1,j-1);
		}
		
	}
	//i+1,j
	if(i!=9){
		if(buscaminas[posPartida].tableroPartida[i+1][j] == '-' && buscaminas[posPartida].tableroBombas[i+1][j] != '*'){
			buscaminas[posPartida].tableroPartida[i+1][j] = buscaminas[posPartida].tableroBombas[i+1][j];
			if(buscaminas[posPartida].tableroPartida[i+1][j] == '0')
				revelarRecursiva(posPartida,i+1,j);
		}
		
	}
	//i+1,j+1
	if(i!=9 && j!=9){
		if(buscaminas[posPartida].tableroPartida[i+1][j+1] == '-' && buscaminas[posPartida].tableroBombas[i+1][j+1] != '*'){
			buscaminas[posPartida].tableroPartida[i+1][j+1] = buscaminas[posPartida].tableroBombas[i+1][j+1];
			if(buscaminas[posPartida].tableroPartida[i-1][j-1] == '0')
				revelarRecursiva(posPartida,i-1,j-1);
		}
		
	}
}


void ponerBandera(int posUsuario,int i, int j){
	int posPartida;
	posPartida = usuarios[posUsuario].nPartida;

	if(buscaminas[posPartida].tableroPartida[i][j] == '-')
	{
		buscaminas[posPartida].tableroPartida[i][j] = usuarios[posUsuario].AB;
	}
	else
	{
		buscaminas[posPartida].tableroPartida[i][j] = 'D';
	}

	usuarios[posUsuario].nBanderas ++;


}


void comprobarFinal(int posUsuario){ 
	int posPartida = usuarios[posUsuario].nPartida;
	char msg[250];
	char msg2[250];
	//printf("buscaminas[posPartida].tableroPartida[i][j] = %c",buscaminas[posPartida].tableroPartida[0][0]);
	for(int i=0;i<10;i++){
		for(int j=0;j<10;j++){
			if((buscaminas[posPartida].tableroPartida[i][j]==usuarios[posUsuario].AB) || (buscaminas[posPartida].tableroPartida[i][j]=='D')){
				printf("hola1\n");
				if(buscaminas[posPartida].tableroBombas[i][j] != '*'){
					printf("hola2\n");
					strcpy(msg,"Ha perdido la partida\n");
					send(usuarios[posUsuario].socket,msg,strlen(msg),0);
					strcpy(msg2,"Ha ganado la partida, enhorabuena!\n");
					send(usuarios[posUsuario].rival,msg2,strlen(msg2),0);
					return;
				}
			}
		}	
	}
	printf("hola3\n");
	strcpy(msg,"Ha ganado la partida, enhorabuena!\n");
	send(usuarios[posUsuario].socket,msg,strlen(msg),0);
	strcpy(msg2,"Ha perdido la partida\n");
	send(usuarios[posUsuario].rival,msg2,strlen(msg2),0);

	//partidaFinalizada(posPartida); //hacer la funcion que ponga todas las cosas de la partida a 0


}
void mostrarTablero(char m[10][10])
{
    int i,j;
    for(i=0;i<10;i++){
        printf("\n");
        for(j=0;j<10;j++){
            printf(" |%c| ",m[i][j]);
        }
    }
    printf("\n");
}

int encontrarJugador(int socket){
	for(int i=0 ; i<40 ; i++){
		if(usuarios[i].socket == socket)
			return i;
	}
	printf("Error del sistema: el socket no es válido\n");
}

void matrizToCadena(char m[10][10],char cadena[201]){
	int k=0;
	for(int i=0 ; i<10 ; i++){
		for(int j=0 ; j<10 ; j++){
			cadena[k]=m[i][j];
			k++;
			cadena[k]=',';
			k++;
		}
		cadena[k] = ';';
		k++;
	}
	cadena[k]='\0';
}

void cadenaToMatriz(char m[10][10],char cadena[201]){
	int fil=0; int col=0;
	for(int i=0; i<strlen(cadena) ; i++){
		if(cadena[i]!=',' && cadena[i]!=';'){
			m[fil][col] = cadena[i];
			col++;		
		}else if(cadena[i] == ';'){
			fil++;
			col=0;
		}
	}
}


/*int main(){

	strcpy(usuarios[0].nombre,"asd");
	strcpy(usuarios[0].password,"uui");
	leerContrasena(0);

	
}*/

