#pragma once
#include "Worker.h"

class SimpleWorker : public Worker
{
public:
	SimpleWorker(World& world, Chunk* chunk);

	void updateCell(int x, int y, Cell* cell) override;

private:
	bool MoveDown(size_t x, size_t y, Cell* cell);

	bool MoveDownSide(size_t x, size_t y, Cell* cell);

	bool MoveSide(size_t x, size_t y, Cell* cell);
};

