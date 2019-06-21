#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <SDL/SDL.h>
#undef main
#include <SDL/SDL_mixer.h>
#include <IL/ilut.h>
#define TICK_INTERVAL 20
#define ALTO_T 20
#define ANCHO_T 10
#define SALTO_NIVEL 2

using namespace std;

int frame=0, tiempo=0, timebase=0;
int cont_bajada=0;
int ancho, alto;
int c_p1[ALTO_T+4][ANCHO_T+4];
int nextpiece[4][6];
int centro_ficha[3];
int next_piece[3];
int nivel=1;
int multiplier=1;
int lines=0;

//int c_p2[ALTO][ANCHO];
enum estados {Menu=0,P1=1,GameOver=2,P2=3, MusicScreen = 4, About = 5};

int estado;
int currentVolume;

float altoCabecera;

Mix_Music *musica;

Mix_Chunk *collisionSound;
Mix_Chunk *clearRowSound;
Mix_Chunk *gameOverSound;

GLuint texturasMenus[5];
GLuint blockTextures[9];

const char* gameBackground = "images/gameBackground.png";
const char* menuBackground = "images/menuBackground.png";
const char* gameOverBackground = "images/gameOverBackground.png";
const char* musicBackground = "images/musicBackground.png";
const char* aboutBackground = "images/aboutBackground.png";
const char* grayBlockTexture = "textures/grayBlock.png";
const char* redBlockTexture = "textures/redBlock.png";
const char* greenBlockTexture = "textures/greenBlock.png";
const char* blueBlockTexture = "textures/blueBlock.png";
const char* pinkBlockTexture = "textures/pinkBlock.png";
const char* yellowBlockTexture = "textures/yellowBlock.png";
const char* orangeBlockTexture = "textures/orangeBlock.png";
const char* lightBlueBlockTexture = "textures/lightBlueBlock.png";
const char* gridBlockTexture = "textures/gridBlock.png";

const char* selectedMusic[4];
char* musicTitles[4];

int selectedTrack = 0;

char scoreText[5];
char maxText[5];
char multiplierText[4];
char linesText[3];
char nivelText[3];

int score=0;

GLuint next_time;

int sacarMAX()
{
    int sacado;
    FILE * pFile;
    pFile = fopen ("max.dat","r");
    fscanf(pFile,"%d",&sacado);
    fclose(pFile);
    return sacado;
}

void meterMAX(int MAX)
{
    if(sacarMAX()<MAX)
    {
        FILE * pFile;
        pFile = fopen ("max.dat","w");
        fprintf(pFile,"%d",MAX);
        fclose(pFile);
    }
}

void playEffect(int effectType)
{
    switch (effectType)
    {
        case 1:
            Mix_Volume(-1,5);
            Mix_PlayChannel(-1, collisionSound, 0);
            break;
        case 2:
            Mix_Volume(-1,50);
            Mix_PlayChannel(-1, clearRowSound, 0);
            break;
        case 3:
            Mix_Volume(-1,100);
            Mix_PlayChannel(-1, gameOverSound, 0);
            break;
    }
}


void initializeAll()
{
    cont_bajada=0;
    nivel=1;
    multiplier=1;
    lines=0;
    score=0;
    currentVolume=20;
}

void incrementLevel()
{
    if(lines>SALTO_NIVEL) nivel=(lines/SALTO_NIVEL);
}

void incrementLines(int numberlines)
{
    lines+=numberlines;
}

void incrementScore(int numberlines)
{
    score = score + (10*(numberlines*numberlines)* (multiplier/100 +1 )*nivel);
    if (multiplier>=1) multiplier=500;
}

void recolocar(int i) {

	int j;

	while(i<ALTO_T){

		for(j=2;j<ANCHO_T+2;j++){

			if(c_p1[i][j]!=9)c_p1[i][j] = c_p1[i+1][j];
		}
		i++;
	}
}


void limpiarMalla() {
    //int c_p1[ALTO_T+2][ANCHO_T+4];

	int i,j;
	int limpiar = 1;
	int n_lineas = 0;

	for(i=2;i<ALTO_T+1;i++){
        limpiar=1;
		for(j=2;(j<=ANCHO_T+2)&&(limpiar!=0);j++){

			if(c_p1[i][j]==0){
				limpiar = 0;
			}

		}

		if(limpiar){

		for(j=0;j<ANCHO_T+4;j++){

			if(c_p1[i][j]!=9)c_p1[i][j]=0;

		}
            n_lineas++;
            recolocar(i--);

		}
	}

	if(n_lineas != 0){
        playEffect(2);
        incrementScore(n_lineas);
        incrementLines(n_lineas);
        incrementLevel();
	}else{
	    playEffect(1);
    }
}


