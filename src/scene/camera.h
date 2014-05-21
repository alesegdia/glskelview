////////////////////////////////////////
// camera.h
////////////////////////////////////////

#ifndef CSE169_CAMERA_H
#define CSE169_CAMERA_H

#include "../geometry/core.h"
#include "../math/matrix34.h"
////////////////////////////////////////////////////////////////////////////////

class Camera {
public:

	struct InputState {

		void Clear();

		bool forward;
		bool backward;
		bool strafeLeft;
		bool strafeRight;
		bool flyUp;
		bool flyDown;

		float rotationX;
		float rotationY;

	};

	Camera();

	void Update();
	void Reset();
	void Draw();

	// Access functions
	void SetAspect(float a)		{Aspect=a;}
	void Move(float x, float y, float z)
		{Matrix34 aux; aux.MakeTranslate(x,y,z);
		 ViewMatrix.Dot(aux,ViewMatrix);}
	void Move(const Vector3& vec) {Position.Add(vec);}
	void HandleInput(InputState state);
	void RotateY(float x) {YRot = YRot + x;}
	void RotateX(float x) {XRot = XRot + x;}
	void MoveY(int arriba) { Position.y += speed * arriba; }
	const Vector3& GetPosition() { return Position; }
	const Vector3 GetForward() {
		return Vector3(-ViewMatrix[0].z,-ViewMatrix[1].z,-ViewMatrix[2].z); }
	void SetSpeed(float value) {
		speed = value;
	}

private:
	void HandleInput();
	static constexpr float SPEED = 0.01f;

	// Perspective controls
	float FOV;		// Field of View Angle
	float Aspect;	// Aspect Ratio
	float NearClip;	// Near clipping plane distance
	float FarClip;	// Far clipping plane distance

	// Transform
	Vector3 Position;
	Matrix34 ViewMatrix;
	float XRot;
	float YRot;
	float speed;
	float rotationRate;

};

////////////////////////////////////////////////////////////////////////////////

/*
The Camera class provides a simple means to controlling the 3D camera. It could
be extended to support more interactive controls. Ultimately. the camera sets the
GL projection and viewing matrices.
*/

#endif
