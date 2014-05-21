#include "animationclip.h"


// ANIMATION CLIP CLASS *****************************

AnimationClip::AnimationClip (const char* path)
{
	channels = NULL;
	numChannels = 0;
	Parse(path);
}

AnimationClip::~AnimationClip ()
{
	if(channels != NULL) delete [] channels;
}

int AnimationClip::GetNumChannels ()
{
	return numChannels;
}


bool AnimationClip::Parse (const char* path) {

	Tokenizer tok;
	tok.Open(path);
	tok.FindToken("animation");
	tok.FindToken("{");

	tok.FindToken("range");
	range.x = tok.GetFloat();
	range.y = tok.GetFloat();

	tok.FindToken("numchannels");
	numChannels = tok.GetInt();
	printf("NUMCHANNELS: %d\n",numChannels);
	channels = new Channel[numChannels];

	for(int i=0; i<numChannels; i++) {
		printf("CHANNEL: %d, ",i); fflush(0);
		channels[i].Parse(tok,range.x,range.y);
		printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	}

	tok.Close();
	return true;

}

float AnimationClip::EvaluateChannel (int index, float t) {
	return channels[index].Evaluate(t,range.y);
}





// CHANNEL CLASS **********************************

AnimationClip::Channel::Channel ()
{
	keyframes=NULL;
}

AnimationClip::Channel::~Channel ()
{
	if(keyframes != NULL) delete [] keyframes;
}

float AnimationClip::Channel::Evaluate(float t, int index, float maxTime) {
	float v;
	if( numKeys == 1 || index == numKeys - 1 ) v = maxTime;
	else v = keyframes[index+1].time;

	float u = invlerp(t,keyframes[index].time,v);
	return keyframes[index].D +
		u * (keyframes[index].C +
		u * (keyframes[index].B +
		u * (keyframes[index].A)));
}

bool AnimationClip::Channel::Parse(Tokenizer& tok, float minTime, float maxTime) {
	char buffer[64];
	tok.FindToken("channel");
	tok.FindToken("{");

	tok.FindToken("extrapolate");
	tok.GetToken(buffer);
	extrapIn = ParseExtrapolateMethod(buffer);
	tok.GetToken(buffer);
	extrapOut = ParseExtrapolateMethod(buffer);

	tok.FindToken("keys");
	numKeys = tok.GetInt();
	printf("numkeys: %d\n",numKeys); fflush(0);
	keyframes = new Keyframe[numKeys];
	tok.FindToken("{");
	for(int i=0; i < numKeys; i++) {
		keyframes[i].time = tok.GetFloat();
		keyframes[i].value = tok.GetFloat();
		tok.GetToken(buffer);
		keyframes[i].ruleIn = ParseTangentRule(buffer);
		tok.GetToken(buffer);
		keyframes[i].ruleOut = ParseTangentRule(buffer);
	}
	tok.FindToken("}");
	tok.FindToken("}");

	Precompute(minTime,maxTime);
	return true;
}


void AnimationClip::Channel::Precompute(float minTime, float maxTime) {

	// quitamos falsos smooths en inicio y final
	keyframes[0].ruleIn = (keyframes[0].ruleIn == TangentRule::SMOOTH)
		? TangentRule::LINEAR : keyframes[0].ruleIn;
	keyframes[0].ruleOut = (keyframes[0].ruleOut == TangentRule::SMOOTH)
		? TangentRule::LINEAR : keyframes[0].ruleOut;
	keyframes[numKeys-1].ruleIn = (keyframes[numKeys-1].ruleIn == TangentRule::SMOOTH)
		? TangentRule::LINEAR : keyframes[numKeys-1].ruleIn;
	keyframes[numKeys-1].ruleOut = (keyframes[numKeys-1].ruleOut == TangentRule::SMOOTH)
		? TangentRule::LINEAR : keyframes[numKeys-1].ruleOut;

	// nos da igual porque se extrapola en estas zona
	keyframes[0].tangentIn = 0;
	keyframes[numKeys-1].tangentOut = 0;

	// computamos tangentes y keyframes
	int i = 1;
	ComputeTangentOut(0,maxTime);
	if(numKeys >= 2) ComputeTangentIn(1,minTime);
	ComputeKeyframe(0,maxTime);
	while(i < numKeys) {
		ComputeTangentOut(i,maxTime);
		i++;
		if(i == numKeys) break;
		ComputeTangentIn(i,minTime);
		ComputeKeyframe(i-1,maxTime);
	}
	ComputeTangentOut(numKeys-1,maxTime);

}

void AnimationClip::Channel::ComputeTangentIn(int i, int minTime) {

	float v1, tmp_t, tmp_v;

	switch(keyframes[i].ruleIn) {

		case TangentRule::FLAT:
			keyframes[i].tangentIn = 0;
			break;

		case TangentRule::LINEAR:
			if(numKeys == 1) {
				tmp_t = minTime;
				tmp_v = keyframes[i].value;
			} else {
				tmp_t = keyframes[i-1].time;
				tmp_v = keyframes[i-1].value;
			}
			v1 = (keyframes[i].time - tmp_t);
			assert( v1 != 0 && "mismo keyframe este y el anterior" );
			if( v1 == 0 ) keyframes[i].tangentIn = 0;
			else keyframes[i].tangentIn =
				(keyframes[i].value - tmp_v) / v1;
			break;

		case TangentRule::SMOOTH:
			assert( (i != 0 && i != numKeys-1) && "control de smooths falla" );
			v1 = (keyframes[i+1].time - keyframes[i-1].time);
			printf("%f, %f,%f\n",keyframes[i].time, keyframes[i-1].time, v1);
			assert( v1 != 0 && "mismo keyframe el anterior y el siguiente" );
			keyframes[i].tangentIn = -
				(keyframes[i+1].value - keyframes[i-1].value) / v1;
			break;
	}
}

