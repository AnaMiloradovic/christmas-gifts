#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>


#define TIMER_ID 0
#define TIMER_INTERVAL 30

#define MAX_GIFTS 5
#define MAX_LIVES 5
static int animation_ongoing;



typedef struct gift{
	float x, y;
	float rotation;
}Gift;

typedef struct snowman{
	double x;
	int direction, active;
}Snowman;

Gift gifts[MAX_GIFTS];
Snowman snowmen[MAX_GIFTS];

int width, height;


void setGame();
void throwGift();
void updateGifts();
void drawGifts();
void drawSnowmen();
void drawImage();

void updateSnowmen();
void checkHit();
void resetBlock(int index);
void resetSnowman(int index);

void light_init(
	GLfloat* light_ambient,
	GLfloat* light_diffuse,
	GLfloat* light_specular
);

int timer;
float dropPosition = 0;
int currentGift = 0;
int playWidth = 23;
int snowmenCount = 0;
int score = 0;
int highscore = 0;
int lives = MAX_LIVES;


//Deklaracije callback funkcija
static void on_timer(int value);
static void on_display(void);
static void on_reshape(int w, int h);
static void on_keyboard(unsigned char key, int x, int y);
static void on_mouse_moved(int x, int y);
static void on_mouse(int button, int state, int x, int y);



int main(int argc, char **argv) {
    
    //Inicijalizuje se GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// Kreira se prozor
	glutInitWindowSize(1280, 830);
	glutInitWindowPosition(200, 200);
        glutCreateWindow("Christmas Gifts");

	//Registruju se funkcije za obradu dogadjaja
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutPassiveMotionFunc(on_mouse_moved);
	glutMouseFunc(on_mouse);
	glutDisplayFunc(on_display);

  	//Na pocetku je animacija neaktivna
	//Obavlja se OpenGL inicijalizacija
	
	//Postavljamo osnovnu boju na boju neba
	glClearColor(0.678, 0.847, 0.902, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	animation_ongoing = 0;
	
	setGame();
	glutMainLoop();

	return 0;
}

void light_on() {
    
	GLfloat light_ambient[] =  {0.5, 0.5, 0.5, 1};
	GLfloat light_diffuse[] =  {0.9, 0.9, 0.9, 1};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1};
	light_init(light_ambient, light_diffuse, light_specular);
}

void light_off() {
    
	GLfloat light_ambient[] =  {0.1, 0.1, 0.1, 1};
	GLfloat light_diffuse[] =  {0.1, 0.1, 0.1, 1};
	GLfloat light_specular[] = {0.0, 0.0, 0.0, 1};
	light_init(light_ambient, light_diffuse, light_specular);
}

void light_init(
            GLfloat* light_ambient,
            GLfloat* light_diffuse,
            GLfloat* light_specular
	) {
	GLfloat light_position[] = {10.0, 5.0, 0.0, 0};
	
	// ukljucuje se osvjetljenje
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}


