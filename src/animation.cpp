/*
 * Animation.cpp
 *
 *  Created on: 9/09/2014
 *      Author: Elliot Wilde
 */

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "animation.h"
#include "define.h"
#include <gl/glut.h>

Animation::Animation(void (* callback) (float,float,float,float,float,float,float)) {
	xpoints = new float[4];
	zpoints = new float[4];
	keytimes = new float[4];
	starttimes = new float[4];
	maxsize = 4;
	keyframes = 0;
	update = callback;
	currentFrame = 0;
	currentTime = 0;
	timeInFrame = 0;
}

void Animation::add(float x, float z, float t){
	// ensure space available then add
	// should probably have some faculty for rotation, maybe later
	resize();
	xpoints[keyframes] = x;
	zpoints[keyframes] = z;
	keytimes[keyframes] = t;
	if (keyframes>0){
		starttimes[keyframes] = starttimes[keyframes-1] + t;
	} else {
		starttimes[keyframes] = 0;
	}
	keyframes++;

	printf("\nframes: %d\n", keyframes);
	for (int i=0; i<keyframes; ++i){
		printf("frame: %d \tx:%.1f\tz:%.1f\tft:%.1f\tst%.1f\n", i, xpoints[i], zpoints[i], keytimes[i], starttimes[i]);
	}

}

void Animation::remove(float x1, float z1){
	// TODO: remove the value nearest to the passed floats?
	// fail if there isn't one nearby?
	printf("anim->remove: x1:%.1f z1:%.1f\n", x1, z1);
	// find the first point within the square bounding +8,-8
	float bound = 8;
	int i;
	for (i=0; i<keyframes; ++i){
		float z = zpoints[i];
		if (z-bound < z1 && z+bound > z1){
			float x = xpoints[i];
			if (x-bound < x1 && x+bound > x1){
				break;
			}
		}
	}
	// if nothing was found, exit
	if (i==keyframes) {
		printf("Nothing found at (%.0f, %.0f)\n", x1, z1);
		return;
	}
	if (i==0 && keyframes==1){
		printf("Cannot remove the last keyframe\n");
		return;
	}
	--keyframes;
	float time = keytimes[i];
	for (;i<keyframes; ++i){
		xpoints[i] = xpoints[i+1];
		zpoints[i] = zpoints[i+1];
		keytimes[i] = keytimes[i+1];
		starttimes[i] = starttimes[i+1] - time;
	}

	for (int i=0; i<keyframes; ++i){
		printf("frame: %d \tx:%.1f\tz:%.1f\tft:%.1f\tst%.1f\n", i, xpoints[i], zpoints[i], keytimes[i], starttimes[i]);
	}

}

void Animation::move(float x1, float z1, float x2, float z2){
	// fail if there isn't one nearby?
	printf("anim->move: x1:%.1f x2:%.1f z1:%.1f z2:%.1f\n", x1, x2, z1, z2);
	// find the first point within the square bounding +8,-8
	float bound = 8;
	int i;
	for (i=0; i<keyframes; ++i){
		float z = zpoints[i];
		if (z-bound < z1 && z+bound > z1){
			float x = xpoints[i];
			if (x-bound < x1 && x+bound > x1){
				break;
			}
		}
	}
	// if nothing was found, exit
	if (i==keyframes) {
		printf("Nothing found at (%.0f, %.0f)\n", x1, z1);
		return;
	}
	// else we do stuff, muahahahaha
	printf("Moved (%.1f, %.1f) ", xpoints[i], zpoints[i]);
	xpoints[i] = x2;
	zpoints[i] = z2;
	printf("to (%.1f, %.1f)\n", xpoints[i], zpoints[i]);

}

void Animation::apply(){
	// need to interpolate
	// is there one frame behind, and two frames ahead? if so, do something, else don't
//	if (currentFrame > 0 && currentFrame < keyframes-1){
		update(getcurrentx(), 0, getcurrentz(), 0, 0, 0, 0);
//	}
}

void Animation::apply(float time){
	apply();
	next(time);
}

bool Animation::next(float time){
	currentTime += time;
	timeInFrame += time;
//	printf("itif:%.1f ", timeInFrame);
	// if we have moved onto the next frame
	// this will break if the keyframe is smaller than time
	if (timeInFrame >= keytimes[currentFrame]){
		++currentFrame;
		if (currentFrame==keyframes) {
			// TODO: wraparound currenttime, and timeinframe
			timeInFrame = timeInFrame - keytimes[currentFrame-1];
			currentFrame=0;
			currentTime = timeInFrame;

//			printf("ct:%.1f st:%.1f kt:%.1f tif:%.1ft\n", currentTime, starttimes[currentFrame], keytimes[currentFrame], timeInFrame);
			return true;
		}
		else {
			timeInFrame = currentTime - starttimes[currentFrame];
		}
	}
//		printf("ct:%.1f st:%.1f kt:%.1f f\n", currentTime, starttimes[currentFrame], keytimes[currentFrame]);
	return false;
//	printf("etif:%.1f\n", timeInFrame);


}

void Animation::gotoTime(float time){
	// set current time to zero, then apply(time)?
	currentTime = 0;
	next(time);

}

float spline(float u, float p0, float p1, float p2, float p3){
	return ((p3 - (3.*p2) + (3.*p1) - p0) * (u*u*u) +
			(-p3 + (4.*p2) - (5.*p1) + (2.*p0)) * (u*u) +
			(p2 - p0) * u + (2.*p1)) /2.;
}

