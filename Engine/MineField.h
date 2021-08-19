#pragma once

#include "Graphics.h"
#include "SpriteCodex.h"

class MineField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden = 0,
			Flagged,
			Revealed
		};
	public:
		void SpawnMine();
		bool HasMine() const;
		//besides Draw in MineField, we need a Draw for a tile
		//besides gfx, we need to know the pixel coords to put them on the screen
		//Vei2 will represent 2 concepts - grid pos and pixel pos
		void Draw(const Vei2& screenPos, bool blown, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SetNeighborMineCount(int mineCount);
		bool HasNoNeighborMines() const;
	private:
		State state = State::Hidden;
		bool hasMine = false;
		int nNeighborMines = -1;
	};
public:
	enum class PlayerState
	{
		Looser = 0,
		Winner = 1,
		Ongoing = 2
	};
	MineField(int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
	//to flag a tile
	void OnFlagClick(const Vei2& screenPos);
	static PlayerState playerState;
private:
	static constexpr int width = 15;
	static constexpr int height = 15;
	static constexpr int nFields = width * height;
	Tile field[width * height];
	bool isBlown = false;
	void RevealTile(const Vei2& gridPos);
private:
	//if the fn is const you cannot return a non-const reference!!
	//you can create another version for this fn - the have identical code, but one of them is a const version
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	const Vei2 ScreenToGrid(const Vei2& screenPos);
	int CountNeighborMines(const Vei2& gridPos);
	const Vei2 startPoint{ Graphics::ScreenWidth / 2 - ((width * SpriteCodex::tileSize) / 2),  Graphics::ScreenHeight / 2 - ((height * SpriteCodex::tileSize) / 2) };
	bool areAllRevealed();
	static constexpr int borderThickness = 10;
	static constexpr Color borderColor = Colors::Blue;
};