void clearNextPiece()
{
    int i,j;
    for(i=0;i<4;i++)
    {
        for(j=0;j<6;j++)
        {
            nextpiece[i][j]=0;
        }
    }
}

void generateNextPiece()
{
    srand (time(NULL));
    int pieza = rand()%8;

    if (nextpiece[next_piece[0]][next_piece[1]]!=0) pieza = nextpiece[next_piece[0]][next_piece[1]];
    clearNextPiece();

    switch(pieza)
    {
    case 0:
    case 1:
        centro_ficha[0]=22;
        centro_ficha[1]=5;
        centro_ficha[2]=1;

        c_p1[centro_ficha[0]][centro_ficha[1]]=1;
        c_p1[centro_ficha[0]][centro_ficha[1]-1]=1;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=1;
        c_p1[centro_ficha[0]][centro_ficha[1]+2]=1;

        break;

    case 2:
        centro_ficha[0]=22;
        centro_ficha[1]=5;
        centro_ficha[2]=4;

        c_p1[centro_ficha[0]][centro_ficha[1]]=2;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=2;
        c_p1[centro_ficha[0]][centro_ficha[1]-1]=2;
        c_p1[centro_ficha[0]-1][centro_ficha[1]]=2;
        break;

    case 3:
        centro_ficha[0]=22;
        centro_ficha[1]=5;
        centro_ficha[2]=1;

        c_p1[centro_ficha[0]][centro_ficha[1]]=3;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=3;
        c_p1[centro_ficha[0]-1][centro_ficha[1]]=3;
        c_p1[centro_ficha[0]-1][centro_ficha[1]+1]=3;
        break;
    case 4:
        centro_ficha[0]=21;
        centro_ficha[1]=5;
        centro_ficha[2]=1;

        c_p1[centro_ficha[0]][centro_ficha[1]]=4;
        c_p1[centro_ficha[0]][centro_ficha[1]-1]=4;
        c_p1[centro_ficha[0]+1][centro_ficha[1]]=4;
        c_p1[centro_ficha[0]+1][centro_ficha[1]+1]=4;
        break;
    case 5:
        centro_ficha[0]=21;
        centro_ficha[1]=5;
        centro_ficha[2]=3;

        c_p1[centro_ficha[0]][centro_ficha[1]]=5;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=5;
        c_p1[centro_ficha[0]+1][centro_ficha[1]]=5;
        c_p1[centro_ficha[0]+1][centro_ficha[1]-1]=5;
        break;
    case 6:
        centro_ficha[0]=21;
        centro_ficha[1]=5;
        centro_ficha[2]=2;

        c_p1[centro_ficha[0]][centro_ficha[1]]=6;
        c_p1[centro_ficha[0]+1][centro_ficha[1]]=6;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=6;
        c_p1[centro_ficha[0]][centro_ficha[1]+2]=6;
        break;
    case 7:
        centro_ficha[0]=22;
        centro_ficha[1]=5;
        centro_ficha[2]=4;

        c_p1[centro_ficha[0]][centro_ficha[1]]=7;
        c_p1[centro_ficha[0]-1][centro_ficha[1]]=7;
        c_p1[centro_ficha[0]][centro_ficha[1]+1]=7;
        c_p1[centro_ficha[0]][centro_ficha[1]+2]=7;
        break;
    }
    //Nextpiece
    pieza = rand()%8;

    switch(pieza)
    {
    case 0:
    case 1:
        next_piece[0]=2;
        next_piece[1]=2;
        next_piece[2]=3;

        nextpiece[next_piece[0]][next_piece[1]]=1;
        nextpiece[next_piece[0]][next_piece[1]-1]=1;
        nextpiece[next_piece[0]][next_piece[1]+1]=1;
        nextpiece[next_piece[0]][next_piece[1]+2]=1;

        break;

    case 2:
        next_piece[0]=2;
        next_piece[1]=2;
        next_piece[2]=4;

        nextpiece[next_piece[0]][next_piece[1]]=2;
        nextpiece[next_piece[0]][next_piece[1]+1]=2;
        nextpiece[next_piece[0]][next_piece[1]-1]=2;
        nextpiece[next_piece[0]-1][next_piece[1]]=2;
        break;

    case 3:
        next_piece[0]=2;
        next_piece[1]=2;
        next_piece[2]=1;

        nextpiece[next_piece[0]][next_piece[1]]=3;
        nextpiece[next_piece[0]][next_piece[1]+1]=3;
        nextpiece[next_piece[0]-1][next_piece[1]]=3;
        nextpiece[next_piece[0]-1][next_piece[1]+1]=3;
        break;
    case 4:
        next_piece[0]=1;
        next_piece[1]=2;
        next_piece[2]=1;

        nextpiece[next_piece[0]][next_piece[1]]=4;
        nextpiece[next_piece[0]][next_piece[1]-1]=4;
        nextpiece[next_piece[0]+1][next_piece[1]]=4;
        nextpiece[next_piece[0]+1][next_piece[1]+1]=4;
        break;
    case 5:
        next_piece[0]=1;
        next_piece[1]=3;
        next_piece[2]=3;

        nextpiece[next_piece[0]][next_piece[1]]=5;
        nextpiece[next_piece[0]][next_piece[1]+1]=5;
        nextpiece[next_piece[0]+1][next_piece[1]]=5;
        nextpiece[next_piece[0]+1][next_piece[1]-1]=5;
        break;
    case 6:
        next_piece[0]=1;
        next_piece[1]=1;
        next_piece[2]=2;

        nextpiece[next_piece[0]][next_piece[1]]=6;
        nextpiece[next_piece[0]+1][next_piece[1]]=6;
        nextpiece[next_piece[0]][next_piece[1]+1]=6;
        nextpiece[next_piece[0]][next_piece[1]+2]=6;
        break;
    case 7:
        next_piece[0]=2;
        next_piece[1]=1;
        next_piece[2]=4;

        nextpiece[next_piece[0]][next_piece[1]]=7;
        nextpiece[next_piece[0]-1][next_piece[1]]=7;
        nextpiece[next_piece[0]][next_piece[1]+1]=7;
        nextpiece[next_piece[0]][next_piece[1]+2]=7;
        break;
    }

}

