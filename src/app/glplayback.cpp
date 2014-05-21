#include "glplayback.h"
#include <iostream>
#include <cmath>
#include "../physics/ray.h"
#include "../scene/entity.h"

GLPlayback::~GLPlayback()
{
	onCleanup();
}

void GLPlayback::InitGL()
{
	glEnable(GL_DEPTH_TEST);

	glPointSize(4);
	glLineWidth(2);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { .0f, 1.0, .0f, 0.0 };
	GLfloat light_position1[] = { -1.0, 1.0, 1.0, 0.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);

	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

}

void GLPlayback::onLoad()
{
	camInputState.forward=camInputState.backward=camInputState.strafeRight=camInputState.strafeLeft=false;
	stateCfg.lock = false;

	// load assets
	Entity* e;
	efactory = new EntityFactory(&world,browser);

	/*
	e = efactory->MakePersona("canduterio");
	browser->add(e->GetTag(),e);
	e = efactory->MakePersona("canduterio2");
	browser->add(e->GetTag(),e);
	e = efactory->MakeDragon("dragoncillo");
	browser->add(e->GetTag(),e);
	e = efactory->MakeMosca("la_mosca");
	browser->add(e->GetTag(),e);
	*/
	//e = efactory->MakeTubo("er_tubito");
	//e = efactory->MakeMosca("avispa");
	e = efactory->MakeAnimMosca("avispa");
	//e = efactory->MakeDragon( "er dragon");

	//browser->add(e->GetTag(),e);
}

void GLPlayback::onLoop(double delta)
{

	// posibles errores de opengl
	GLenum glerr=glGetError();
	switch(glerr) {
		case GL_NO_ERROR:
			break;
		default:
		printf("glError: %s\n",gluErrorString(glerr));
	}

	// actualizar sliders con joint seleccionado
	if(selectedJoint) {
		OnSelectedJoint();
	} else {
		sliders[0]->bounds(0,0);
		sliders[1]->bounds(0,0);
		sliders[2]->bounds(0,0);
	}


	if(stateCfg.lock) {
		// calcular offset desde el centro y resetear raton
		int cursorx_abs, cursory_abs;
		int cursorx_center, cursory_center;
		cursorx_abs = Fl::event_x_root();
		cursory_abs = Fl::event_y_root();
		cursorx_center = x_root() + ( w() >> 1 );
		cursory_center = y_root() + ( h() >> 1 );
		jump_cursor(cursorx_center, cursory_center);

		// calcular step y avanzar la camara
		camInputState.rotationX = (cursorx_abs - cursorx_center);
		camInputState.rotationY = (cursory_abs - cursory_center);
		Cam.HandleInput( camInputState );
	}

	Cam.Update();

	for(int i=0;i<world.Size();i++)
		world[i]->Update(delta);

	// raycast desde el centro de la camara hacia delante
	if (stateCfg.lock)
	{
		Ray ray;
		ray.Origin.Set(Cam.GetPosition());
		ray.Origin.Negate();
		ray.Dir.Set(Cam.GetForward());

		bool LMBclicked = Fl::event_button1();
		if(LMBclicked && stateCfg.prevLMBclicked) {
			LMBclicked = false;
		}
		//stateCfg.prevLMBclicked = LMBclicked;
		else if(LMBclicked) stateCfg.prevLMBclicked=true;
		else stateCfg.prevLMBclicked = false;

		for(int i=0;i<world.Size();i++) {
			Entity* e = world[i];
			Skeleton::Joint* j = e->SkelRayTest(ray);
			if(j != NULL) {
				// ha sido seleccionado uno!
				if(LMBclicked) {
					if(selectedJoint==j) {
						// ya seleccionado, cambiar estado
						selectedJoint->SwitchSelected();
						if(!selectedJoint->IsSelected()) selectedJoint = NULL;
					} else {
						// joint nuevo, deseleccionamos el que habia
						if(selectedJoint) selectedJoint->Deselect();
						selectedJoint = j;
						j->SwitchSelected();
					}
					if(selectedJoint) OnSelectedJoint();
				}
			}
		}
	}
}

void GLPlayback::UpdateWASD(char key, int state) {

	switch(key) {
	case 'w': camInputState.forward=state; break;
	case 'a': camInputState.strafeLeft=state; break;
	case 's': camInputState.backward=state; break;
	case 'd': camInputState.strafeRight=state; break;
	case 'c': camInputState.flyDown=state; break;
	case ' ': camInputState.flyUp=state; break;
	}

}

