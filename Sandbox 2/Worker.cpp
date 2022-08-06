#include "Worker.h"
#include "World.h"
#pragma once


Worker::Worker(World& world, Chunk* chunk) :
		world(world),
		chunk(chunk)
	{}

void Worker::updateChunk()
	{
		for (int y = chunk->dirtyRectY1; y <= chunk->dirtyRectY2; y++)
		{
			for (int x = chunk->dirtyRectX1; x <= chunk->dirtyRectX2; x++)
			{
				if (x < 0 || x >= chunk->dimensions.x || y < 0 || y >= chunk->dimensions.y)
				{
					printf("Dirty rect overflow\n");
				}
				Cell* cell = chunk->getCell(x + y * chunk->dimensions.x);
				updateCell(chunk->position.x * chunk->dimensions.x + x, chunk->position.y * chunk->dimensions.y + y, cell);
			}
		}
	}


	Cell* Worker::getCell(int x, int y)
	{
		if (chunk->inBounds(x, y))
			return chunk->getCell(x, y);
		return world.GetCell(x, y);
	}

	void Worker::setCell(int x, int y, Cell* cell)
	{
		if (chunk->inBounds(x, y))
			return chunk->setCell(x, y, cell);
		return world.setCell(x, y, cell);
	}

	void Worker::moveCell(int x, int y, int destX, int destY)
	{
		int pingX = 0, pingY = 0;

		if (x == chunk->position.x * chunk->dimensions.x) pingX = -1;
		if (x == chunk->position.x * chunk->dimensions.x + chunk->dimensions.x-1) pingX = 1;
		if (y == chunk->position.y * chunk->dimensions.y) pingY = -1;
		if (y == chunk->position.y * chunk->dimensions.y + chunk->dimensions.y-1) pingY = 1;

		if (pingX != 0)				  
			world.keepAlive(destX + pingX, destY);
		if (pingY != 0)				 
			world.keepAlive(destX, destY + pingY);
		if (pingX != 0 && pingY != 0) 
			world.keepAlive(destX + pingX, destY + pingY);


		if (   chunk->inBounds(x, y)
			&& chunk->inBounds(destX, destY))
		{
			return chunk->moveCell(chunk, x, y, destX, destY);
		}
		return world.moveCell(x, y, destX, destY);
	}

	bool Worker::inBounds(int x, int y)
	{
		return chunk->inBounds(x, y) || world.inBounds(x, y);
	}

	bool Worker::isEmpty(int x, int y)
	{
		if (chunk->inBounds(x, y))
			return chunk->isEmpty(x, y);
		return world.isEmpty(x, y);
	}


