#include "SimpleWorker.h"
#include "World.h"

SimpleWorker::SimpleWorker(World& world, Chunk* chunk) : Worker(world, chunk) {}

	void SimpleWorker::updateCell(int x, int y, Cell* cell) {
		if ((bool)(cell->props & CellProperties::MOVE_DOWN) && MoveDown(x, y, cell)) {}
		else if ((bool)(cell->props & CellProperties::MOVE_DOWN_SIDE) && MoveDownSide(x, y, cell)) {}
		else if ((bool)(cell->props & CellProperties::MOVE_SIDE) && MoveSide(x, y, cell)) {}
	}


	bool SimpleWorker::MoveDown(size_t x, size_t y, Cell* cell)
	{
		if (Chunk* chunk = world.getChunk(x, y + 1))
		{
			if (chunk->canGoHere(x, y + 1, cell))
			{
				moveCell(x, y, x, y + 1);
				return true;
			}
		}
		return false;
	}

	bool SimpleWorker::SimpleWorker::MoveDownSide(size_t x, size_t y, Cell* cell)
	{
		int delta = Randomiser::rand() % 2 * 2 - 1;
		if (Chunk* chunk = world.getChunk(x + delta, y + 1))
		{
			if (chunk->canGoHere(x + delta, y + 1, cell) && chunk->canGoHere(x + delta, y, cell))
			{
				moveCell(x, y, x + delta, y + 1);
				return true;
			}
		}
		else if (Chunk* chunk = world.getChunk(x + delta, y + 1))
		{
			if (chunk->canGoHere(x - delta, y + 1, cell) && chunk->canGoHere(x - delta, y, cell))
			{
				moveCell(x, y, x - delta, y + 1);
				return true;
			}
		}
		return false;
	}

	bool SimpleWorker::MoveSide(size_t x, size_t y, Cell* cell)
	{
		int delta = Randomiser::rand() % 2 * 2 - 1;
		if (Chunk* chunk = world.getChunk(x + delta, y))
		{
			if (chunk->canGoHere(x + delta, y, cell))// && canGoHere(x + delta, y, cell))
			{
				moveCell(x, y, x + delta, y);
				return true;
			}
		}
		else if (Chunk* chunk = world.getChunk(x + delta, y))
		{
			if (chunk->canGoHere(x - delta, y, cell))// && canGoHere(x - delta, y, cell))
			{
				moveCell(x, y, x - delta, y);
				return true;
			}
		}
		return false;
	}


