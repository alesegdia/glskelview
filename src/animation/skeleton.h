#pragma once

#include "../common.h"

#include "../parser/token.h"
#include "../physics/ray.h"
#include "../math/vector3.h"
#include "../math/matrix34.h"
#include "../geometry/core.h"
#include "../constants.h"



class Skeleton {


public:

	class Joint {

	private:

		// solo creacion y destruccion desde Skeleton
		Joint ();
		virtual ~Joint ();


		// joint props
		std::vector<Joint*> Children;
		std::string Tag;
		Vector3 BoxMin;
		Vector3 BoxMax;
		Vector3 Offset;
		Vector3 Pose;
		Vector3 RotXLimit;
		Vector3 RotYLimit;
		Vector3 RotZLimit;
		int index;

		// transform
		Matrix34 Local;
		Matrix34 World;

		// GUI stuff
		Vector3 Color;
		Vector3 HoldColor;
		bool selected;
		bool hit;

		// para crearlos y cargarlos
		friend class Skeleton;

	public:

		int Index();

		// pose interface
		Vector3& GetPose()			{ return Pose; }
		float TXLimit() 			{ return RotXLimit.x; }
		float TYLimit() 			{ return RotYLimit.x; }
		float TZLimit() 			{ return RotZLimit.x; }
		float BXLimit() 			{ return RotXLimit.y; }
		float BYLimit() 			{ return RotYLimit.y; }
		float BZLimit() 			{ return RotZLimit.y; }

		// GUI stuff
		void SwitchSelected() { selected = !selected; }
		bool IsSelected() { return selected; }
		void Deselect() { selected = false; }

	private:

		// parsear y cargar
		bool Parse(Tokenizer& tok, char& next, Joint* list[]);
		void ApplyPoseConstraints();
		void Parse3DVector (Vector3& vec, Tokenizer& tok);
		void Parse2DVector (Vector3& vec, Tokenizer& tok);
		void AddChild (Joint* child);

		// graficos y fisicas
		void Update(Matrix34& ParentW);
		void Draw();
		const Matrix34& GetWorldMatrix() { return World; }
		float RayTest(const Ray& ray);

	};

private:

	Joint* lista[256];
	Joint* Root;
	int numJoints;
	std::string Tag;


public:

	Skeleton (const char* path=NULL);
	virtual ~Skeleton ();

	void Parse(const char* path);
	void Reset();

	// skinning y animation
	const Matrix34& GetWorldMatrix(int index);
	void UpdateJointPose (int index, float x, float y, float z);

	// graficos y fisicas
	Joint* RayTest(const Ray& ray);
	void Update(Matrix34& transform);
	void Draw();

};


