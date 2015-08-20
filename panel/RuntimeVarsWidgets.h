#ifndef RUNTIMEVARSWIDGETS_H_
#define RUNTIMEVARSWIDGETS_H_

#include "config.h"

#include "CarState.h"

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>

class RtVarsTree : public Fl_Tree {
public:
	RtVarsTree(int X,int Y,int W,int H,const char*L=0) : Fl_Tree(X,Y,W,H,L), p_CarState(NULL) { }
	virtual ~RtVarsTree() { }

	virtual void draw(void);

	void buildTree();

	void setCarState(CarState * car_state);

private:
	CarState * p_CarState;
};

#endif // RUNTIMEVARSWIDGETS_H_
