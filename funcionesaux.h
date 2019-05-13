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

#define MSG_SIZE 250
#define MAX_CLIENTS 50

/* FUNCIONES PARA UTILIZARLAS DENTRO DE EL SERVIDOR O DE EL CLIENTE*/

typedef struct Tablero{//estructura de partida
	int idA;
	int idB;
	int estado; //1 en partida
	char tableroBombas[10][10];//tablero con bombas
	int turno; //0 = turno jugador A ; 1 = turno jugador B
	char tableroPartida[10][10];
}Juego;

typedef struct User{
	int socket;
	int nPartida;
	char AB; // para saber si es el jugador A o B dentro de la partida
	int estado; //1 en partida 0 en espera
	char nombre[40];
	char password[40];
	int logged; // 1 si el usuario está registrado
	int nBanderas;
	int rival;
}User;

//VARIABLES GLOBALES
Juego buscaminas[20];
User usuarios[40];

//LLAMADA A LAS FUNCIONES
void crearTableroVacio(int posPartida);

void crearTablerosRellenos(int x);

void registrar(int nUser);

int leerUsuario(int nUser,char user[40]);

int leerContrasena(int nUser,char pass[40]);

void revelarRecursiva(int posPartida,int i, int j);

void ponerBandera(int posUsuario,int i, int j);

void mostrarMatriz(char** m);

int encontrarJugador(int socket);

void comprobarFinal(int posUsuario);

void mostrarTablero(char m[10][10]);

void matrizToCadena(char matriz[10][10],char cadena[201]);

void cadenaToMatriz(char m[10][10],char cadena[201]);