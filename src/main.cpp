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

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

GLuint g_paneWidth = 320;
GLuint g_paneHeight = 360;

void G308_Reshape(int w, int h);
void G308_display();
void G308_SetCamera();
void G308_SetOrtho();
void G308_SetLight();

void G308_ColourDraw();

void mouseFunc(int, int, int, int);
bool leftdown=false, rightdown=false, middledown=false;
// position where left/right/middle mouse button was pressed
int ldx, ldy, rdx, rdy, mdx, mdy;
// functions that get called on mouse up
void mClickRelease(int, int);
void lClickRelease(int, int);
void rClickRelease(int, int);

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
	
	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouseFunc);

	G308_SetCamera();
	G308_SetLight();


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
	glRotatef(rot+=5, .5,.8, -.5);
	glRotatef(-rot, .0,-.8, -.5);
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
	glTranslatef(g_nWinWidth-g_paneWidth,g_nWinHeight-g_paneHeight,0);
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




	// and ends here
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

// Draws all selectable elements on the screen using their colour-pickable shapes and colours
void G308_ColourDraw(){


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

void rClickRelease(int x, int y){
	rightdown=false;

}

void lClickRelease(int x, int y){
	leftdown=false;
}

void mClickRelease(int x, int y){
	middledown=false;
}

void G308_Reshape(int w, int h) {
	if (w>0) g_nWinWidth = w;
	if (h>0) g_nWinHeight = h;
	// if you try and resize the window below a certain size, it snaps it to that size
	if (w<640) {
		printf("window too small, resizing\n");
		g_nWinWidth = 640;
		glutReshapeWindow(g_nWinWidth, g_nWinHeight);
		return;
	}
	if (h<720) {
		printf("window too small, resizing\n");
		g_nWinHeight = 720;
		glutReshapeWindow(g_nWinWidth, g_nWinHeight);
		return;
	}
	
	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
	G308_SetCamera();
//	g_paneHeight = g_nWinHeight/2;
//	g_paneWidth = 320;
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
	gluLookAt(0.0, 2.0, 7.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);
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