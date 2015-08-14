#ifndef CMDTREE_H_
#define CMDTREE_H_

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>

class CmdTree : public Fl_Tree {
public:
	CmdTree(int X,int Y,int W,int H,const char*L=0) : Fl_Tree(X,Y,W,H,L) { }
	int handle(int e) {
		if ( e == FL_PUSH ) { // Mouse click?
			const Fl_Tree_Item *item = find_clicked(); // item clicked
			if ( item ) { // valid item?
				callback_item((Fl_Tree_Item*)item); // set callback item
				callback_reason(FL_TREE_REASON_SELECTED); // set callback reason
				do_callback();
			}
		}
		return (Fl_Tree::handle(e));
	}
};

#endif // CMDTREE_H_
