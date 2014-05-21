#include "world.h"
#include "entity.h"

World::World()
{
	size=0;
}

World::~World()
{

}


bool World::Add(Entity* entity)
{
	if(size < ENTITIES_SIZE) {
		entities[size] = entity;
		size++;
		return true;
	} else {
		return false;
	}
}

void World::Debug()
{
	for(int i=0;i<size;i++)
		printf("%s,",entities[i]->GetTag());
	printf("\n");
}

bool World::Delete(const char* tag)
{
	int i=0;
	while(i<size) {
		if (strcmp(entities[i]->GetTag(),tag)==0) i++;
		else break;
	}

	if (i<size) {
		delete entities[i];
		for(;i<size;i++) {
			entities[i] = entities[i+1];
		}
		size--;
		return true;
	} else {
		return false;
	}
}

bool World::Delete(Entity* entity)
{
	int i=0;
	while(i<size) {
		if (entities[i]!=entity) i++;
		else break;
	}

	if (i<size) {
		delete entities[i];
		for(;i<size;i++) {
			entities[i] = entities[i+1];
		}
		size--;
		return true;
	} else {
		return false;
	}
}