static void on_keyboard(unsigned char key, int x, int y) {
    
	switch (key) {
		case 'g':
		case 'G':
                    
			//Pokrece se animacija
			if (!animation_ongoing) {
				setGame();
				animation_ongoing = 1;
				on_timer(TIMER_ID);
			}
			break;
                        
		case 27:
			//Zavrsava se program
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


static void on_mouse(int button, int state, int x, int y) {
    
	//Kontrolisemo ispustanje poklona na klik ako je igra pocela
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && animation_ongoing) {
		throwGift();
	}
}


void resetBlock(int index) {
	gifts[index].x = dropPosition;
	gifts[index].y = 20;
	gifts[index].rotation = 0;
}

void resetSnowman(int index) {
	snowmen[index].x = 0;
	snowmen[index].active = 0;
}

void endGame() {
	//Zavrsavamo igru i ispisujemo rezultat
	if (score>highscore) {
		highscore = score;
	}
	animation_ongoing = 0;

}

void setGame() {
    
	//Postavljanje prve/nove igre
	srand(time(NULL));
	timer = 0;
	lives = MAX_LIVES;
	score = 0;
	for (int i=0; i<MAX_GIFTS; i++){
		resetBlock(i);
		resetSnowman(i);
	}
}

void updateGifts() {
	float dropSpeed = 0.4;
	for (int i=0; i<MAX_GIFTS; i++){
            
		//Ako poklon nije bacen on je na visini 20 i prati poziciju misa
		if (gifts[i].y >= 20) {
			gifts[i].x = dropPosition;
		}
		
		//Ako je bacen onda se spusta brzinom dropSpeed
		//rotation se koristi za animaciju
		else {
			gifts[i].y -= dropSpeed;
			gifts[i].rotation += 0.2;
		}
		//Ako dodirne pod onda se resetije poklon i igrac gubi zivot
		if (gifts[i].y < 0) {
			resetBlock(i);
			lives--;
			if (lives == 0) {
				endGame();
			}
		}
	}
}


void updateSnowmen() {
    
	//Sto je vise vremena proslo od pocetka partije to se snesko belici brze krecu
	double speed = 0.1 + timer*0.00005;

	for (int i=0; i<MAX_GIFTS; i++) {
		if (!snowmen[i].active)
			continue;

		int dir = 1;
		if (snowmen[i].direction) {
			dir = -1;
		}

		//pomeramo sneska levo/desno u zavisnosti od dir
		snowmen[i].x += speed *dir;
                
		//Ako se previse udalji od centra onda igrac gubi zivot i snesko se resetuje
		if (fabs(snowmen[i].x)-playWidth > 0) {
			resetSnowman(i);
			lives--;
			if (lives == 0) {
				animation_ongoing = 0;
			}
		}
	}
}

//Proverava se sudar sneska i poklona
void checkHit(){
	for (int i=0; i<MAX_GIFTS; i++) {
		if (!snowmen[i].active)
			continue;
                
		//Ako je poklon dovoljno blizu po x i ako je nizi od 4 onda se registruje sudar
		for (int j=0; j<MAX_GIFTS; j++) {
			if (fabs(snowmen[i].x-gifts[j].x) < 2 && gifts[j].y < 4) {
				resetBlock(j);
				resetSnowman(i);
				score++;
			}
		}
	}
}

void throwGift() {
    
	//currentGift prati koji poklon sledeci bacamo, kada predje MAX_GIFTS vraca se na 0
	//tako da se redom bacaju pokoni na indeksima 0, 1, 2, 3 ,4 ,1, 2 itd.
	//Ako je y == 20 to znaci da currentGift nije bacen  tako da moze da se baci
	if (gifts[currentGift].y >= 20) {
		gifts[currentGift].y = 19.9;
		currentGift = (currentGift+1)%MAX_GIFTS;
	}
}

int countSnowmen() {
	int count = 0;
	for(int i=0; i<MAX_GIFTS; i++) {
		if(snowmen[i].active)
			count++;
	}
	return count;
}

void addSnowman() {
    
	//Prolazimo kroz niz i prvog sneska koji nije aktivan aktiviramo
	for (int i=0; i<MAX_GIFTS; i++) {
		if (!snowmen[i].active) {
			snowmen[i].active = 1;

			//Odredjujemo random pravac (levo/desno) i postavljamo poziciju
			//sneska na ivicu ekrana
			int r = rand()%2;
			if (r) {
				snowmen[i].direction = 0;
				snowmen[i].x = -playWidth;
			}
			else {
				snowmen[i].direction = 1;
				snowmen[i].x = playWidth;
			}

			return;
		}
	}
}

static void on_timer(int value) {
    if (value != TIMER_ID) {
    	return;
    }

	//Na svakih 600 frame-ova se povecava broj sneska koji se pojavljuju
	snowmenCount = timer/600 + 1;
	int currentSnowmenCount = countSnowmen();

	//Ako je trenutno aktivno manje nego sto treba dodajemmo sneska
	if (currentSnowmenCount < snowmenCount) {
		addSnowman();
	}

	updateGifts();
	updateSnowmen();
	checkHit();

	timer++;
        
	//Forsira se ponovno iscrtavanje prozora
	glutPostRedisplay();
        
        //Po potrebi se ponovo postavlja tajmer
        if (animation_ongoing) {
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
}


static void on_reshape(int w, int h) {
    //Pamte se sirina i visina prozora
    width = w;
    height = h;

    //Podesava se viewport
    glViewport(0, 0, width, height);

    //Podesava se projekcija
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 50);
}


void drawGifts(){
    
	//Prolazimo kroz sve poklone i crtamo ih
	for (int i=0; i<MAX_GIFTS; i++) {
		glPushMatrix();
		glColor3f(0.8, 0, 0.1);
		glTranslatef(gifts[i].x, gifts[i].y, 0);
                
		//Animacija rotiranja - rotation se povecava dok poklon pada
		if (gifts[i].rotation > 0) {
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


void drawSnowman() {
    
	// Iscrtavanje jednog sneska, namestamo boju i materijal
	GLfloat ambient_snowman[]  = {1.0, 0.98, 0.98, 1.0};
	GLfloat diffuse_snowman[]  = {1.0, 0.98, 0.98, 1.0};
	GLfloat specular_snowman[] = {1.0, 0.98, 0.98, 1.0};
	GLfloat shininess_snowman  = 5;
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_snowman);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_snowman);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_snowman);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess_snowman);
	
	
	//Animiramo sneska pomocu 'timer' promenljive
	int hopInterval = 20;
	float bounce = 0;
	
	if (timer%hopInterval >= 0 && timer%hopInterval < hopInterval/2) {
		bounce += timer%hopInterval;
	}
	if (timer%hopInterval >= hopInterval/2 && timer%hopInterval < hopInterval) {
		bounce = hopInterval - timer%hopInterval;
	}
	
	glPushMatrix();
		glutSolidSphere(0.8, 20, 20);
		glTranslatef(0, 0.8 + bounce/30, 0);
		glutSolidSphere(0.6, 20, 20);
		glTranslatef(0, 0.6 + bounce/30, 0);
		glutSolidSphere(0.5, 20, 20);
		glRotatef(90, 0, 1, 0);
		
		//Materijali nosa
		GLfloat ambient_nose[]  = {1.0, 0.5, 0.31, 1.0};
		GLfloat diffuse_nose[]  = {1.0, 0.5, 0.31, 1.0};
		GLfloat specular_nose[] = {1.0, 0.5, 0.31, 1.0};
		GLfloat shininess_nose = 5;
	
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_nose);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_nose);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_nose);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess_nose);
		
		glutSolidCone(0.1, 1, 20, 20);
	glPopMatrix();
}


