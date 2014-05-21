#pragma once

#include "skeleton.h"
#include "skin.h"

struct Model {
	Model(Skeleton* skel, Skin* skin)
	{
		if( skel != NULL ) this->skel=skel;
		this->skin = skin;
		if( skin != NULL ) { this->skin->SetSkel(skel); } }
	~Model () { if (skel) delete skel; if (skin) delete skin; }

	Skeleton* skel;
	Skin* skin;
};