// Funcion que carga una imagen, la convierte en una textura, y devuelve el ID de la textura
GLuint loadImage(const char* nombreFichero)
{
	ILuint imagenID;
	GLuint texturaID;
	ILboolean exito;
	ILenum error;
	ilGenImages(1, &imagenID);
	ilBindImage(imagenID);
	cout << nombreFichero << endl;
	exito = ilLoadImage(nombreFichero); 	// Cargamos la imagen


	if (exito)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImagenInfo;
		iluGetImageInfo(&ImagenInfo);
		if (ImagenInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}
		exito = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!exito)
		{
			error = ilGetError();
			cout << "Error en la conversion de la imagen: " << error << endl;
			exit(-1);
		}
		// Generamos una textura
		glGenTextures(1, &texturaID);
		// Asignamos la textura a un nombre
		glBindTexture(GL_TEXTURE_2D, texturaID);
		// Seleccionamos el metodo de clamping de la textura
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Seleccionamos el metodo de interpolacion de la textura
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// Especificamos informacion de la textura
		glTexImage2D(GL_TEXTURE_2D, 				// Tipo de textura
					 0,
					 ilGetInteger(IL_IMAGE_BPP),
					 ilGetInteger(IL_IMAGE_WIDTH),
					 ilGetInteger(IL_IMAGE_HEIGHT),
					 0,
					 ilGetInteger(IL_IMAGE_FORMAT),
					 GL_UNSIGNED_BYTE,		// Tipo de datos de la imagen
					 ilGetData());
 	}
  	else
  	{
		error = ilGetError();
		cout << "Fallo al cargar la imagen: " << error << endl;
		exit(-1);
  	}
 	ilDeleteImages(1, &imagenID); // Borramos la imagen, porque ya hemos copiado los datos en la textura.
	cout << "Textura creada correctamente." << endl;
	return texturaID;
}

