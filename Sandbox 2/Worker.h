#pragma once


class World;
class Chunk;
class Cell;

class Worker
{
protected:
	World& world;
	Chunk* chunk;
public:
	Worker(World& world, Chunk* chunk);

	void updateChunk();

	virtual void updateCell(int x, int y, Cell* cell) = 0;

	Cell* getCell(int x, int y);

	void setCell(int x, int y, Cell* cell);

	void moveCell(int x, int y, int destX, int destY);

	bool inBounds(int x, int y);

	bool isEmpty(int x, int y);

};

