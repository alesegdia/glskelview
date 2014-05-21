#pragma once

#include "../commonfltk.h"

#include "../constants.h"
#include "glplayback.h"


class FlApp : public Fl_Double_Window {

private:

	enum AppState {
		CONSOLE, GUI, NAVIGATE
	};

	GLPlayback* glp;
	Fl_Value_Slider *xslid,*yslid,*zslid;
	Fl_Button *bx1,*bx2,*by1,*by2,*bz1,*bz2;
	Fl_Button *animplay, *animstop, *animpause;
	Fl_Browser* browser;
	Fl_Group *slidersGroup;

	Entity* selectedEntity;
	Skeleton::Joint** selectedJoint;

	char* shellbuffer;
	int nextCmdBufferChar;
	AppState estado;

public:
	FlApp (int W, int H, const char* L=0)
		: Fl_Double_Window(W,H,L)
	{
		nextCmdBufferChar=0;
		estado=AppState::GUI;

		selectedEntity = NULL;
		glp = new GLPlayback(w()/4,0,3*w()/4,h()-90);
		selectedJoint = glp->GetSelectedPPtr();
		shellbuffer = new char[256];

        browser = new Fl_Browser(0,0,w()/4,h()-90);
        browser->callback(BrowserChanged_CB, ((void*)this));
        browser->type(FL_MULTI_BROWSER);
        glp->SetBrowser(browser);
        glp->onLoad();
        browser->end();

        xslid = new Fl_Value_Slider(w()/4, h()-90, 3*w()/4, 20, "Pose.X");
        xslid->align(FL_ALIGN_LEFT);
        xslid->type(FL_HOR_SLIDER);
        xslid->bounds(0.0, 0.0);
        xslid->value(0.0);
        xslid->callback(ValueChangedX_CB, ((void*)this));

        yslid = new Fl_Value_Slider(w()/4, h()-60, 3*w()/4, 20, "Pose.Y");
        yslid->align(FL_ALIGN_LEFT);
        yslid->type(FL_HOR_SLIDER);
        yslid->bounds(0.0, 0.0);
        yslid->value(0.0);
        yslid->callback(ValueChangedY_CB, ((void*)this));

        zslid = new Fl_Value_Slider(w()/4, h()-30, 3*w()/4, 20, "Pose.Z");
        zslid->align(FL_ALIGN_LEFT);
        zslid->type(FL_HOR_SLIDER);
        zslid->bounds(0.0, 0.0);
        zslid->value(0.0);
        zslid->callback(ValueChangedZ_CB, ((void*)this));

        animplay = new Fl_Button(100,h()-90,60,20,"PLAY");
        animplay->callback(AnimPlay_CB, ((void*)this));
        animstop = new Fl_Button(100,h()-60,60,20,"STOP");
        animstop->callback(AnimStop_CB, ((void*)this));

		Fl_Value_Slider* sliders[3] = {xslid,yslid,zslid};
		glp->SetSliders(sliders);

       	bx1 = new Fl_Button(20,h()-90,20,20,"-");
        bx1->callback(EntityMoveX1_CB, ((void*)this));
        bx2 = new Fl_Button(60,h()-90,20,20,"+");
        bx2->callback(EntityMoveX2_CB, ((void*)this));
        by1 = new Fl_Button(20,h()-60,20,20,"-");
        by1->callback(EntityMoveY1_CB, ((void*)this));
        by2 = new Fl_Button(60,h()-60,20,20,"+");
        by2->callback(EntityMoveY2_CB, ((void*)this));
        bz1 = new Fl_Button(20,h()-30,20,20,"-");
        bz1->callback(EntityMoveZ1_CB, ((void*)this));
        bz2 = new Fl_Button(60,h()-30,20,20,"+");
        bz2->callback(EntityMoveZ2_CB, ((void*)this));

		Fl_Group *boxgroup = new Fl_Group(30,h()-90,100,100);
        Fl_Box* boxX = new Fl_Box(40,h()-90,20,20);
        Fl_Box* boxY = new Fl_Box(40,h()-60,20,20);
        Fl_Box* boxZ = new Fl_Box(40,h()-30,20,20);
        boxX->label("X");
        boxY->label("Y");
        boxZ->label("Z");
        boxgroup->end();

        //resizable(boxgroup);

		end();

	}

	virtual ~FlApp () { delete [] shellbuffer; }


	void ParseCommand();

	/* FlApp INTERFACE ****************************/
	int handle(int event);
	void resize(int X, int Y, int W, int H);


#define DECL_CALLBACK_HELPER(cbname) static void cbname (Fl_Widget* w, void* userdata) { \
				FlApp *appwin = (FlApp*)userdata; \
				appwin->cbname##2(); \
			}

	DECL_CALLBACK_HELPER(AnimPlay_CB)
	DECL_CALLBACK_HELPER(AnimStop_CB)

	DECL_CALLBACK_HELPER(EntityMoveX1_CB)
	DECL_CALLBACK_HELPER(EntityMoveX2_CB)
	DECL_CALLBACK_HELPER(EntityMoveY1_CB)
	DECL_CALLBACK_HELPER(EntityMoveY2_CB)
	DECL_CALLBACK_HELPER(EntityMoveZ1_CB)
	DECL_CALLBACK_HELPER(EntityMoveZ2_CB)

	DECL_CALLBACK_HELPER(ValueChangedX_CB)
	DECL_CALLBACK_HELPER(ValueChangedY_CB)
	DECL_CALLBACK_HELPER(ValueChangedZ_CB)

	void AnimPlay_CB2();
	void AnimStop_CB2();
	void EntityMoveX1_CB2();
	void EntityMoveX2_CB2();
	void EntityMoveY1_CB2();
	void EntityMoveY2_CB2();
	void EntityMoveZ1_CB2();
	void EntityMoveZ2_CB2();

	void ValueChangedX_CB2();
	void ValueChangedY_CB2();
	void ValueChangedZ_CB2();

	static void BrowserChanged_CB (Fl_Widget* w, void* userdata);
	void BrowserChanged_CB2 (Entity* e);

};