void dibujarFondo(GLuint textura){
	// Clear the screen
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Select the texture to use
	glBindTexture(GL_TEXTURE_2D, textura);

	//glColor3ub(255, 255, 255);

	// Draw our texture
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		// Abajo Izquierda
		glTexCoord2f(0.0, 0.0);
		glVertex2i(0, 0);

		// Arriba Izquierda
		glTexCoord2f(0.0, 1.0);
		glVertex2i(0, alto);

		// Arriba Derecha
		glTexCoord2f(1.0, 1.0);
		glVertex2i(ancho, alto);

		// Abajo Derecha
		glTexCoord2f(1.0, 0.0);
		glVertex2i(ancho, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void giro() {


	int opcion;
	int eje;
	int libre = 1;

	opcion = c_p1[centro_ficha[0]][centro_ficha[1]];
	eje = centro_ficha[2];


	switch(opcion){

		case 1:

			if(eje==1){


					if( c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 4;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 1;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					}

			}
			break;

		case 2 :

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					centro_ficha[2] = 2;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;

					}
			}
			else if(eje==2){



					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 3;

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;

					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					centro_ficha[2] = 4;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;


					}
			}
			else if(eje==4){



					if(c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 1;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;

					}
			}


			break;
		case 3:

			break;
		case 4:

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 2;

					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 1;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					}

			}

			break;

		case 5:

			if(eje==3){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 4;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					centro_ficha[2] = 3;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;


					}

			}
			break;
		case 6:

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 2;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;


					if(libre){

					centro_ficha[2] = 3;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					//c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					//c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;

					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 4;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					}
			}
			else if(eje==4){

					if(c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 1;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					}

			}

			break;

		case 7:


			if(eje==1){

					if(c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 2;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					}
			}
			else if(eje==2){

					if(c_p1[centro_ficha[0]+2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 3;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;

					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 4;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){

					centro_ficha[2] = 1;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					}

			}
			break;

	}

}

GLuint time_left(void)
{
    GLuint now;
    now =  glutGet(GLUT_ELAPSED_TIME);

    if (next_time <= now)
        return 0;
    else
        return next_time - now;
}



void desplazamientoDerecha()
{
int opcion;
	int eje;
	int libre = 1;

	opcion = c_p1[centro_ficha[0]][centro_ficha[1]];
	eje = centro_ficha[2];


	switch(opcion){

		case 1:

			if(eje==1){


					if( c_p1[centro_ficha[0]][centro_ficha[1]+3] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+3] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}

			}
			break;

		case 2 :

			if(eje==1){



					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==2){



					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}

			}

			else if(eje==3){

					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}
			}
			else if(eje==4){



					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;



					}
			}


			break;
		case 3:

					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;



					}

			break;
		case 4:

			if(eje==1){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}

			}

			break;

		case 5:

			if(eje==3){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}

			}
			break;
		case 6:

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+3] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+3] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==4){

					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}

			}

			break;

		case 7:


			if(eje==1){

					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==2){

					if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+2][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]][centro_ficha[1]+3] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]+3] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[1] = centro_ficha[1]+1;


					}

			}
			break;

	}
}

void desplazamientoIzquierda()
{


	int opcion;
	int eje;
	int libre = 1;

	opcion = c_p1[centro_ficha[0]][centro_ficha[1]];
	eje = centro_ficha[2];


	switch(opcion){

		case 1:

			if(eje==1){


					if( c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}

			}
			break;

		case 2 :

			if(eje==1){



					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==2){



					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}

			}

			else if(eje==3){

					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}
			}
			else if(eje==4){



					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;



					}
			}


			break;
		case 3:

					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;



					}

			break;
		case 4:

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}

			}

			break;

		case 5:

			if(eje==3){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]+1][centro_ficha[1]-2] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]+1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}

			}
			break;
		case 6:

			if(eje==1){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==4){

					if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]-3] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-3] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}

			}

			break;

		case 7:


			if(eje==1){

					if(c_p1[centro_ficha[0]][centro_ficha[1]-2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]-2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==2){

					if(c_p1[centro_ficha[0]][centro_ficha[1]-3] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-3] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]+2][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){



					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]+2][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){


					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[1] = centro_ficha[1]-1;


					}

			}
			break;

	}


}

void inicializarArray()
{
    int i,j;

    for(i=0;i<24;i++)
    {
        for(j=0;j<14;j++)
        {
            if(i<2) c_p1[i][j]=9;
            else if(j<2 || j>11) c_p1[i][j]=9;
            else c_p1[i][j]=0;
        }
    }
}


