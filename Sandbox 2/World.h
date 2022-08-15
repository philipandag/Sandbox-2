#pragma once
#include "Vector4.h"
#include "Vector2.h"
#include "MyRenderer.h"
#include <vector>
#include "Cell.h"
#include "AbstractMap.h"
#include <algorithm>
#include "Chunk.h"
#include <unordered_map>
#include <thread>
#include "SimpleWorker.h"
#include "Writer.h"
#include <mutex>
#include <functional>
#include <concurrent_unordered_map.h>

using std::vector;

struct pair_hash {
	template<typename T1, typename T2>
	size_t operator() (const std::pair<T1, T2>& pair) const {
		return pair.first*7 + pair.second*29;
	}
};

const int chunkDensity = 2; // amount of chunks in each axis, total amount = chunkDensity * chunkDensity
							 // chunks of size 20x20 seem to be optimal

class World
{

public:
	const Vector2 dimensions;
	const Vector2 chunkDimensions;
	double scale;
	int cellsCount;

	SDL_Texture* texture;
	std::vector<Chunk*> chunks;
	Concurrency::concurrent_unordered_map<std::pair<int, int>, Chunk*, pair_hash> chunkLookup;
	std::mutex chunkMutex;
	MyRenderer& renderer;
public:
	World(Vector2 dimensions,
		double scale,
		MyRenderer& renderer
	) :
		dimensions(dimensions/scale),
		chunkDimensions(dimensions/scale/chunkDensity),
		scale(scale),
		renderer(renderer)
	{
		texture = SDL_CreateTexture(renderer.getRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, dimensions.x, dimensions.y);

		updateTexture();
	}

	void updateTexture()
	{
		SDL_SetRenderDrawColor(renderer.getRenderer(), 0, 0, 0, 255);
		SDL_SetRenderTarget(renderer.getRenderer(), texture);
		SDL_RenderFillRect(renderer.getRenderer(), NULL);
		for (Chunk* p : chunks)
		{
			SDL_Rect rect;
			rect.x = p->position.x * chunkDimensions.x;
			rect.y = p->position.y * chunkDimensions.y;
			rect.w = p->dimensions.x;
			rect.h = p->dimensions.y;
			SDL_RenderCopy(renderer.getRenderer(), p->getTexture(),
				NULL, &rect);
		}
		
		drawChunksVisualisation();
		SDL_SetRenderTarget(renderer.getRenderer(), NULL);
	}

	void drawChunksVisualisation()
	{
		for (Chunk* chunk : chunks)
		{
			SDL_Rect r;
			r.x = chunk->position.x * scale * chunkDimensions.x;
			r.y = chunk->position.y * scale * chunkDimensions.y;
			r.w = chunk->dimensions.x * scale;
			r.h = chunk->dimensions.y * scale;
			SDL_SetRenderDrawColor(renderer.getRenderer(), 255 * (1 - (float)chunk->emptyFrames / EMPTY_FRAMES_DELETION_DELAY), 0, 0, 255);
			SDL_RenderDrawRect(renderer.getRenderer(), &r);

			if (chunk->dirtyRectX2 > 0 && chunk->dirtyRectY2 > 0)
			{
				r.x += chunk->dirtyRectX1 * scale;
				r.y += chunk->dirtyRectY1 * scale;
				r.w = (chunk->dirtyRectX2 - chunk->dirtyRectX1 + 1) * scale;
				r.h = (chunk->dirtyRectY2 - chunk->dirtyRectY1 + 1) * scale;
				SDL_SetRenderDrawColor(renderer.getRenderer(), 0, 255 * (1 - (float)chunk->emptyFrames / EMPTY_FRAMES_DELETION_DELAY), 0, 255);
				SDL_RenderDrawRect(renderer.getRenderer(), &r);
			}
		}
	}

	bool inBounds(size_t x, size_t y)
	{
		if (Chunk* chunk = getChunk(x, y))
			return chunk->inBounds(x, y);
	}

	bool isEmpty(size_t x, size_t y)
	{
		return inBounds(x, y) && getChunk(x, y)->isEmpty(x, y);
	}

	bool canGoHere(size_t x, size_t y, Cell* cell)
	{
		if (Chunk* ch = getChunk(x, y))
		{
			return ch->canGoHere(x, y, cell);
		}
		return false;
	}

	Cell* GetCell(int x, int y) {
		return getChunk(x, y)->getCell(x, y);
	}

	void setCell(size_t x, size_t y, Cell* cell)
	{
		if (Chunk* chunk = getChunk(x, y))
		{
			chunk->setCell(x, y, cell);

			int pingX=0, pingY=0;
			if (x == chunk->position.x * chunk->dimensions.x) pingX = -1;
			if (x == chunk->position.x * chunk->dimensions.x + chunk->dimensions.x - 1) pingX = 1;
			if (y == chunk->position.y * chunk->dimensions.y) pingY = -1;
			if (y == chunk->position.y * chunk->dimensions.y + chunk->dimensions.y - 1) pingY = 1;

			if (pingX != 0)
				keepAlive(x + pingX, y);
			if (pingY != 0)
				keepAlive(x, y + pingY);
			if (pingX != 0 && pingY != 0)
				keepAlive(x + pingX, y + pingY);
		}
	}

