#pragma once
#include "Vector4.h"
#include "Vector2.h"
#include "MyRenderer.h"
#include <vector>
#include "Cell.h"
#include "AbstractMap.h"
#include <algorithm>
#include <mutex>
using std::vector;

const int EMPTY_FRAMES_DELETION_DELAY = 10;

int clamp(int number, int min, int max);
const int DIRTY_RECT_BUFFER_SIZE = 2;

class Chunk
{
private:
	std::mutex filledCellCountMutex;
	std::mutex changesMutex;
	std::mutex workingRectMutex;
public:
	const Vector2 dimensions;
	const Vector2 position;

	int dirtyRectX1, dirtyRectX2, dirtyRectY1, dirtyRectY2;
protected:
	int dirtyRectX1W, dirtyRectX2W, dirtyRectY1W, dirtyRectY2W;

	Cell** board;
	unsigned char* pixels;
	SDL_Texture* texture;
	bool commitAlternator = false;
	vector<std::tuple<Chunk*, size_t, size_t>> moveChanges; //source chunk, destination, source
	MyRenderer& renderer;
	int cells;
public:
	int emptyFrames;
	Chunk(Vector2 dimensions, 
		Vector2 position, 
		MyRenderer& renderer
	) :
		dimensions(dimensions),
		position(position),
		board(new Cell*[dimensions.x * dimensions.y]),
		pixels(new unsigned char[dimensions.x * dimensions.y * 4]),
		renderer(renderer),
		cells(0),
		emptyFrames(0),
		dirtyRectX1W(dimensions.x),
		dirtyRectX2W(-1),
		dirtyRectY1W(dimensions.y),
		dirtyRectY2W(-1)
	{
		for (int i = 0; i < dimensions.x * dimensions.y; i++)
			board[i] = &EMPTY;

		texture = SDL_CreateTexture(renderer.getRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, dimensions.x, dimensions.y);

		updateTexture();
	}

	void updateTexture()
	{
		for (int i = 0; i < dimensions.y * dimensions.x; i++)
		{
			pixels[i * 4] = board[i]->color.b;
			pixels[i * 4 + 1] = board[i]->color.g;
			pixels[i * 4 + 2] = board[i]->color.r;
			pixels[i * 4 + 3] = board[i]->color.a;
		}
		SDL_UpdateTexture(texture, NULL, pixels, dimensions.x * 4);
	}

	inline size_t getIndex(size_t x, size_t y) {
		return (x - position.x * dimensions.x)  + (y - position.y * dimensions.y) * dimensions.x;
	}

	Cell* getCell(size_t index) {
		return (board[index]);
	}

	Cell* getCell(size_t x, size_t y) {
		return (board[getIndex(x, y)]);
	}

	bool inBounds(size_t x, size_t y) 
	{ 
		return x >= position.x * dimensions.x && x < position.x * dimensions.x + dimensions.x
			&& y >= position.y * dimensions.y && y < position.y * dimensions.y + dimensions.y;
	}

	bool isEmpty(size_t x, size_t y) 
	{ 
		return inBounds(x, y) && getCell(x, y)->type == CellType::EMPTY; 
	}

	bool canGoHere(size_t x, size_t y, Cell* cell) 
	{ 
		return inBounds(x,y) && (bool)(cell->weaker & getCell(x, y)->type);
	}

	void setCell(size_t x, size_t y, Cell* cell)
	{
		setCell(getIndex(x, y), cell);
	}


	void setCell(size_t index, Cell* cell)
	{
		if (cell->type != board[index]->type)
		{
			if (board[index]->type == CellType::EMPTY)
			{
				std::unique_lock lock(filledCellCountMutex);
				cells++;
			}
			else
			{
				std::unique_lock lock(filledCellCountMutex);
				cells--;
			}
			emptyFrames = 0;
		}

		board[index] = cell;
		keepAlive(index);
	}

	void moveCell(Chunk* source, size_t x, size_t y, size_t destX, size_t destY)
	{
		size_t src = source->getIndex(x, y);
		size_t dst = getIndex(destX, destY);
		std::unique_lock lock(changesMutex);
		moveChanges.emplace_back(source, src, dst);
	}

	void commit()
	{
		if (commitAlternator)
		{
			for (size_t i = 0; i < moveChanges.size(); i++)
			{
				auto [chunk, src, dst] = moveChanges[i];

				if (board[dst]->type != chunk->getCell(src)->type)
				{
					Cell* buff = board[dst];
					setCell(dst, chunk->getCell(src));
					chunk->setCell(src, buff);
				}
			}
		}
		else
		{
			for (int i = moveChanges.size()-1; i >= 0; i--)
			{
				auto [chunk, src, dst] = moveChanges[i];

				if (board[dst]->type != chunk->getCell(src)->type)
				{
					Cell* buff = chunk->getCell(src);
					chunk->setCell(src, board[dst]);
					setCell(dst, buff);
					
				}
			}
		}
		commitAlternator = !commitAlternator;
		moveChanges.clear();
	}

	SDL_Texture* getTexture() {
		return texture;
	}

	bool empty() {
		if (cells == 0)
			emptyFrames++;
		return emptyFrames > EMPTY_FRAMES_DELETION_DELAY;
	}

	void keepAlive(int x, int y) {
		keepAlive(getIndex(x, y));
	}

	void keepAlive(size_t index)
	{
		int x = index % dimensions.x;
		int y = index / dimensions.x;
		
		std::unique_lock lock(workingRectMutex);

		dirtyRectX1W = clamp(std::min(x - DIRTY_RECT_BUFFER_SIZE, dirtyRectX1W), 0, dimensions.x-1);
		dirtyRectY1W = clamp(std::min(y - DIRTY_RECT_BUFFER_SIZE, dirtyRectY1W), 0, dimensions.y-1);
		dirtyRectX2W = clamp(std::max(x + DIRTY_RECT_BUFFER_SIZE, dirtyRectX2W), 0, dimensions.x-1);
		dirtyRectY2W = clamp(std::max(y + DIRTY_RECT_BUFFER_SIZE, dirtyRectY2W), 0, dimensions.y-1);

		if (dirtyRectX2W >= dimensions.x || dirtyRectX1W < 0 || dirtyRectY2W >= dimensions.y || dirtyRectY1W < 0)
			printf("dirty overflow chunk.h keepAlive 185\n");
		emptyFrames = 0;
	}

	void updateRect()
	{
		dirtyRectX1 = dirtyRectX1W; dirtyRectX1W = dimensions.x;
		dirtyRectY1 = dirtyRectY1W; dirtyRectY1W = dimensions.y;
		dirtyRectX2 = dirtyRectX2W; dirtyRectX2W = -1;
		dirtyRectY2 = dirtyRectY2W; dirtyRectY2W = -1;
	}

	int getCellsCount() {
		return cells;
	}
};



