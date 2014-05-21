#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "../animation/model.h"
#include "../animation/animationclip.h"

class Entity {
public:
	Entity (const char* tag,
			Model* model=NULL, AnimationClip* animclip=NULL);
	virtual ~Entity ();

	virtual void Update(float);
	virtual void Draw();

	Skeleton* GetSkel() { return model->skel; }
	AnimationClip* GetAnim() { return animclip; }
	Skeleton::Joint* SkelRayTest(const Ray& ray);
	const char* GetTag() { return tag; }
	float EvaluateChannel(int index, float t);

	void UpdateAnim(float delta);
	void ResetAnim();
	void SwitchPlayAnim();
	void StopAnim();
	bool IsPlaying();

	void MoveX(int qtt) { transform[3].x += qtt; }
	void MoveY(int qtt) { transform[3].y += qtt; }
	void MoveZ(int qtt) { transform[3].z += qtt; }

private:

	Model* model;
	AnimationClip* animclip;
	AnimationClip::AnimationData animdata;
	Matrix34 transform;
	char* tag;

};

