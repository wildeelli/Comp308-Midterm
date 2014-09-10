/*
 * Animation.cpp
 *
 *  Created on: 9/09/2014
 *      Author: Elliot Wilde
 */

#include <cstdlib>
#include "animation.h"
#include "define.h"
#include <gl/glut.h>

Animation::Animation(void (* callback) (float,float,float,float,float,float,float)) {
	// TODO Auto-generated constructor stub
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
	starttimes[keyframes] = starttimes[keyframes-1] + t;
	keyframes++;
}

void Animation::remove(float, float){
	// TODO: remove the value nearest to the passed floats?
	// fail if there isn't one nearby?

}

void Animation::move(float, float, float, float){
	// TODO: move the value closest to the first two passed floats to the second two passed floats?
	// fail if there isn't one nearby?

}

void Animation::apply(){
	// need to interpolate
	// is there one frame behind, and two frames ahead? if so, do something, else don't
	if (currentFrame > 0 && currentFrame < keyframes-2){
		// get some values out and store them so that the maths isn't thousands of lines long, just hundreds
		float p0 = xpoints[currentFrame-1], p1 = xpoints[currentFrame];
		float p2 = xpoints[currentFrame+1], p3 = xpoints[currentFrame+2];
		float u = timeInFrame;
		float xmove = ((p3 - 3.*p2 + 3.*p1 - p0) * (u*u*u) +
				(-p3 + 4.*p2 - 5.*p1 + 2.*p0) * (u*u) +
				(p2 - p0) + (2.*p1)) /2.;
		// and now we do it all again for the z-motion
		p0 = zpoints[currentFrame-1], p1 = zpoints[currentFrame];
		p2 = zpoints[currentFrame+1], p3 = zpoints[currentFrame+2];
		float zmove = ((p3 - 3.*p2 + 3.*p1 - p0) * (u*u*u) +
				(-p3 + 4.*p2 - 5.*p1 + 2.*p0) * (u*u) +
				(p2 - p0) + (2.*p1)) /2.;
		// and now, we pray
		update(getcurrentx(), 0, getcurrentz(), 0, 0, 0, 0);
	}
}

void Animation::apply(float time){
	apply();
	next(time);
}

void Animation::next(float time){
	currentTime += time;
	for (int i=0; i<keyframes; ++i){
		if (currentTime > starttimes[i]){
			currentFrame = i;
			timeInFrame = currentTime - starttimes[i];
		}
	}

}

void Animation::gotoTime(float time){
	// set current time to zero, then apply(time)?
	currentTime = 0;
	next(time);

}

float Animation::getcurrentx(){
	// TODO: wraparound?
	if (currentFrame > 0 && currentFrame < keyframes-2){
		float p0 = xpoints[currentFrame-1], p1 = xpoints[currentFrame];
		float p2 = xpoints[currentFrame+1], p3 = xpoints[currentFrame+2];
		float u = timeInFrame;
		return ((p3 - 3.*p2 + 3.*p1 - p0) * (u*u*u) +
				(-p3 + 4.*p2 - 5.*p1 + 2.*p0) * (u*u) +
				(p2 - p0) + (2.*p1)) /2.;
	}
	return 0;
}
float Animation::getcurrentz(){
	// TODO: wraparound?
	if (currentFrame > 0 && currentFrame < keyframes-2){
		float p0 = zpoints[currentFrame-1], p1 = zpoints[currentFrame];
		float p2 = zpoints[currentFrame+1], p3 = zpoints[currentFrame+2];
		float u = timeInFrame;
		return ((p3 - 3.*p2 + 3.*p1 - p0) * (u*u*u) +
				(-p3 + 4.*p2 - 5.*p1 + 2.*p0) * (u*u) +
				(p2 - p0) + (2.*p1)) /2.;
	}
	return 0;
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

	// now we draw the curves
	// TODO: fix
	float t = starttimes[1];
	gotoTime(0);
	glColor3f(.8, .8, .8);
	while (currentTime < starttimes[keyframes-2]){
		float x1 =getcurrentx(), z1 = getcurrentz();
		next(.1);
		float x2 =getcurrentx(), z2 = getcurrentz();
		glBegin( GL_LINES );
			glVertex2f(x1, z1);
			glVertex2f(x2, z2);
		glEnd();
	}



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
	if (keyframes==maxsize){
		maxsize*=2;
		realloc(xpoints, maxsize);
		realloc(zpoints, maxsize);
		realloc(keytimes, maxsize);
		realloc(starttimes, maxsize);
	}

}
