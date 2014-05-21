#pragma once

#include <FL/Fl_Browser.H>

class World;
class Entity;

class EntityFactory {
public:
	EntityFactory (World* world, Fl_Browser* browser);
	virtual ~EntityFactory ();

	Entity* MakeMosca(const char* name);
	Entity* MakeDragon(const char* name);
	Entity* MakePersona(const char* name);
	Entity* MakeTubo(const char* name);
	Entity* MakeAnimMosca(const char* name);

private:
	void addEntity(Entity* e);
	World* world;
	Fl_Browser* browser;
	int count;
	char tagbuffer[256];

};