/* Funcin para inicializar algunos parmetros de OpenGL */
void init(void)
{
    glClearColor(1.0,1.0,1.0,0.0);
    glEnable(GL_DEPTH_TEST);
    ancho = glutGet(GLUT_SCREEN_WIDTH);
    alto  = glutGet(GLUT_SCREEN_HEIGHT);
    next_time = glutGet(GLUT_ELAPSED_TIME) + TICK_INTERVAL;
    // Parte canario
    altoCabecera = 40 * ancho / 1366;
    score = 0;
    currentVolume = 20;

    ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	texturasMenus[0] = loadImage(menuBackground);
	texturasMenus[1] = loadImage(gameOverBackground);
	texturasMenus[2] = loadImage(musicBackground);
	texturasMenus[3] = loadImage(aboutBackground);
	texturasMenus[4] = loadImage(gameBackground);

	blockTextures[0] = loadImage(grayBlockTexture);
	blockTextures[1] = loadImage(blueBlockTexture);
	blockTextures[2] = loadImage(redBlockTexture);
	blockTextures[3] = loadImage(yellowBlockTexture);
	blockTextures[4] = loadImage(greenBlockTexture);
	blockTextures[5] = loadImage(orangeBlockTexture);
	blockTextures[6] = loadImage(pinkBlockTexture);
	blockTextures[7] = loadImage(lightBlueBlockTexture);
	blockTextures[8] = loadImage(gridBlockTexture);

	selectedMusic[0] = "music/bso1.wav";
	selectedMusic[1] = "music/bso2.wav";
	selectedMusic[2] = "music/bso3.wav";
	selectedMusic[3] = "music/bso4.wav";

    collisionSound = Mix_LoadWAV("music/collisionSound.wav");
	clearRowSound = Mix_LoadWAV("music/clearRowSound.wav");
	gameOverSound = Mix_LoadWAV("music/gameOverSound.wav");

    musicTitles[0] = "Tetris - Original Theme";
	musicTitles[1] = "Start Wars BSO";
	musicTitles[2] = "Lord Of The Rings";
	musicTitles[3] = "Game of Thrones";


}

void stopSound(void)
{
    Mix_HaltMusic();
}

void sonido(void){
	SDL_Init(SDL_INIT_AUDIO);
	int frecuencia = 22050;
	int canales = 2;

	int buffer = 1024;
	Uint16 formato = AUDIO_S16;

	Mix_OpenAudio ( frecuencia, formato, canales, buffer );

	musica = Mix_LoadMUS(selectedMusic[selectedTrack]);


	if(musica != NULL){

		Mix_VolumeMusic(currentVolume);
		Mix_PlayMusic(musica,2);
	}
	else{
		cout << "ERROR reproduciendo cancion..." << endl;
	}
}

void desplazamientoAbajo(int abajo){
	int opcion;
	int eje;
	int libre = 1;

	opcion = c_p1[centro_ficha[0]][centro_ficha[1]];
	eje = centro_ficha[2];
do{
	switch(opcion){

		case 1:

			if(eje==1){

					if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+2] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}
			}
			else if(eje==4){

					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}
			break;

		case 2 :

			if(eje==1){

					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;


					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;
					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;
					}

			}

			else if(eje==3){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}

			break;
		case 3:


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]+1] = opcion ;


					centro_ficha[0] = centro_ficha[0]-1;

					}

			break;
		case 4:

			if(eje==1){



					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-2][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-2][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}

			break;

		case 5:

			if(eje==3){


					if(c_p1[centro_ficha[0]][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0]--;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0]--;

					}

			}
			break;
		case 6:

			if(eje==1){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;


					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+2] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}

			else if(eje==3){

					if(c_p1[centro_ficha[0]-3][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]-3][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-2] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;

					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-2] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}

			break;

		case 7:

			if(eje==1){


					if(c_p1[centro_ficha[0]-3][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;


					c_p1[centro_ficha[0]-3][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;


					}
			}
			else if(eje==2){


					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]-2] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]+1][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]-2] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-2] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]-1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}

			}

			else if(eje==3){

					if(c_p1[centro_ficha[0]-1][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;


					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]+2][centro_ficha[1]] = 0 ;

					c_p1[centro_ficha[0]-1][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;

					}
			}
			else if(eje==4){


					if(c_p1[centro_ficha[0]-2][centro_ficha[1]] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+1] != 0)
						libre = 0;
					else if(c_p1[centro_ficha[0]-1][centro_ficha[1]+2] != 0)
						libre = 0;

					if(libre){

					c_p1[centro_ficha[0]][centro_ficha[1]] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+1] = 0 ;
					c_p1[centro_ficha[0]][centro_ficha[1]+2] = 0 ;

					c_p1[centro_ficha[0]-2][centro_ficha[1]] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+1] = opcion ;
					c_p1[centro_ficha[0]-1][centro_ficha[1]+2] = opcion ;

					centro_ficha[0] = centro_ficha[0]-1;
					}

			}

			break;

     }
}while(libre == 1 && abajo ==1);
    // ha chocado
    if(libre==0) {
    int z;

        for(z=0;z<ANCHO_T+4;z++){

            if(c_p1[22][z] != 0 && c_p1[22][z] != 9){

                estado = GameOver;
                stopSound();
                playEffect(3);

            }
            if(c_p1[23][z] != 0 && c_p1[23][z] != 9){

                estado = GameOver;
                stopSound();
                playEffect(3);
            }
        }
        limpiarMalla();
        generateNextPiece();
}

}

