#pragma once

#include <GL/gl.h>

struct weight_t {
	void Set(int boneID, float weight)
	{ this->boneID=boneID; this->weight=weight; }
	float weight;
	int boneID;
};


class vertex_t {
public:
	vertex_t() { numWeights=0; color.Set(1,1,1); }

	// RENDERING DATA
private:
	Vector3 pos;
	Vector3 color;
	Vector3 normal;
public:
	const Vector3& Position() { return pos; }
	const Vector3& Normal() { return normal; }
	const Vector3& Color() { return color; }
	void Position(const Vector3& pos) { this->pos.Set(pos); }
	void Normal(const Vector3& normal) { this->normal.Set(normal); }
	void Color(const Vector3& color) { this->color.Set(color); }

	// SKINNING DATA
private:
	int numWeights;
	weight_t weights[4];
public:
	int NumWeights() { return numWeights; }
	int BoneID(int boneIndex)
	{ return weights[boneIndex].boneID; }
	float Weight(int boneIndex)
	{ return weights[boneIndex].weight; }
	void AddWeight(int boneID, float weight)
	{
		weights[numWeights].Set(boneID,weight);
		numWeights++;
	}

};

typedef GLuint triangle_t[3];