void drawSnowmen() {
    
	//Crtamo svakog sneska koji je aktivan
	for (int i=0; i<MAX_GIFTS; i++) {
		if (!snowmen[i].active) {
			continue;
		}
			
		glPushMatrix();
                
		//Stavlja ih na malo razlicite y da se ne bi preklopili
		glTranslatef(snowmen[i].x, 1+i*0.2, 0);
		glScalef(1.5, 1.5, 1.5);
                
		//Okrecemo sneska ako se krece u suprotnoj strani u odnosu na sliku
		if (snowmen[i].direction) {
			glScalef(-1, 1, 1);
		}
		
		glNormal3f(0, 0, -1);
		drawSnowman();
		glPopMatrix();
	}
}

void drawText() {
    
	//Crtamo tekst za zivote, score i highscore
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

void drawEndText() {
    
	//Crtamo tekst sa instrukcijama za resetovanje igre
	char str[50];
	int len = 0;
	
	char* text1 = "Game over!";
	char* text2 = "Press 'G' to Start";
	char* text3 = "Press 'G' to Restart";

	if (!lives) {
		glColor3f(1, 0, 0);
		glPushMatrix();
			glRasterPos3f(-0.9, 14, 2);
			sprintf(str, "%s", text1);
			len = strlen(str);
			for (int i = 0; i < len; i++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
			}
		glPopMatrix();
	}
	
	glColor3f(0, 0, 0);
	glPushMatrix();
		glRasterPos3f(-1.6, 13, 2);
		lives ? sprintf(str, "%s", text2) : sprintf(str, "%s", text3);
		len = strlen(str);
		for (int i = 0; i < len; i++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}
	glPopMatrix();
}

void drawTree() {
    
	//Iscrtavamo 3D drvo od 3 konusa
	GLfloat ambient[] = {0.0, 0.3, 0.0, 1};
	GLfloat diffuse[] = {0.0, 0.3, 0.0, 1};
	GLfloat specular[] = {0.0, 0.3, 0.0, 1};
	GLfloat shininess = 5;
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	
	glPushMatrix();
		glRotatef(-90.0, 1, 0, 0);
		glutSolidCone(2.5, 3.5, 20, 20);
		glTranslatef(0, 0, 2);
		glutSolidCone(2.0, 3.0, 20, 20);
		glTranslatef(0, 0, 1.5);
		glutSolidCone(1.5, 2.5, 20, 20);
	glPopMatrix();
}

void drawTrees(){
    
	//Podesavamo gustinu pozadine izborom kolicine drveca
	int trees = 15;
	int treeRows = 6;
	for (int i=0; i<treeRows; i++) {
		for (int j=0; j<trees; j++){
			glPushMatrix();
				glTranslatef(-30+j*6+i*3, 3, -3-3*i);
				drawTree();
			glPopMatrix();
		}
	}
	
}

void drawGround() {
    
	//Iscrtavamo zemlju
	GLfloat ambient_ground[] = {0.9, 0.9, 0.9, 1};
	GLfloat diffuse_ground[] = {0.6, 0.6, 0.6, 1};
	GLfloat specular_ground[] = {0.6, 0.6, 0.6, 1};
	GLfloat shininess_ground = 5;
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_ground);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_ground);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_ground);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess_ground);
	
	glPushMatrix();
		glTranslatef(0, -1, 0);
		glScalef(100, 0, 40);
		glutSolidCube(1);
	glPopMatrix();
}

static void on_display(void) {
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 10, 20,
			  0, 10,  0,
			  0,  1,  0);

	//'Gasimo svetla' ako igra nije aktivna trenutno
	if (animation_ongoing) {
		light_on();
	} else {
		light_off();
	}
	
	
/*	glEnable(GL_TEXTURE_2D);*/
	
	//Iskljucujemo osvetljenje za 2D objekte
	glDisable(GL_LIGHTING);
		drawText();
		drawGifts();
		if (!animation_ongoing) {
			drawEndText();
		}
	glEnable(GL_LIGHTING);

	//I nakon paljenja iscrtavamo 3D
	drawGround();
	drawTrees();
	drawSnowmen();

	glutSwapBuffers();
}