void pintarCuadricula()
{
    int i,j;
    float sideblock,medio;
    int selectedTexture;

    sideblock = alto/24;
    medio = ancho/3;

    //Main
    for(i=1;i<ALTO_T+1;i++)
    {
        for(j=1;j<ANCHO_T+3;j++)
        {
            switch (c_p1[i][j])
            {
            case 1:                     // azul
                glColor3ub(0,171,255);
                selectedTexture = 1;
                break;
            case 2:                     // rojo
                glColor3ub(255,0,0);
                selectedTexture = 2;
                break;
            case 3:                     // amarillo
                glColor3ub(255,255,0);
                selectedTexture = 3;
                break;
            case 4:                     // verde
                glColor3ub(63,203,11);
                selectedTexture = 4;
                break;
            case 5:                     // orange
                glColor3ub(202,25,184);
                selectedTexture = 5;
                break;
            case 6:                     // pink
                glColor3ub(255,124,9);
                selectedTexture = 6;
            break;
            case 7:                     // light blue
                glColor3ub(9,255,215);
                selectedTexture = 7;
            break;
            case 9:                     // gray
                 glColor3ub(9,255,215);
                 selectedTexture = 0;
            break;
            default:                    // blanco
                 glColor4f(255, 255, 255, 0.0);
                selectedTexture = 8;
               break;
            }
        // Pintamos los cuadros

            glBindTexture(GL_TEXTURE_2D, blockTextures[selectedTexture]);

            glColor3ub(0, 0, 0);
            glEnable(GL_TEXTURE_2D);


                glBegin(GL_QUADS);
                glColor3ub(200, 200, 200);
                glColor3ub(255, 255, 255);
                //Abajo Izquierda
                glTexCoord2f(0.0, 0.0);
                glVertex2i(medio+(sideblock*j),20+(sideblock*i));
                //Abajo Derecha
                glTexCoord2f(1.0, 0.0);
                glVertex2i((medio+sideblock)+(sideblock*j),20+(sideblock*i));
                //Arriba Derecha
                glTexCoord2f(1.0, 1.0);
                glVertex2i((medio+sideblock)+(sideblock*j),(20+sideblock)+(sideblock*i));
                //Arriba Izquierda
                glTexCoord2f(0.0, 1.0);
                glVertex2i(medio+(sideblock*j),(20+sideblock)+(sideblock*i));
            glEnd();
            glDisable(GL_TEXTURE_2D);

        }
    }
    //NextPiece int nextpiece[6][8];
    int medio_alto = alto/2;

    for(i=0;i<4;i++)
    {
        for(j=0;j<6;j++)
        {
            switch (nextpiece[i][j])
                        {
            case 1:                     // azul
                glColor3ub(0,171,255);
                selectedTexture = 1;
                break;
            case 2:                     // rojo
                glColor3ub(255,0,0);
                selectedTexture = 2;
                break;
            case 3:                     // amarillo
                glColor3ub(255,255,0);
                selectedTexture = 3;
                break;
            case 4:                     // verde
                glColor3ub(63,203,11);
                selectedTexture = 4;
                break;
            case 5:                     // orange
                glColor3ub(202,25,184);
                selectedTexture = 5;
                break;
            case 6:                     // pink
                glColor3ub(255,124,9);
                selectedTexture = 6;
            break;
            case 7:                     // light blue
                glColor3ub(9,255,215);
                selectedTexture = 7;
            break;
            case 9:                     // gray
                 glColor3ub(9,255,215);
                 selectedTexture = 0;
            break;
            default:                    // blanco
                 glColor4f(255, 255, 255, 0.0);
                selectedTexture = 8;
               break;
            }

        medio = ancho/3;
        medio *=2;
        // Pintamos los cuadros
            glBindTexture(GL_TEXTURE_2D, blockTextures[selectedTexture]);

            glColor3ub(0, 0, 0);
            glEnable(GL_TEXTURE_2D);


                glBegin(GL_QUADS);
                glColor3ub(200, 200, 200);
                glColor3ub(255, 255, 255);
                //Abajo Izquierda
                glTexCoord2f(0.0, 0.0);

           glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0);
                glVertex2i(medio+(sideblock*j),medio_alto+(sideblock*i));
                glTexCoord2f(1.0, 0.0);
                glVertex2i((medio+sideblock)+(sideblock*j),medio_alto+(sideblock*i));
                glTexCoord2f(1.0, 1.0);
                glVertex2i((medio+sideblock)+(sideblock*j),(medio_alto+sideblock)+(sideblock*i));
                glTexCoord2f(0.0, 1.0);
                glVertex2i(medio+(sideblock*j),(medio_alto+sideblock)+(sideblock*i));
            glEnd();

        }
    }
}

