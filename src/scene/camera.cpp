////////////////////////////////////////
// camera.cpp
////////////////////////////////////////

#include "camera.h"

////////////////////////////////////////////////////////////////////////////////

Camera::Camera() {
	Reset();
}

void Camera::InputState::Clear ()
{
	forward = backward = false;
	strafeLeft = strafeRight = false;
	flyUp = flyDown = false;
	rotationX = rotationY = 0.f;
}

////////////////////////////////////////////////////////////////////////////////

void Camera::Update() {


	ViewMatrix.Identity();

	// guardar matrices para eficiencia
	Matrix34 offset, rot;
	offset.MakeTranslate(Position);
	rot.FromEulers(YRot,XRot,0,Matrix34::EULER_YXZ);

	ViewMatrix.Dot(offset,rot);
}

////////////////////////////////////////////////////////////////////////////////

void Camera::Reset() {
	FOV=60.0f;
	Aspect=1.33f;
	NearClip=0.1f;
	FarClip=100.0f;
	ViewMatrix.Identity();
	Position.x=0.f;
	Position.y=0.f;
	Position.z=0.f;
	rotationRate = 0.001f;
	XRot=0;
	YRot=0;
	speed=SPEED;
}

void Camera::HandleInput (InputState state) {
	Vector3 dir;
	bool fwdf = true;
	if(state.forward) {
		dir.Set(
			ViewMatrix[2].x,
			-ViewMatrix[1].z,
			-ViewMatrix[2].z);
		dir.Negate();
	} else if(state.backward) {
		dir.Set(
			ViewMatrix[2].x,
			-ViewMatrix[1].z,
			-ViewMatrix[2].z);
	} else {
		fwdf=false;
	}

	bool strafef = true;
	Vector3 strafe;
	if(state.strafeRight) {
		strafe.Set(ViewMatrix[0]);
		strafe.Negate();
		strafe.NegateZ();
		strafe.y = 0;
	} else if(state.strafeLeft) {
		strafe.Set(ViewMatrix[0]);
		strafe.NegateZ();
		strafe.y = 0;
	} else {
		strafef = false;
	}

	if(fwdf && strafef)
	{
		strafe.Normalize();
		dir.Add(strafe);
		dir.Normalize();
		dir.Scale(speed);
		Move(dir);
	}
	else if(fwdf && !strafef)
	{
		dir.Normalize();
		dir.Scale(speed);
		Move(dir);
	}
	else if(!fwdf && strafef)
	{
		strafe.Normalize();
		strafe.Scale(speed);
		Move(strafe);
	}

	if (state.flyUp) MoveY(-1);
	else if (state.flyDown) MoveY(1);

	RotateX(state.rotationX * rotationRate);
	RotateY(state.rotationY * rotationRate);

}

////////////////////////////////////////////////////////////////////////////////

void Camera::Draw() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV,Aspect,NearClip,FarClip);
	glMultMatrixf(ViewMatrix);

	// draw crosshair
	/*
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
	*/
}

////////////////////////////////////////////////////////////////////////////////