	void keepAlive(int x, int y)
	{
		if (Chunk* chunk = getChunk(x, y)) {
			chunk->keepAlive(x, y);
		}
	}

	void moveCell(size_t x, size_t y, size_t destX, size_t destY)
	{
		if(Chunk* src = getChunk(x, y))
			if(Chunk* dst = getChunk(destX, destY))
				dst->moveCell(src, x, y, destX, destY);
	}

	Chunk* getChunk(int x, int y)
	{
		auto location = getChunkLocation(x, y);
		Chunk* chunk = getChunkDirect(location);
		return chunk ? chunk : createChunk(location);
	}

	Chunk* getChunkDirect(std::pair<int, int> location)
	{
		auto itr = chunkLookup.find(location);
		auto end = chunkLookup.end();
		return itr != end ? itr->second : nullptr;
	}

	std::pair<int, int> getChunkLocation(int x, int y)
	{
		std::pair<int, int> l = {floor(static_cast<float>(x) / chunkDimensions.x),
				 floor(static_cast<float>(y) / chunkDimensions.y)};
		//std::cout << "(" << l.first << ", " << l.second << ")\n";
		return l;
	}
	
	Chunk* createChunk(std::pair<int, int> location)
	{
		auto [lx, ly] = location;
		if (lx < 0 || ly < 0 ||
			lx >= dimensions.x / chunkDimensions.x || ly >= dimensions.y / chunkDimensions.y)
		{
			return nullptr;
		}

		Chunk* chunk = new Chunk(chunkDimensions, { lx, ly }, renderer);
		chunkLookup.insert({ location, chunk });

		{
			std::unique_lock lock(chunkMutex);
			chunks.push_back(chunk);
		}

		return chunk;
	}

	void updateThreaded()
	{
		std::mutex mutex;
		std::condition_variable cond;
		auto doForAllChunks = [&](std::function<void(Chunk*)> func)
		{
			int chunkCount = chunks.size();
			std::vector<std::thread> tasks(chunks.size());
			for (int i = 0; i < chunks.size(); i++)
			{
				Chunk* chunk = chunks[i];
				tasks[i] = std::thread([&, chunk]() {
					func(chunk);
					{
						std::unique_lock lock(mutex);
						chunkCount--;
					}
					//cond.notify_one();
					});
			}
			for (std::thread& th : tasks)
				th.join();
			{
				std::unique_lock lock(mutex);
				cond.wait(lock, [&]() {return chunkCount == 0; });
			}
		};
		doForAllChunks([&](Chunk* chunk) {
			SimpleWorker(*this, chunk).updateChunk();
		});
		doForAllChunks([&](Chunk* chunk) {
			chunk->commit();
			});
	}

	void updateNotThreaded()
	{
		for (int i = 0; i < chunks.size(); i++)
			SimpleWorker(*this, chunks[i]).updateChunk();
		for (Chunk* ch : chunks)
			ch->commit();
	}

	void update()
	{
		removeEmptyChunks();
		cellsCount = 0;
		
		//updateThreaded();
		updateNotThreaded();


		for (Chunk* ch : chunks)
		{
			cellsCount += ch->getCellsCount();
			ch->updateTexture();
			ch->updateRect();
		}
		updateTexture();
	}


	bool MoveDown(size_t x, size_t y, Cell* cell)
	{
		if (Chunk * chunk = getChunk(x, y+1))
		{
			if (chunk->canGoHere(x, y + 1, cell))
			{
				moveCell(x, y, x, y + 1);
				return true;
			}
		}
		return false;
	}

	bool MoveDownSide(size_t x, size_t y, Cell* cell)
	{
		int delta = Randomiser::rand() % 2 * 2 - 1;
		if (Chunk* chunk = getChunk(x + delta, y + 1))
		{
			if (chunk->canGoHere(x + delta, y + 1, cell))// && canGoHere(x + delta, y, cell))
			{
				moveCell(x, y, x + delta, y + 1);
				return true;
			}
		}
		else if (Chunk* chunk = getChunk(x + delta, y + 1))
		{
			if (chunk->canGoHere(x - delta, y + 1, cell))// && canGoHere(x - delta, y, cell))
			{
				moveCell(x, y, x - delta, y + 1);
				return true;
			}
		}
		return false;
	}

	bool MoveSide(size_t x, size_t y, Cell* cell)
	{
		int delta = Randomiser::rand() % 2 * 2 - 1;
		if (Chunk* chunk = getChunk(x + delta, y))
		{
			if (chunk->canGoHere(x + delta, y, cell))// && canGoHere(x + delta, y, cell))
			{
				moveCell(x, y, x + delta, y);
				return true;
			}
		}
		else if (Chunk* chunk = getChunk(x + delta, y))
		{
			if (chunk->canGoHere(x - delta, y, cell))// && canGoHere(x - delta, y, cell))
			{
				moveCell(x, y, x - delta, y);
				return true;
			}
		}
		return false;
	}

	void removeEmptyChunks()
	{
		for (size_t i = 0; i < chunks.size(); i++)
		{
			Chunk* chunk = chunks[i];
			if (chunk->empty())
			{

				chunkLookup.unsafe_erase(std::pair<int,int>(chunk->position.x, chunk->position.y ));
				chunks.erase(chunks.begin() + i);
				i--;
				delete chunk;
			}
		}
	}

};

