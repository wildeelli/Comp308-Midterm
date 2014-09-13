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
	// constructor, takes a pointer to a function that takes 7 floats, location(x, y, z), and rotation(r, x, y, z)
	Animation(void (*) (float, float, float, float, float, float, float));
	void add(float, float, float);
	void remove(float, float);
	void move(float, float, float, float);
	void apply();
	// calls apply, then calls next(float)
	void apply(float);
	// sets the animation time forwards by the specifed number of seconds
	bool next(float);
	void gotoTime(float);
	void draw();
	void draw_Colour();
private:
	float* xpoints;
	float* zpoints;
	float* keytimes;
	float* starttimes;
	// the number of keyframes in the animation
	int keyframes;
	// the maximum number of keyframes able to be stored before the two arrays need resizing
	int maxsize;

	float currentTime;
	float timeInFrame;
	int currentFrame;

	void (* update) (float, float, float, float, float, float, float);
	float getcurrentx();
	float getcurrentz();

	// call every time something is added, does not necessarily mean it will do anything though
	void resize();

};

#endif /* ANIMATION_H_ */
