#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "image.h"

#define TIMER_ID 0
#define TIMER_INTERVAL 30

#define MAX_GIFTS 5
static int animation_ongoing;

static GLuint textureNames[1];

typedef struct gift{
	float x, y;
	float rotation;
}Gift;

typedef struct santa{
	double x;
	int direction, active;
}Santa;

Gift gifts[MAX_GIFTS];
Santa santas[MAX_GIFTS];

int width, height;

void loadImage();
void resetGame();
void throwGift();
void updateGifts();
void drawGifts();
void drawSantas();
void updateSantas();
void checkHit();
void resetBlock(int index);
void resetSanta(int index);

int timer;
float dropPosition = 0;
int currentGift = 0;
int playWidth = 23;
int santaCount = 0;
int score = 0;
int highscore = 0;
int lives = 5;


/* Deklaracije callback funkcija. */
static void on_timer(int value);
static void on_display(void);
static void on_reshape(int w, int h);
static void on_keyboard(unsigned char key, int x, int y);
static void on_mouse_moved(int x, int y);
static void on_mouse(int button, int state, int x, int y);



int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	/* Kreira se prozor. */
	glutInitWindowSize(640, 420);
	glutCreateWindow(argv[0]);

	/* Registruju se funkcije za obradu dogadjaja. */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutPassiveMotionFunc(on_mouse_moved);
	glutMouseFunc(on_mouse);
	glutDisplayFunc(on_display);

  /* Na pocetku je animacija neaktivna */
	/* Obavlja se OpenGL inicijalizacija. */
	glClearColor(0.7, 0.9, 1, 0);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	animation_ongoing = 0;
	resetGame();
	loadImage();
	glutMainLoop();

	return 0;
}


static void on_keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'g':
			/* Zavrsava se program. */
			if (!animation_ongoing){
				animation_ongoing = 1;
				on_timer(TIMER_ID);
			}
			break;
		case 27:
			/* Zavrsava se program. */
			exit(0);
			break;
	}
}

static void on_mouse_moved(int x, int y) {
	//Pomeramo poziciju poklona na osnovu pozicije misa
	float aspect = (float)width/height;
	dropPosition = x/(float)width;
	dropPosition -= 0.5;
	dropPosition *= aspect;
	dropPosition *= 20;
}

static void on_mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
		throwGift();
  }
}

void loadImage(){
	//Ucitavamo slicicu deda mraza
	glGenTextures(1, textureNames);
	Image* image;
	image = image_init(0, 0);

	image_read(image, "santa.bmp");
	glBindTexture(GL_TEXTURE_2D, textureNames[0]);
	glTexParameteri(GL_TEXTURE_2D,
									GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
									GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,
									GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
									GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
							 image->width, image->height, 0,
							 GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	image_done(image);
}

void resetBlock(int index){
	gifts[index].x = dropPosition;
	gifts[index].y = 20;
	gifts[index].rotation = 0;
}

void resetSanta(int index){
	santas[index].x = 0;
	santas[index].active = 0;
}

void resetGame(){
	if (score>highscore)
		highscore = score;

	srand(time(NULL));
	timer = 0;
	lives = 5;
	score = 0;
	for (int i=0; i<MAX_GIFTS; i++){
		resetBlock(i);
		resetSanta(i);
	}
}

void updateGifts(){
	float dropSpeed = 0.4;
	for (int i=0; i<MAX_GIFTS; i++){
		//Ako poklon nije bacen on je na visini 20 i prati poziciju misa
		if (gifts[i].y >= 20){
			gifts[i].x = dropPosition;
		}
		//Ako je bacen onda se spusta brzinom dropSpeed
		//rotation se koristi za animaciju
		else{
			gifts[i].y -= dropSpeed;
			gifts[i].rotation += 0.2;
		}

		//Ako dodirne pod onda se resetije poklon i igrac gubi zivot
		if (gifts[i].y < 0){
			resetBlock(i);
			lives--;
			if (lives == 0){
				resetGame();
			}
		}
	}
}

void updateSantas(){
	//Sto je vise vremena proslo od pocetka partije to se deda mrazevi brze krecu
	double speed = 0.1 + timer*0.00005;

	for (int i=0; i<MAX_GIFTS; i++){
		if (!santas[i].active)
			continue;

		int dir = 1;
		if (santas[i].direction){
			dir = -1;
		}

		//pomeramo deda mraza levo/desno u zavisnosti od dir
		santas[i].x += speed *dir;
		//Ako se previse udalji od centra onda igrac gubi zivot i deda mraz se resetuje
		if (fabs(santas[i].x)-playWidth > 0){
			resetSanta(i);
			lives--;
			if (lives == 0){
				resetGame();
			}
		}
	}
}

//Proverava sudar deda mraza i poklona
void checkHit(){
	for (int i=0; i<MAX_GIFTS; i++){
		if (!santas[i].active)
			continue;
		//Ako je poklon dovoljno blizu po x i ako je nizi od 4 onda se registruje sudar
		for (int j=0; j<MAX_GIFTS; j++){
			if (fabs(santas[i].x-gifts[j].x) < 2 && gifts[j].y < 4){
				resetBlock(j);
				resetSanta(i);
				score++;
			}
		}
	}
}

void throwGift(){
	//currentGift prati koji poklon sledeci bacamo, kada predje MAX_GIFTS vraca se na 0
	//tako da se redom bacaju pokoni na indeksima 0, 1, 2, 3 ,4 ,1, 2 itd.
	//Ako je y == 20 to znaci da currentGift nije bacen  tako da moze da se baci
	if (gifts[currentGift].y >= 20){
		gifts[currentGift].y = 19.9;
		currentGift = (currentGift+1)%MAX_GIFTS;
	}
}

int countSantas(){
	int count = 0;
	for (int i=0; i<MAX_GIFTS; i++){
		if (santas[i].active)
			count++;
	}
	return count;
}

void addSanta(){
	//Prolazimo kroz niz i prvi deda mraz koji nije aktivan aktiviramo
	for (int i=0; i<MAX_GIFTS; i++){
		if (!santas[i].active){
			santas[i].active = 1;

			//Odredjujemo random pravac (levo/desno) i postavljamo poziciju deda mrazu na ivicu ekrana
			int r = rand()%2;
			if (r){
				santas[i].direction = 0;
				santas[i].x = -playWidth;
			}
			else{
				santas[i].direction = 1;
				santas[i].x = playWidth;
			}

			return;
		}
	}
}

static void on_timer(int value)
{
    if (value != TIMER_ID)
        return;

		//Na svakih 600 frame-ova se povecava broj deda mrazova koji se pojavljuju
		santaCount = timer/600 + 1;
		int currentSantaCount = countSantas();

		//Ako je trenutno aktivno manje nego sto treba dodajemmo deda mraza
		if (currentSantaCount < santaCount){
			addSanta();
		}

		updateGifts();
		updateSantas();
		checkHit();

		timer++;
		/* Forsira se ponovno iscrtavanje prozora. */
		glutPostRedisplay();
    /* Po potrebi se ponovo postavlja tajmer. */
    if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}

static void on_reshape(int w, int h)
{
    /* Pamte se sirina i visina prozora. */
    width = w;
    height = h;

    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 50);
}

