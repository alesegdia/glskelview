#include "entityfactory.h"
#include "../scene/entity.h"
#include "../scene/world.h"
#include "../animation/model.h"
#include <stdio.h>

EntityFactory::EntityFactory (World* world, Fl_Browser* browser)
{
	this->world = world;
	this->browser=browser;
	count=0;
}

EntityFactory::~EntityFactory ()
{

}

void EntityFactory::addEntity(Entity* e) {
	world->Add(e);
	browser->add(e->GetTag(),e);
}

Entity* EntityFactory::MakeMosca(const char* name) {
	Entity* e = new Entity(name,new Model(new Skeleton("media/wasp.skel"),new Skin("media/wasp.skin")));
	addEntity(e);
	return e;
}

Entity* EntityFactory::MakeDragon(const char* name) {
	Entity* e = new Entity(name,new Model(new Skeleton("media/dragon.skel"),NULL));
	addEntity(e);
	return e;
}

Entity* EntityFactory::MakePersona(const char* name) {
	Entity* e = new Entity(name,new Model(new Skeleton("media/test.skel"),NULL));
	addEntity(e);
	return e;
}

Entity* EntityFactory::MakeTubo(const char* name) {
	Entity* e = new Entity(name,new Model(new Skeleton("media/tube.skel"),new Skin("media/tube_smooth.skin")));
	addEntity(e);
	return e;
}

Entity* EntityFactory::MakeAnimMosca(const char* name) {
	Entity* e = new Entity(name,
		new Model(
			new Skeleton("media/wasp.skel"), new Skin("media/wasp.skin")),
		new AnimationClip("media/wasp_walk.anim"));
	addEntity(e);
	return e;
}
