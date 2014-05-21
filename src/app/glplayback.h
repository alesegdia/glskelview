#pragma once

#include "../commonfltk.h"

#include "../scene/camera.h"
#include "../scene/entity.h"
#include "../scene/entityfactory.h"
#include "../scene/world.h"

class GLPlayback : public Fl_Gl_Window {

private:

	struct AppStateConfig {

		void Clear();

		bool wireframe;
		bool lock;
		bool prevLMBclicked;

		int numChannel;

	};

	Camera::InputState camInputState;
	Camera Cam;
	Skeleton::Joint* selectedJoint;
	EntityFactory* efactory;
	Fl_Value_Slider* sliders[3];
	Fl_Browser* browser;

	AppStateConfig stateCfg;

	int numChannel;

	World world;

	static constexpr float FPS = 60.0;


public:

	GLPlayback (int x, int y, int w, int h, const char* label=0)
		: Fl_Gl_Window(x,y,w,h,label) {
			mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
			stateCfg.Clear();
			camInputState.Clear();
			selectedJoint = NULL;
			stateCfg.numChannel = 6;
			Fl::add_timeout(1.0/FPS, Timer_CB, (void*)this);
		}

	virtual ~GLPlayback ();

private:

	void InitGL();

public:

	void onLoad();
	void onLoop(double delta);
	void onRender();
	void onCleanup();

	void draw();
	void resize(int x, int y, int w, int h);

	static void Timer_CB(void *userdata);

public:

	void DecChannel();
	void IncChannel();

// private

	static int jump_cursor (int new_x, int new_y);

	// Fl_Gl_Window interface
	void UpdateWASD(char key, int state);
	void MakeEntity(const char* enttype, const char* name) {
		if(strcmp(enttype,"test")==0) efactory->MakePersona(name);
		if(strcmp(enttype,"mosca")==0) efactory->MakeMosca(name);
		if(strcmp(enttype,"animmosca")==0) efactory->MakeAnimMosca(name);
		if(strcmp(enttype,"tubo")==0) efactory->MakeTubo(name);
		if(strcmp(enttype,"dragon")==0) efactory->MakeDragon(name);
	}
	void SetCamSpeed(float value) {
		Cam.SetSpeed(value);
	}

	int handle(int event);
	void FixViewport(int w, int h);


	void handleMouse();
	void SwitchLock() {
		stateCfg.lock = !stateCfg.lock;
		if(stateCfg.lock) cursor(FL_CURSOR_NONE);
		else cursor(FL_CURSOR_ARROW);
	}

	Skeleton::Joint** GetSelectedPPtr()
	{
		return &selectedJoint;
	}

	void DeleteEntity(const char* tag) {
		if(world.Delete(tag)) {
			printf("Deleting entity: '%s'.",tag); fflush(0);
		} else {
			printf("FAILED DELETION: '%s'.",tag); fflush(0);
		}
	}

	void SetSliders(Fl_Value_Slider* sliders[3]) {
		for(int i=0; i<3;i++) {
			this->sliders[i]=sliders[i];
			this->sliders[i]->bounds(0,0);
		}
	}

	void OnSelectedJoint() {
		if(selectedJoint) {
			sliders[0]->bounds(
				selectedJoint->BXLimit(),
				selectedJoint->TXLimit());
			sliders[1]->bounds(
				selectedJoint->BYLimit(),
				selectedJoint->TYLimit());
			sliders[2]->bounds(
				selectedJoint->BZLimit(),
				selectedJoint->TZLimit());

			sliders[0]->value(selectedJoint->GetPose().x);
			sliders[1]->value(selectedJoint->GetPose().y);
			sliders[2]->value(selectedJoint->GetPose().z);
		}
	}

	void SetBrowser(Fl_Browser* browser) {
		this->browser=browser;
	}

	void SwitchWireframe();
	void ConstructRay(Ray& ray);

};