float Animation::getcurrentx(){
	// TODO: wraparound?
	float p0 = xpoints[currentFrame-1], p1 = xpoints[currentFrame];
	float p2 = xpoints[currentFrame+1], p3 = xpoints[currentFrame+2];
	if (currentFrame == 0) {
		p0 = xpoints[keyframes-1];
	}
	if (currentFrame == (keyframes -2)){
		p2 = xpoints[currentFrame+1];
		p3 = xpoints[0];
	}
	if (currentFrame == (keyframes -1)){
		p2 = xpoints[0];
		p3 = xpoints[1];
	}

	float u = (1./keytimes[currentFrame])*timeInFrame;
//	printf("\nu:%.2f\tkt:%.2f\ttf:%.2f\t", u, keytimes[currentFrame], timeInFrame);
	return spline(u, p0, p1, p2, p3);
}

float Animation::getcurrentz(){
	float p0 = zpoints[currentFrame-1], p1 = zpoints[currentFrame];
	float p2 = zpoints[currentFrame+1], p3 = zpoints[currentFrame+2];
	if (currentFrame == 0) p0 = zpoints[keyframes-1];
	if (currentFrame == (keyframes -2)){
		p2 = zpoints[currentFrame+1];
		p3 = zpoints[0];
	}
	if (currentFrame == (keyframes -1)){
		p2 = zpoints[0];
		p3 = zpoints[1];
	}

	float u = (1./keytimes[currentFrame])*timeInFrame;
	return spline(u, p0, p1, p2, p3);
}

void Animation::draw(){
	// TODO: draw the current path
	// TODO: transform the x and z locations into a 320x360 window
	// store the current time and frame values
	float ctime = currentTime;
	float tframe = timeInFrame;
	int cframe = currentFrame;


	// draw the first frame
	int i=0;
	glColor3f(.2, .2, .8);
	glBegin( GL_QUADS );
		glVertex2f(xpoints[i], zpoints[i]-4);
		glVertex2f(xpoints[i]-4, zpoints[i]);
		glVertex2f(xpoints[i], zpoints[i]+4);
		glVertex2f(xpoints[i]+4, zpoints[i]);
	glEnd();
	// iterate between the second keyframe and the second to last
	for (i=1; i<keyframes-1; ++i){
		glColor3f(.8, .8, .8);
		glBegin( GL_QUADS );
			glVertex2f(xpoints[i], zpoints[i]-4);
			glVertex2f(xpoints[i]-4, zpoints[i]);
			glVertex2f(xpoints[i], zpoints[i]+4);
			glVertex2f(xpoints[i]+4, zpoints[i]);
		glEnd();
	}
	// then draw the last keyframe
	i=keyframes-1;
	glColor3f(.8, .2, .2);
	glBegin( GL_QUADS );
		glVertex2f(xpoints[i], zpoints[i]-4);
		glVertex2f(xpoints[i]-4, zpoints[i]);
		glVertex2f(xpoints[i], zpoints[i]+4);
		glVertex2f(xpoints[i]+4, zpoints[i]);
	glEnd();
	{
		// draw a doofah denoting where the animation is up to
		float x = getcurrentx();
		float z = getcurrentz();
		glColor3f(.2, .8, .8);
		glBegin( GL_QUADS );
			glVertex2f(x, z-4);
			glVertex2f(x-4, z);
			glVertex2f(x, z+4);
			glVertex2f(x+4, z);
		glEnd();

	}

	// now we draw the curves
	// TODO: fix
//	gotoTime(0);
	currentFrame = 0;
	currentTime =  0;
	timeInFrame =  0;
	glColor3f(.8, .8, .8);
//	printf("\nframe start:\n");
	bool loop = true;
	while (loop){
		float x1 =getcurrentx(), z1 = getcurrentz();
		if (next(.1)) loop=false;
		float x2 =getcurrentx(), z2 = getcurrentz();
//		printf("%d\t%.1f\t%.1f\t%.1f\t%.1f\n",currentFrame, x1, z1, x2, z2);
		if (x1 == -1 || x2 == -1 || z1 == -1 || z2 == -1) continue;
		glBegin( GL_LINES );
			glVertex2f(x1, z1);
			glVertex2f(x2, z2);
		glEnd();
	}
//	printf("\n frame end\n\n");



	// restore the current time and frame values;
	currentFrame = cframe;
	currentTime = ctime;
	timeInFrame = tframe;
}

void Animation::draw_Colour(){
	// TODO: draw the current path using unique colours, so one can be selected?
	// or de we not want to bother with colour picking?

}

void Animation::resize(){
	// FIXED | is this breaking when it resizes? yes it is
	if (keyframes==maxsize){
		int oldsize = maxsize;
		maxsize*=2;
		float* nxp = new float[maxsize];
		memcpy(nxp, xpoints, oldsize * sizeof(float) );
		delete[] xpoints;
		xpoints = nxp;

		float* nzp = new float[maxsize];
		memcpy(nzp, zpoints, oldsize * sizeof(float) );
		delete[] zpoints;
		zpoints = nzp;

		float* nkt = new float[maxsize];
		memcpy(nkt, keytimes, oldsize * sizeof(float) );
		delete[] keytimes;
		keytimes = nkt;

		float* nst = new float[maxsize];
		memcpy(nst, starttimes, oldsize * sizeof(float) );
		delete[] starttimes;
		starttimes = nst;
	}

}


