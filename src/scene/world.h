#pragma once

class Entity;
#include <stdio.h>

class World {
public:
	World ();
	virtual ~World ();

	bool Add(Entity* entity);
	bool Delete(Entity* entity);
	bool Delete(const char* tag);

	Entity* operator[](const size_t i)
	{ return entities[i]; }
	size_t Size()
	{ return size; }
	Entity* GetEntity(const size_t i) {
		return entities[i];
	}

	void Debug();

private:
	static const size_t ENTITIES_SIZE = 1024;
	Entity* entities[ENTITIES_SIZE];
	size_t size;
};