void drawGifts(){
	//Prolazimo kroz sve poklone i crtamo ih
	for (int i=0; i<MAX_GIFTS; i++){
		glPushMatrix();
		glColor3f(0.8, 0, 0.1);
		glTranslatef(gifts[i].x, gifts[i].y, 0);
		//Animacija rotiranja - rotation se povecava dok poklon pada
		if (gifts[i].rotation > 0){
			glRotatef(gifts[i].rotation*10, 1, 0, 0);
			glRotatef(gifts[i].rotation*25, 0, 1, i*0.2);
		}
		glScalef(1.5, 1, 1.2);
		glutSolidCube(1);

		//Ukras
		glColor3f(0.1, 0, 0.7);
		glPushMatrix();
		glScalef(0.2, 1.1, 1.1);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glScalef(1.1, 1.1, 0.2);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glScalef(1, 1, 0.3);
		glTranslatef(-0.2, 0.8, 0);
		glutSolidSphere(0.3, 10, 10);
		glTranslatef(0.4, 0, 0);
		glutSolidSphere(0.3, 10, 10);
		glPopMatrix();

		glPopMatrix();
	}
}

void drawImage(){
	//Crtamo kvadrat i postavljamo uv koordinate tako da slika moze da se iscrta kako treba
	glBindTexture(GL_TEXTURE_2D, textureNames[0]);
	glPushMatrix();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}


void drawSantas(){
	//Crtame svakog deda mraza koji je aktivan
	for (int i=0; i<MAX_GIFTS; i++){
		if (!santas[i].active)
			continue;

		glPushMatrix();
		glColor3f(1, 1, 1);
		//Stavlja ih na malo razlicite y da se ne bi preklopili
		glTranslatef(santas[i].x, 1+i*0.2, 0);
		glScalef(1.5, 1.5, 1.5);
		//Flip-uje deda mraza ako se krece u suprotnoj strani u odnosu na slicicu
		if (santas[i].direction)
			glScalef(-1, 1, 1);
		glNormal3f(0, 0, -1);
		drawImage();
		glPopMatrix();
	}


}

void drawText(){
	//Crta tekst za zivote, score i highscore
	char str[50];
	glColor3f(0, 0, 0);
	int len = 0;

	glPushMatrix();
	glRasterPos3f(-13, 18, 2);
	sprintf(str, "Lives: %d", lives);
	len = strlen(str);
	for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(10, 18, 2);
	sprintf(str, "Score: %d", score);
	len = strlen(str);
	for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(10, 17, 2);
	sprintf(str, "Highscore: %d", highscore);
	len = strlen(str);
	for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	glPopMatrix();
}

void drawTree(){
	glPushMatrix();

	glColor3f(0.5, 0.1, 0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(-0.5, 0);
	glVertex2f(-0.5, 2);
	glVertex2f(0.5, 0);
	glVertex2f(0.5, 2);
	glEnd();

	glColor3f(0.1, 0.7, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-3, 2);
	glVertex2f(0, 5);
	glVertex2f(3, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(-2.2, 4);
	glVertex2f(0, 7);
	glVertex2f(2.2, 4);
	glEnd();

	glColor3f(0.1, 0.7, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-1.4, 6);
	glVertex2f(0, 8);
	glVertex2f(1.4, 6);
	glEnd();

	glPopMatrix();
}

void drawTrees(){
	for (int i=0; i<10; i++){
		glPushMatrix();
		glTranslatef(-30+i*6, 3, 0);
		drawTree();
		glTranslatef(-2, 1, -0.1);
		glScalef(0.9, 0.9, 0.9);
		drawTree();
		glPopMatrix();
	}
}

static void on_display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 10, 20,
						0, 10, 0,
						0, 1, 0);

	drawGifts();
	drawSantas();

	//Podloga
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(0, -1, 0);
	glScalef(100, 0, 40);
	glutSolidCube(1);
	glPopMatrix();

	drawText();
	drawTrees();

	glutSwapBuffers();
}
