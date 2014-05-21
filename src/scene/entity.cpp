#include "entity.h"

Entity::Entity (const char* tag, Model* model, AnimationClip* animclip)
{
	this->tag = strdup(tag);
	if(model) this->model=model;
	if(animclip) this->animclip = animclip;
	animdata.time = 0;
	animdata.playing = false;
}

Entity::~Entity ()
{
	if(tag) delete tag;
	if(model) delete model;
	if(animclip) delete animclip;
}

void Entity::Update(float delta)
{
	if(model->skin && animclip && animdata.playing) UpdateAnim(0.01f);
	if(model->skel != NULL) model->skel->Update(transform);
	if(model->skin != NULL) model->skin->Update();
}

void Entity::Draw()
{
	glLoadMatrixf(transform);
	if(model->skel) model->skel->Draw();
	if(model->skin != NULL) model->skin->Draw(transform);
}

// dentro del Skeleton y sets fuera? skel->UpdateWithAnim(animclip,delta);
void Entity::UpdateAnim (float delta) {
	int numJoints = (animclip->GetNumChannels()-3)/3;
	animdata.time += delta;
	if (animdata.time > 4.f) animdata.time = 0.f;
	transform[3].x = animclip->EvaluateChannel(0,animdata.time);
	transform[3].y = animclip->EvaluateChannel(1,animdata.time);
	transform[3].z = animclip->EvaluateChannel(2,animdata.time);
	int channel_index = 3;
	for (int i=0; i < numJoints; i++) {
		float x,y,z;
		x = animclip->EvaluateChannel(channel_index, animdata.time);
		channel_index++;
		y = animclip->EvaluateChannel(channel_index, animdata.time);
		channel_index++;
		z = animclip->EvaluateChannel(channel_index, animdata.time);
		channel_index++;
		model->skel->UpdateJointPose(i,x,y,z);
	}
}

float Entity::EvaluateChannel(int index, float t)
{
	return animclip->EvaluateChannel(index,t);
}

void Entity::ResetAnim()
{
	animdata.time=0.f;
}

void Entity::SwitchPlayAnim()
{
	animdata.playing = !animdata.playing;
}

void Entity::StopAnim()
{
	animdata.time = 0;
	animdata.playing = false;
}

bool Entity::IsPlaying()
{
	return animdata.playing;
}


Skeleton::Joint* Entity::SkelRayTest(const Ray& ray) {
	if(model->skel) return model->skel->RayTest(ray);
	else return NULL;
}
