#pragma once
#include "Color.h"
#include <unordered_map>

enum class CellProperties {
	NONE =           0b00000001,
	MOVE_DOWN =      0b00000010,
	MOVE_DOWN_SIDE = 0b00000100,
	MOVE_SIDE =      0b00001000,
};

enum class CellType {
	EMPTY = 0b00000001,
	SAND =  0b00000010,
	WATER = 0b00000100,
	ROCK =  0b00001000,
	ALL =   0b11111111,
};

inline CellProperties operator|(CellProperties a, CellProperties b)
{
	return CellProperties(static_cast<int>(a) | static_cast<int>(b));
}

inline auto operator&(CellProperties a, CellProperties b)
{
	return CellProperties(static_cast<int>(a) & static_cast<int>(b));
}

inline CellType operator|(CellType a, CellType b)
{
	return CellType(static_cast<int>(a) | static_cast<int>(b));
}

inline CellType operator&(CellType a, CellType b)
{
	return CellType(static_cast<int>(a) & static_cast<int>(b));
}

struct Cell
{
	const char* name = "Cell";
	CellType type = CellType::EMPTY;
	CellProperties props = CellProperties::NONE;
	Color color = Color(0, 0, 0, 0);
	CellType weaker = CellType::EMPTY;
};

static Cell EMPTY = { "Empty", CellType::EMPTY, CellProperties::NONE, Color{0, 0, 0, 0}, CellType::ALL};
static Cell SAND = { "Sand", CellType::SAND, CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE, Color(180,150, 20, 255), CellType::EMPTY | CellType::WATER};
static Cell WATER = { "Water", CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE | CellProperties::MOVE_SIDE, Color(5, 150, 255, 150)};
static Cell ROCK = { "Rock", CellType::ROCK, CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE, Color(80, 80, 80, 255), CellType::EMPTY | CellType::WATER};
static Cell WALL = { "Wall", CellType::ROCK, CellProperties::NONE, Color(255, 100, 20, 255), CellType::ALL};
static Cell WEIRDO = { "Weirdo", CellType::WATER, CellProperties::MOVE_SIDE, Color(0, 255, 0, 255), CellType::ALL};
