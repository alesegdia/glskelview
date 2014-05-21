#pragma once

#include "../common.h"

#include "../math/vector3.h"
#include "../math/matrix34.h"
#include "../parser/token.h"
#include "../geometry/vertex.h"
#include "skeleton.h"


class Skin {

private:

	vertex_t* vertices;
	triangle_t* tris;
	int vertexNum;
	int normalsNum;
	int trisNum;
	Skeleton* skel;
	Matrix34* bindings;
	GLfloat* vertexbuffer;
	GLfloat* normalbuffer;
	GLuint* trisbuffer;

public:

	Skin (const char* path=NULL);
	virtual ~Skin ();

	void Reset();
	void SetSkel(Skeleton* skel);
	void Parse(const char* path);
	void Update();
	void Draw(Matrix34& mat);


};