void displayString(char *s)
{
	for (int i = 0; i < strlen (s); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,s[i]);
	}
}

/* Funcin que se llamar cada vez que se dibuje en pantalla */
void display ( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (estado)
    {

        case Menu:
            stopSound();
            dibujarFondo(texturasMenus[0]);
            break;

        case P1:
            glRasterPos3i(30,alto - (altoCabecera/2),1);
            displayString("Score: ");
            sprintf(scoreText,"%d",score);
            displayString(scoreText);

            glRasterPos3i(300,alto - (altoCabecera/2),1);
            displayString("Nivel: ");
            sprintf(nivelText,"%d",nivel);
            displayString(nivelText);

            glRasterPos3i(550,alto - (altoCabecera/2),1);
            displayString("Now playing: ");
            displayString(musicTitles[selectedTrack]);

            glRasterPos3i(ancho-100,alto - (altoCabecera/2),1);
            displayString("Lines: ");
            sprintf(linesText,"%d",lines);
            displayString(linesText);

            glRasterPos3i(ancho-350,alto - (altoCabecera/2),1);
            displayString("Multiplicador: ");
            sprintf(multiplierText,"%d",multiplier);
            displayString(multiplierText);


            cont_bajada++;

            if (multiplier>1) multiplier--;

            if(cont_bajada>=(50-nivel*5)) {
                desplazamientoAbajo(0);
                cont_bajada=0;
            }

            pintarCuadricula();
            dibujarFondo(texturasMenus[4]);

            break;

        case GameOver:
            dibujarFondo(texturasMenus[1]);

            meterMAX(score);

            glRasterPos3i(((2*ancho)/4)-95,((alto*3)/5)+50,1);
            displayString("Score: ");
            sprintf(scoreText,"%d",score);
            displayString(scoreText);

            glRasterPos3i(((2*ancho)/4)-95,((alto*3)/5)+100,1);
            displayString("HIGH Score:   ");
            sprintf(maxText,"%d",sacarMAX());
            displayString(maxText);

            break;
        case MusicScreen:
            dibujarFondo(texturasMenus[2]);
            break;
        case About:
            dibujarFondo(texturasMenus[3]);
            break;
    }
    glutSwapBuffers();

    usleep(time_left());
    next_time += TICK_INTERVAL;

}

/* Funcin que se llamar cada vez que se redimensione la ventana */
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

void handleMenuKeyboard(unsigned char key)
{
    cout << "Handling Menu Keyboard: " << key << endl;
    switch ( key )
    {
        case 'm':
        case 'M':
            estado = MusicScreen;
            break;
        case 'g':
        case 'G':
            estado = GameOver;
            break;
        case 'b':
        case 'B':
            estado = About;
            break;
        case 13:
            initializeAll();
            inicializarArray();
            clearNextPiece();
            generateNextPiece();
            estado = P1;
            sonido();
            break;
        case 27:
            exit(0);
    }
}