void GLPlayback::onRender()
{
	// SET VIEWPORT & CLEAR SCREEN
	glViewport(0,0,w(),h());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_direction[] = { 0.0, 0.0, -1.0 };
	GLfloat light_position1[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_cutoff = 15.f;

	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// SETUP CAM
	Cam.Draw();

	// DRAW CROSSHAIR
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINES);
	glColor3f(255,0,0);
	glVertex3f(0,0.01f,-0.1f);
	glVertex3f(0,-0.01f,-0.1f);
	glVertex3f(0.01f,0.f,-0.1f);
	glVertex3f(-0.01f,0.f,-0.1f);
	glColor3f(255,255,255);
	glEnd();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, light_cutoff);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	// DRAW OBJECTS
	for(int i=0; i<world.Size();i++)
		world[i]->Draw();

	// DRAW ORIGIN AXIS
	glPushMatrix();
	glLoadIdentity();
	drawAxis(25);
	glPopMatrix();

	// DRAW CHANNELS
	glViewport(30,30,200,200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.f, 1.f, 1.f, -1.f, -1.f, 20.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT, GL_FILL);

	drawBox(-2,-2,-10,2,2,-11);

	glBegin(GL_LINES);
	float size=0.2;
	for(float i=-5.5; i<=5.5; i++) {
		glColor4f(0.f,0.f,1.f,.5f);
		glVertex2f(-1.f,i*size);
		glVertex2f(1.f,i*size);
	}
	glEnd();
	glBegin(GL_LINES);
	size=0.2;
	for(float i=-5.5; i<=5.5; i++) {
		glColor4f(0.f,0.f,1.f,0.5f);
		glVertex2f(i*size,1.f);
		glVertex2f(i*size,-1.f);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(1.f,0.f,0.f);
	for(int i=-0.16; i<world.Size();i++) {
		AnimationClip* aclip = world[i]->GetAnim();
		if(aclip) {
			float f = 0.4;
			for(float i=0.f; i<=4.f; i+=0.01) {
				glVertex2f((i-2.f)*f,aclip->EvaluateChannel(stateCfg.numChannel,i)*f);
			}
		}
	}
	glEnd();
	glPopAttrib();
}

void GLPlayback::onCleanup()
{
	for(int i=0;i<world.Size();i++) {
		printf("Deleting %s...\n",world[i]->GetTag());
		delete world[i];
	}
	delete efactory;
}

void GLPlayback::Timer_CB (void* userdata)
{
	GLPlayback* app = (GLPlayback*)userdata;
	app->redraw();
	Fl::repeat_timeout(1.0/FPS, Timer_CB, userdata);
}

void GLPlayback::DecChannel ()
{
	stateCfg.numChannel--;
	printf("canal %d\n", stateCfg.numChannel);
}

void GLPlayback::IncChannel ()
{
	stateCfg.numChannel++;
	printf("canal %d\n", stateCfg.numChannel);
}

void GLPlayback::draw()
{
	static bool first = true;
	if (first) { InitGL(); first=false;}
	if(!valid()) { valid(1);
		glLoadIdentity();
            glViewport(0,0,w(),h());
	}

	onLoop(0);
	onRender();
}

void GLPlayback::FixViewport(int w, int h)
{
	glLoadIdentity();
    glViewport(0,0,w,h);
}

void GLPlayback::resize(int x, int y, int w, int h)
{
	Cam.SetAspect(float(w)/float(h));
	FixViewport(w,h);
	Fl_Gl_Window::resize(x,y,w,h);
}


int GLPlayback::handle(int event)
{
	switch(event) {
		case FL_FOCUS: FL_UNFOCUS: return 1;
	}
	//redraw();
	return Fl_Gl_Window::handle(event);
}

extern Display* fl_display;

int GLPlayback::jump_cursor(int new_x, int new_y)
{
#if defined WIN32
	BOOL result = SetCursorPos(new_x, new_y);
	if (result) return 0; // OK
#elif defined __APPLE__
	CGPoint new_pos;
	CGEventErr err;
	new_pos.x = new_x;
	new_pos.y = new_y;
	err = CGWarpMouseCursorPosition(new_pos);
	if (!err) return 0; // OK
#else // Assume this is Xlib
	Window rootwindow = DefaultRootWindow(fl_display);
	int err = XWarpPointer(fl_display, rootwindow, rootwindow, 0, 0, 0, 0, new_x, new_y);
	if (err != BadWindow) return 0; // OK
#endif
	return -1; // Fail
}

void GLPlayback::SwitchWireframe()
{
	stateCfg.wireframe = !stateCfg.wireframe;
	if(stateCfg.wireframe)
		glPolygonMode(GL_FRONT,GL_LINE);
	else
		glPolygonMode(GL_FRONT,GL_FILL);
}


void GLPlayback::AppStateConfig::Clear ()
{
	wireframe = lock = prevLMBclicked = false;
}
