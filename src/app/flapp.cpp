#include "flapp.h"

void FlApp::ParseCommand ()
{
	shellbuffer[nextCmdBufferChar+1] = '\0';
	Tokenizer tok;
	tok.OpenBuffer(shellbuffer);
	char buffer[256];
	tok.GetToken(buffer);
	if(strcmp(buffer, "load")==0) {
		printf("load!!");
		tok.GetToken(buffer);
		char* tmp = strdup(buffer);
		tok.GetToken(buffer);
		glp->MakeEntity(tmp, buffer);
		delete [] tmp;
	} else if(strcmp(buffer, "setcamspeed")==0) {
		glp->SetCamSpeed(tok.GetFloat());
	} else if(strcmp(buffer, "rotate")==0) {
		tok.GetToken(buffer);
		switch (buffer[0]) {
		case 'x': printf("X!\n"); fflush(0); break;
		case 'y': printf("Y!\n"); fflush(0); break;
		case 'z': printf("Z!\n"); fflush(0); break;
		}
	} else if(strcmp(buffer, "delete")==0) {
		tok.GetToken(buffer);
		glp->DeleteEntity(buffer);
		selectedEntity=NULL;
		selectedJoint=NULL;
	}
	tok.Close();
}

int FlApp::handle (int event)
{
	glp->redraw();
	static bool first = true;
	if(event==FL_FOCUS||event==FL_UNFOCUS) return 1;
	if(event==FL_KEYDOWN)
	{
		//*  CONSOLA *****************************/
		if(estado==AppState::CONSOLE) {
			if(first) {
				printf("\n $> ");
				fflush(0);
				first=false;
			}

			if(Fl::event_key()==FL_Escape) {
				estado=AppState::GUI;
				return 1;
			} else if(Fl::event_key()==FL_Enter) {
				ParseCommand();
				nextCmdBufferChar=0;
				printf("\n $> ");
				fflush(0);
				return 1;
			} else if(Fl::event_key()==FL_BackSpace) {
				nextCmdBufferChar--;
				if (nextCmdBufferChar<0) nextCmdBufferChar=0;
				printf("\b \b");fflush(0);
			} else if(Fl::event_key()==' ') {
				printf(" "); fflush(0);
				shellbuffer[nextCmdBufferChar]=' ';
				nextCmdBufferChar++;
				return 1;
			} else {
				printf("%c",Fl::event_key());
				fflush(0);
				shellbuffer[nextCmdBufferChar] = Fl::event_key();
				nextCmdBufferChar++;
				return 1;
			}
		//* APLICACIÓN **************************/
		} else if(estado==AppState::GUI) {
			if(Fl::event_key()==FL_Escape)
				hide();
			else if(Fl::event_key()=='1') {
				estado = AppState::CONSOLE;
				first=true;
				return 1;
			} else if(Fl::event_key()=='e') {
				estado = AppState::NAVIGATE;
				glp->SwitchLock();
				return 1;
			} else if(Fl::event_key()=='f') {
				glp->SwitchWireframe();
				return 1;
			} else if(Fl::event_key()=='o') {
				glp->IncChannel();
				return 1;
			} else if(Fl::event_key()=='l') {
				glp->DecChannel();
				return 1;
			}
		} else if (estado == AppState::NAVIGATE) {
			switch(Fl::event_key()) {

			case 'w': case 'a': case 's': case 'd': case 'c': case ' ':
				glp->UpdateWASD(Fl::event_key(), true);
				return 1;

			case 'e':
				estado = AppState::GUI;
				glp->SwitchLock();
				return 1;
			}
		}
	} else if(event==FL_KEYUP) {
		if(estado==AppState::NAVIGATE) {
			switch(Fl::event_key()) {
			case 'w': case 'a': case 's': case 'd': case 'c': case ' ':
				glp->UpdateWASD(Fl::event_key(), false);
				return 1;
			}
		}
	}

	return Fl_Double_Window::handle(event);
}

void FlApp::resize (int X, int Y, int W, int H)
{
	Fl_Double_Window::resize(X,Y,W,H);
}

void FlApp::AnimPlay_CB2 ()
{
	if(selectedEntity) {
		selectedEntity->SwitchPlayAnim();
		if (selectedEntity->IsPlaying()) {
			animplay->label("PAUSE");
		} else {
			animplay->label("PLAY");
		}
	}
}

void FlApp::AnimStop_CB2 ()
{
	if(selectedEntity) selectedEntity->StopAnim();
}


void FlApp::EntityMoveX1_CB2()
{
	if(selectedEntity) selectedEntity->MoveX(-1);
	glp->redraw();
}

void FlApp::EntityMoveX2_CB2()
{
	if(selectedEntity) selectedEntity->MoveX(1);
	glp->redraw();
}

void FlApp::EntityMoveY1_CB2()
{
	if(selectedEntity) selectedEntity->MoveY(-1);
	glp->redraw();
}

void FlApp::EntityMoveY2_CB2()
{
	if(selectedEntity) selectedEntity->MoveY(1);
	glp->redraw();
}

void FlApp::EntityMoveZ1_CB2()
{
	if(selectedEntity) selectedEntity->MoveZ(-1);
	glp->redraw();
}

void FlApp::EntityMoveZ2_CB2()
{
	if(selectedEntity) selectedEntity->MoveZ(1);
	glp->redraw();
}

void FlApp::ValueChangedX_CB2()
{
	(**selectedJoint).GetPose().x = xslid->value();
}

void FlApp::ValueChangedY_CB2()
{
	(**selectedJoint).GetPose().y = yslid->value();
}

void FlApp::ValueChangedZ_CB2()
{
	(**selectedJoint).GetPose().z = zslid->value();
}

void FlApp::BrowserChanged_CB(Fl_Widget* w, void* userdata) {
	FlApp *appwin = (FlApp*)userdata;
	Fl_Browser* brwsr = ((Fl_Browser*)w);
	for(int i =0; i <= brwsr->size(); i++)
	{
		if(brwsr->selected(i)) {
			Entity* e = ((Entity*)brwsr->data(i));
			appwin->BrowserChanged_CB2(e);
			break;
		}
	}
}

void FlApp::BrowserChanged_CB2(Entity* e)
{
	selectedEntity = e;
}