void handleGameKeyboard(unsigned char key)
{
    cout << "Handling Game Keyboard: " << key << endl;
    switch ( key )
    {
        case 'f':
        case 'F':
            glutFullScreen();
            break;

        case 'd':
        case 'D':
            desplazamientoDerecha();
            break;

        case 'a':
        case 'A':
            desplazamientoIzquierda();
            break;

        case 'w':
        case 'W':
            giro();
            break;

        case 's':
        case 'S':
            desplazamientoAbajo(0);
            break;

        case 'm':
        case 'M':
            stopSound();
            break;

        case '1':
            selectedTrack = 0;
            sonido();
            break;
        case '2':
            selectedTrack = 1;
            sonido();
            break;
        case '3':
            selectedTrack = 2;
            sonido();
            break;
        case '4':
            selectedTrack = 3;
            sonido();
            break;

        case '+':
            currentVolume = currentVolume + 10;
            Mix_VolumeMusic(currentVolume);
            break;

        case '-':
            currentVolume = currentVolume - 10;
            Mix_VolumeMusic(currentVolume);
            break;

        case 32:
            desplazamientoAbajo(1);
            break;

        case 27:
            estado=Menu;
            break;
    }
}

void handleMusicKeyboard(unsigned char key)
{
    cout << "Handling Music Keyboard: " << key << endl;
    switch ( key )
    {
        case '1':
            selectedTrack = 0;
            sonido();
            estado = Menu;
            break;
        case '2':
            selectedTrack = 1;
            sonido();
            estado = Menu;
            break;
        case '3':
            selectedTrack = 2;
            sonido();
            estado = Menu;
            break;
        case '4':
            selectedTrack = 3;
            sonido();
            estado = Menu;
            break;
        case 27:
            estado = Menu;
            break;
    }
}

void handleAboutKeyboard(unsigned char key)
{
    cout << "Handling About Keyboard: " << key << endl;
    switch ( key )
    {
        case 27:
            estado = Menu;
            break;
    }
}

void handleGameOverKeyboard(unsigned char key)
{
    cout << "Handling About Keyboard: " << key << endl;
    switch ( key )
    {
        case 13:
            sonido();
            initializeAll();
            inicializarArray();
            clearNextPiece();
            generateNextPiece();
            estado = P1;
            break;

        case 32:
            estado = Menu;
            break;

        case 27:
            exit(0);
            break;
    }
}

/* Funcin que controla los eventos de teclado */
void keyboard ( unsigned char key, int x, int y )
{
    switch (estado)
    {
        case Menu:
            handleMenuKeyboard(key);
            break;
        case P1:
            handleGameKeyboard(key);
            break;
        case MusicScreen:
            handleMusicKeyboard(key);
            break;
        case GameOver:
            handleGameOverKeyboard(key);
            break;
        case About:
            handleAboutKeyboard(key);
            break;
    }

    glutPostRedisplay();
}

void keyboardSpecial (int key, int x, int y)
{
    if(estado = P1){
        switch (key)
        {
        case GLUT_KEY_RIGHT:
            desplazamientoDerecha();
            break;

        case GLUT_KEY_LEFT:
            desplazamientoIzquierda();
            break;

        case GLUT_KEY_UP:
            giro();
            break;

        case GLUT_KEY_DOWN:
            desplazamientoAbajo(0);
            break;

        case GLUT_KEY_F10:
            break;
        }
    }
    glutPostRedisplay();
}

/* Funcin que se ejecuta cuando no hay eventos */
void idle(void)
{
    char s[20];
    frame++;
    tiempo=glutGet(GLUT_ELAPSED_TIME);

    if (tiempo - timebase > 1000)
    {
        sprintf(s,"FPS:%4.2f",frame*1000.0/(tiempo-timebase));
        glutSetWindowTitle(s);    //no valdra a pantalla completa
        timebase = tiempo;
        frame = 0;
    }
    glutPostRedisplay();
}

/* Funcin principal */
int main(int argc, char** argv)
{
    inicializarArray();
    estado = Menu;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tetris");
    glutSetCursor(GLUT_CURSOR_NONE);
    glutFullScreen();
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
