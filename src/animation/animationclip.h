#pragma once

#include "../common.h"

#include "../math/vector3.h"
#include "../math/lerp.h"
#include "../parser/token.h"


class AnimationClip {

public:

	struct AnimationData {
		bool playing;
		float time;
	};

private:

	class Channel {

	private:

		enum ExtrapMethod {
			CONSTANT, CYCLE, CYCLE_OFFSET
		};

		enum TangentRule {
			LINEAR, SMOOTH, FLAT
		};

		struct Keyframe {
			float time;
			float value;
			float tangentIn, tangentOut;
			TangentRule ruleIn, ruleOut;
			float A,B,C,D;
		};

		Keyframe* keyframes;
		ExtrapMethod extrapIn;
		ExtrapMethod extrapOut;
		int numKeys;

	public:

		Channel();
		~Channel();

		bool Parse(Tokenizer& tok, float minTime, float maxTime);
		float Evaluate /* time */
			(float t, float max);

	private:

		void Precompute(float minTime, float maxTime);
		void ComputeTangentOut(int i, int maxTime);
		void ComputeTangentIn(int i, int minTime);
		void ComputeKeyframe(int i, int maxTime);
		float Evaluate /* keyframe */
			(float t,int index, float maxTime);


		ExtrapMethod ParseExtrapolateMethod(const char* m);
		TangentRule ParseTangentRule(const char* m);

	};


	Channel* channels;
	int numChannels;

	Vector3 range;

public:

	AnimationClip(const char* path);
	~AnimationClip();
	bool Parse(const char* path);

	int GetNumChannels();
	float EvaluateChannel(int index, float t);

};

