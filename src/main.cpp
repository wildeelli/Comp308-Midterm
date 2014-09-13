/*
 * main.cpp
 *
 *  Created on: 19/08/2014
 *      Author: Elliot Wilde
 */

#include <stdio.h>
#include <stdlib.h>
#include <gl/glut.h>
#include "define.h"
#include "animation.h"

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

GLuint g_paneWidth = G308_PANE_WIDTH;
GLuint g_paneHeight = G308_PANE_HEIGHT;

void G308_keyboardListener(unsigned char, int, int);
void G308_Reshape(int w, int h);
void G308_display();
void G308_SetCamera();
void G308_SetOrtho();
void G308_SetLight();

void G308_ColourDraw();


void upd(float, float, float, float, float, float, float);

void mouseFunc(int, int, int, int);
bool leftdown=false, rightdown=false, middledown=false;
// position where left/right/middle mouse button was pressed
GLuint ldx, ldy, rdx, rdy, mdx, mdy;
// functions that get called on mouse up
void mClickRelease(unsigned int, unsigned int);
void lClickRelease(unsigned int, unsigned int);
void rClickRelease(unsigned int, unsigned int);

Animation* anim;

float tx=0, tz=0;
bool playing=false;

void idle() {
	glutPostRedisplay();

}

int main(int argc, char** argv){
	// check command line args

	// init glut
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(g_nWinWidth, g_nWinHeight);
	g_mainWnd = glutCreateWindow("COMP308 MidTerm Project");
	
	glutKeyboardFunc(G308_keyboardListener);
	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouseFunc);

	G308_SetCamera();
	G308_SetLight();

	anim = new Animation(upd);
	anim->add(163, 47, 1);
	anim->add(254, 95, 1);
	anim->add(277, 183, 1);
	anim->add(243, 279, 1);
	anim->add(164, 320, 1);
	anim->add(66, 264, 1);
	anim->add(43, 174, 1);
	anim->add(85, 78, 1);




	glutMainLoop();

	return 0;
}

void drawFloor(){
	for (int i=-10; i<10; ++i){
		for (int j=-10; j<10; j++){
			glPushMatrix();
				glTranslatef(i,0,j);
				if (abs(i+j)%2)
					glColor3f(.5,.5,.5);
				else
					glColor3f(.2,.2,.2);
				glBegin(GL_QUADS);
				glNormal3f(0,1,0);
					glVertex3f(0,0,0);
					glNormal3f(0,1,0);
					glVertex3f(1,0,0);
					glNormal3f(0,1,0);
					glVertex3f(1,0,1);
					glNormal3f(0,1,0);
					glVertex3f(0,0,1);
				glEnd();
			glPopMatrix();
		}
	}
}

float rot=0;

void G308_display(){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}
	
	//drawing stuff goes here
	// store the viewport matrix, then draw the floor
	G308_SetCamera();
	glPushMatrix();
	drawFloor();
	glPushMatrix();
	glTranslatef(0,1,0);
	glPushMatrix();
	// draw a spinning thing to test drawing
	glScalef(.1,.1,.1);
	if (playing) anim->apply(1./60.);
	glTranslated(tx, 0, tz);
//	glRotatef(rot+=5, .5,.8, -.5);
//	glRotatef(-rot, .0,-.8, -.5);
	for (int i=0; i<6; ++i){
		glColor3f(.7,.7,1./float(i));
		// glColor3f(.8,.8,.8);
		glPushMatrix();
		glRotatef(60*i, 0,0,1);
		glBegin( GL_TRIANGLES );
			glNormal3f(0,0,1);
			glVertex3f(0,0,1);
			glNormal3f(-0.5773,1,0);
			glVertex3f(-0.5773,1,0);
			glNormal3f(0.5773,1,0);
			glVertex3f(0.5773,1,0);
		glEnd();
		glPopMatrix();
	}
	// aaaand end of the spinning thing
	glPopMatrix();
	glPopMatrix();
	
	// clear the viewport matrix
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	G308_SetOrtho();
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// now draw the two panes on the right
	// draw the top pane
	glPushMatrix();
	glTranslatef(g_nWinWidth-g_paneWidth,0,0);
	glColor4f(.2, .2, .2, 0.8);
	glBegin( GL_QUADS );
		glVertex2f(0, 0);
		glVertex2f(0, g_paneHeight);
		glVertex2f(g_paneWidth, g_paneHeight);
		glVertex2f(g_paneWidth, 0);
	glEnd();
	glColor4f(.8, .8, .8, 1.0);
	glBegin( GL_LINE_LOOP );
		glVertex2f(1, 1);
		glVertex2f(1, g_paneHeight);
		glVertex2f(g_paneWidth, g_paneHeight);
		glVertex2f(g_paneWidth, 1);
	glEnd();
	// do stuff on the top pane
	glPopMatrix();

	// draw the bottom pane
	glPushMatrix();
	glTranslatef(g_nWinWidth-g_paneWidth,g_paneHeight,0);
	glColor4f(.2, .2, .2, 0.8);
	glBegin( GL_QUADS );
		glVertex2f(0, 0);
		glVertex2f(0, g_paneHeight);
		glVertex2f(g_paneWidth, g_paneHeight);
		glVertex2f(g_paneWidth, 0);
	glEnd();
	glColor4f(.8, .8, .8, 1.0);
	glBegin( GL_LINE_LOOP );
		glVertex2f(1, 1);
		glVertex2f(1, g_paneHeight);
		glVertex2f(g_paneWidth, g_paneHeight);
		glVertex2f(g_paneWidth, 1);
	glEnd();
	// do stuff on the bottom pane
	glPopMatrix();
	glPushMatrix();
		glTranslatef(g_nWinWidth-g_paneWidth,0,0);
	anim->draw();
	glPopMatrix();




	// and ends here
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