void AnimationClip::Channel::ComputeTangentOut(int i, int maxTime) {

	float v1, tmp_t, tmp_v;

	switch(keyframes[i].ruleOut) {

		case TangentRule::FLAT:
			keyframes[i].tangentOut = 0;
			break;

		case TangentRule::LINEAR:
			if(numKeys==1) {
				tmp_t = maxTime;
				tmp_v = keyframes[i].value;
			} else {
				tmp_t = keyframes[i+1].time;
				tmp_v = keyframes[i+1].value;
			}
			v1 = (tmp_t - keyframes[i].time);
			assert( v1 != 0 && "mismo keyframe este y el siguiente" );
			if( v1 == 0 ) keyframes[i].tangentOut = 0;
			else keyframes[i].tangentOut =
			// - ?
				(tmp_v - keyframes[i].value) / v1;
			break;

		case TangentRule::SMOOTH:
			keyframes[i].tangentOut = keyframes[i].tangentIn;
			break;
	}


}

float AnimationClip::Channel::Evaluate (float t, float max) {
	Vector3 range;
	float p;
	int i;
	range.x = keyframes[0].time;
	range.y = keyframes[numKeys-1].time;
	if(numKeys == 1) return Evaluate(t,0,max);
	if(t < range.x) {
		// extrapolate left
		switch(extrapIn) {
		case ExtrapMethod::CONSTANT:
			return keyframes[0].value;
			break;
		case ExtrapMethod::CYCLE:
			p = range.y - range.x;
			while(t < range.x) {
				t += p;
			}
			return Evaluate(t,max);
			break;
		case ExtrapMethod::CYCLE_OFFSET:
			p = range.y - range.x;
			i=0;
			while(t < range.x) {
				t += p;
				i++;
			}
			return (keyframes[0].value - keyframes[numKeys-1].value) * (i)
				+ Evaluate(t,max);
			break;
		}

	} else if(t > range.y) {
		// extrapolate right
		switch(extrapOut) {
		case ExtrapMethod::CONSTANT:
			return keyframes[numKeys-1].value;
			break;
		case ExtrapMethod::CYCLE:
			p = range.y - range.x;
			while(t > range.y) {
				t -= p;
			}
			return Evaluate(t,max);
			break;
		case ExtrapMethod::CYCLE_OFFSET:
			p = range.y - range.x;
			i=0;
			while(t > range.y) {
				t -= p;
				i++;
			}
			return (keyframes[numKeys-1].value - keyframes[0].value) * (i)
				+ Evaluate(t,max);
			break;
		}
	} else {
		int i = 0;
		while (keyframes[i].time <= t) {
			if (keyframes[i].time == t)
				return keyframes[i].value;
			i++;
		}
		return Evaluate(t,i-1,max);
	}
}



// PRE: tangentIn de i+1 computada
void AnimationClip::Channel::ComputeKeyframe(int i, int max) {
	float p0, p1, t0, t1, v0, v1;
	p0 = keyframes[i].value;
	t0 = keyframes[i].time;
	v0 = keyframes[i].tangentOut;
	if(numKeys == 0 && i == 0) {
		p1 = p0;
		t1 = max;
		v1 = 0;
	} else {
		p1 = keyframes[i+1].value;
		t1 = keyframes[i+1].time;
		v1 = keyframes[i+1].tangentIn;
	}
	float v_0 = (t1-t0)*v0;
	float v_1 = (t1-t0)*v1;

	keyframes[i].A = 2 * p0 - 2 * p1 + v_0 + v_1;
	keyframes[i].B = -3 * p0 + 3 * p1 - 2 * v_0 - v_1;
	keyframes[i].C = v_0;
	keyframes[i].D = p0;

	printf("\tkeyframe %d",i);
	printf("\t\t%f\t%f\t%f\t%f \n\t\t\t\t%f\t%f\t%f\t%f\t%f\t%f\n",keyframes[i].A,keyframes[i].B,
			keyframes[i].C,keyframes[i].D,p0,p1,v0,v1,t0,t1);
}


AnimationClip::Channel::ExtrapMethod AnimationClip::Channel::ParseExtrapolateMethod (const char* m) {
	if(!strcmp("constant",m))
		return ExtrapMethod::CONSTANT;
	if(!strcmp("cycle",m))
		return ExtrapMethod::CYCLE;
	if(!strcmp("cycle_offset",m))
		return ExtrapMethod::CYCLE_OFFSET;
}

AnimationClip::Channel::TangentRule AnimationClip::Channel::ParseTangentRule (const char* m) {
	if(!strcmp("linear",m))
		return TangentRule::LINEAR;
	if(!strcmp("smooth",m))
		return TangentRule::SMOOTH;
	if(!strcmp("flat",m))
		return TangentRule::FLAT;
}

