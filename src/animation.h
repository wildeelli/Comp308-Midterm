/*
 * Animation.h
 *
 *  Created on: 9/09/2014
 *      Author: Elliot Wilde
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

class Animation {
public:
	// replace void* with something appropriate
	Animation(void (*) (float, float, float, float, float, float));
	void add(float, float);
	void remove(float, float);
	void move(float, float, float, float);
	float* next(int);
	float* gotoTime(float);
	void draw();
	void draw_Colour();
private:
	float* xpoints;
	float* ypoints;
	// the number of keyframes in the animation
	int keyframes;
	// the maximum number of keyframes able to be stored before the two arrays need resizing
	int maxsize;

	void (* update) (float, float, float, float, float, float);

	// call every time something is added, does not necessarily mean it will do anything though
	void resize();

};

#endif /* ANIMATION_H_ */
