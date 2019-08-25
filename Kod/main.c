#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>


#define TIMER_ID 0
#define TIMER_INTERVAL 30

#define MAX_GIFTS 5

static int animation_ongoing;

static GLuint textureNames[1];

typedef struct gift{
	float x, y;
// 	float rotation;
    
}Gift;



Gift gifts[MAX_GIFTS];


int width, height;


void throwGift();
void updateGifts();
void drawGifts();
void resetGame();


void light_init(
 	GLfloat* light_ambient,
 	GLfloat* light_diffuse,
 	GLfloat* light_specular
);


float dropPosition = 0;
int currentGift = 0;



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

void resetGame(){
	for (int i=0; i<MAX_GIFTS; i++){
		gifts[i].x = 0;
		gifts[i].y = 20;
	}
}



void updateGifts() {

    
	for (int i=0; i<MAX_GIFTS; i++){
            
		//Ako poklon nije bacen on je na visini 20 i prati poziciju misa
		if (gifts[i].y >= 20) {
			gifts[i].x = dropPosition;
		}
		
		else {

                    gifts[i].y -= 0.3;
		}
		
		if (gifts[i].y < 0) {
                    gifts[i].x = dropPosition;
			gifts[i].y = 20;
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



static void on_timer(int value) {
    
    //     Proverava se da li callback dolazi od odgovarajuceg tajmera.
        if (value != TIMER_ID) {
            return;
        }

	updateGifts();

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
	
	
	glEnable(GL_TEXTURE_2D);
	
	//Iskljucujemo osvetljenje za 2D objekte
 	glDisable(GL_LIGHTING);
 		
        drawGifts();

 	glEnable(GL_LIGHTING);

        drawGifts();
	//I nakon paljenja iscrtavamo 3D
 	drawGround();
 	drawTrees();


	glutSwapBuffers();
}