// Draws all selectable elements on the screen using their colour-pickable shapes and colours
void G308_ColourDraw(){
	// TODO: this

}

void mouseFunc(int button, int state, int x, int y){
	// change the currently stored mouse states
	// then call a function if the mouse was lifted
	// printf("%d %d %d %d\n", button, state, x, y);
	if (button==GLUT_LEFT_BUTTON){
		if (state==GLUT_DOWN) {
			leftdown = true;
			ldx = x; ldy = y;
		}
		else if (state==GLUT_UP) lClickRelease(x, y);
	} else if (button==GLUT_RIGHT_BUTTON){
		if (state==GLUT_DOWN) {
			rightdown = true;
			rdx = x; rdy = y;
		}
		else if (state==GLUT_UP) rClickRelease(x, y);
	} else if (button==GLUT_MIDDLE_BUTTON){
		if (state==GLUT_DOWN) {
			middledown = true;
			mdx = x; mdy = y;
		}
		else if (state==GLUT_UP) mClickRelease(x, y);
	}
	// printf("left:%d %d %d\tright:%d %d %d\tmiddle:%d %d %d\n", leftdown,ldx,ldy, rightdown,rdx,rdy, middledown,mdx,mdy);

}

void rClickRelease(unsigned int x, unsigned int y){
	if (!rightdown) fprintf(stderr, "Something went wrong: Right Mouse released when not down\n");
	rightdown=false;
	if (rdx > g_nWinWidth - g_paneWidth && rdx < g_nWinWidth && rdy > 0 && rdy < g_paneHeight){
		anim->remove(rdx - (g_nWinWidth - g_paneWidth), rdy);
	}
}

void lClickRelease(unsigned int x, unsigned int y){
	if (!leftdown) fprintf(stderr, "Something went wrong: Left Mouse released when not down\n");
	leftdown=false;
//	printf("%d %d\n", x, y);
	if (x > g_nWinWidth - g_paneWidth && x < g_nWinWidth && y > 0 && y < g_paneHeight){
		int mx = x - (g_nWinWidth - g_paneWidth);
		int my = y;
		anim->add(mx, my, 1);
	}
	// work out where we released, and how close it was the leftdown
	// then do stuff with that information
}

void mClickRelease(unsigned int x, unsigned int y){
	if (!middledown) fprintf(stderr, "Something went wrong: Middle Mouse released when not down\n");
	middledown=false;
	if (x > g_nWinWidth - g_paneWidth && x < g_nWinWidth && y > 0 && y < g_paneHeight &&
			mdx > g_nWinWidth - g_paneWidth && mdx < g_nWinWidth && mdy > 0 && mdy < g_paneHeight){
		anim->move(mdx - (g_nWinWidth - g_paneWidth), mdy, x - (g_nWinWidth - g_paneWidth), y);
	}
}

// this should update the position of the animatable object
void upd(float x, float y, float z, float r, float rx, float ry, float rz){
	// convert the motion from animation space to world space
	tx = (20./g_paneWidth)*x - 10.;
	tz = (20./g_paneHeight)*z - 10.;
//	printf("tx: %.2f, tz: %.2f\n", tx, tz);
}

void G308_Reshape(int w, int h) {
	if (w>0) g_nWinWidth = w;
	if (h>0) g_nWinHeight = h;
	// if you try and resize the window below a certain size, it snaps it to that size
	if (w<640) {
		fprintf(stderr, "window too small, resizing\n");
		g_nWinWidth = 640;
		glutReshapeWindow(g_nWinWidth, g_nWinHeight);
		return;
	}
	if (h<720) {
		fprintf(stderr, "window too small, resizing\n");
		g_nWinHeight = 720;
		glutReshapeWindow(g_nWinWidth, g_nWinHeight);
		return;
	}
	
	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
	G308_SetCamera();
}

void G308_keyboardListener(unsigned char key, int x, int y) {
	if (key==' ' || key =='p') playing = !playing;
	(void)(x); (void)(y);
}

void G308_SetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// if (anim)
		// gluLookAt(0.0, 2.0, 7.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);
	// else
//		gluLookAt(0.0, 10.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0);
//	gluLookAt(6.0, 4.0, 6.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(0.0, 25.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);
}

void G308_SetOrtho(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, g_nWinWidth, g_nWinHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void G308_SetLight() {
	float direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float diffintensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}
