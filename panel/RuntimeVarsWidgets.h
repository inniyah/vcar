#ifndef RUNTIMEVARSWIDGETS_H_
#define RUNTIMEVARSWIDGETS_H_

#include "config.h"

#include "CarState.h"

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>

class RtVarsTree : public Fl_Tree, public ICarStateListener {
public:
	RtVarsTree(int X, int Y, int W, int H, const char * L = 0);
	virtual ~RtVarsTree();

	virtual void draw(void);

	void buildTree();

	void setCarState(CarState * car_state);

	virtual void eventCarStateChanged(void); // ICarStateListener

private:
	CarState * p_CarState;
};

#endif // RUNTIMEVARSWIDGETS_H_
