/*
 * Animation.cpp
 *
 *  Created on: 9/09/2014
 *      Author: Bandit
 */

#include "animation.h"

Animation::Animation(void* animateeee) {
	// TODO Auto-generated constructor stub
	xpoints = new float[4];
	ypoints = new float[4];
	maxsize = 4;
	keyframes = 0;

}

void Animation::add(float, float){
	resize();
}

void Animation::remove(float, float){

}

void Animation::move(float, float, float, float){

}

float* Animation::next(int){
	return 0;
}

void Animation::draw(){

}

void Animation::draw_Colour(){

}

void Animation::resize(){

}