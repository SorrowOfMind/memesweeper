#include "MineField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"

MineField::PlayerState MineField::playerState = MineField::PlayerState::Ongoing;

void MineField::Tile::SpawnMine()
{
    assert(!hasMine);//only spawn on tiles without a bomb
    hasMine = true;
}

bool MineField::Tile::HasMine() const
{
    return hasMine;
}

void MineField::Tile::Draw(const Vei2& screenPos, bool blown, Graphics& gfx) const
{
    if (!blown)
    {
        switch (state)
        {
        case State::Hidden: //btn looking thing
            SpriteCodex::DrawTileButton(screenPos, gfx);
            break;
        case State::Flagged:
            SpriteCodex::DrawTileButton(screenPos, gfx);
            SpriteCodex::DrawTileFlag(screenPos, gfx);
            break;
        case State::Revealed:
            //has a mine or doesn't have a mine
            if (!HasMine())
            {
                SpriteCodex::DrawTileNumber(screenPos, nNeighborMines, gfx);
            }
            else
            {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
            }
            break;
        }
    }
    else //reveal all the mines and mistakes with flags
    {
        switch (state)
        {
        case State::Hidden: //btn looking thing
            if (HasMine())
            {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
            }
            SpriteCodex::DrawTileButton(screenPos, gfx);
            break;
        case State::Flagged:
            if (HasMine()) //flag on top of the mine
            {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
                SpriteCodex::DrawTileFlag(screenPos, gfx);
            }
            else //cross over mine
            {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
                SpriteCodex::DrawTileCross(screenPos, gfx);
            }
            break;
        case State::Revealed:
            //has a mine or doesn't have a mine
            if (!HasMine())
            {
                SpriteCodex::DrawTileNumber(screenPos, nNeighborMines, gfx);
            }
            else
            {   //the bomb you triggered
                SpriteCodex::DrawTileBombRed(screenPos, gfx);
            }
            break;
        }
    }
}

void MineField::Tile::Reveal()
{
    assert(state == State::Hidden);
    state = State::Revealed;
}

bool MineField::Tile::IsRevealed() const
{
    return state == State::Revealed;
}

void MineField::Tile::ToggleFlag()
{
    assert(!IsRevealed());
    if (state == State::Hidden)
        state = State::Flagged;
    else
        state = State::Hidden;

}

bool MineField::Tile::IsFlagged() const
{
    return state == State::Flagged;
}

void MineField::Tile::SetNeighborMineCount(int mineCount)
{
    assert(nNeighborMines == -1);
    nNeighborMines = mineCount;
}

void MineField::OnRevealClick(const Vei2& screenPos)
{
    //first check if is over
    if (!isBlown && playerState == PlayerState::Ongoing)
    {
        //convert screenPos to gridPos
        const Vei2 gridPos = ScreenToGrid(screenPos);
        //assert(gridPos.x > 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
        //TODO: 
        assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);

        Tile& tile = TileAt(gridPos);
        if (!tile.IsRevealed() && !tile.IsFlagged())
        {
            tile.Reveal();
            if (tile.HasMine())
            {
                isBlown = true;
                playerState = PlayerState::Looser;
            }
            else if (areAllRevealed())
                playerState = PlayerState::Winner;
        }
    }
}

void MineField::OnFlagClick(const Vei2& screenPos)
{
    if (!isBlown && playerState == PlayerState::Ongoing)
    {
        //convert screenPos to gridPos
        const Vei2 gridPos = ScreenToGrid(screenPos);
        assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);

        Tile& tile = TileAt(gridPos);
        //create a fn in the Tile to toggle a flag
        if (!tile.IsRevealed())
        {
            tile.ToggleFlag();
            if (areAllRevealed())
                playerState = PlayerState::Winner;
        }
    }
}

MineField::MineField(int nMines)
{
    assert(nMines > 0 && nMines < nFields);
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xDist(0, width - 1);
    std::uniform_int_distribution<int> yDist(0, height - 1);

    for (int nSpawned = 0; nSpawned < nMines; ++nSpawned)
    {
        Vei2 spawnPos;
        do
        {
            spawnPos = { xDist(rng), yDist(rng) }; //randomly generated position
        }
        while (TileAt(spawnPos).HasMine()); //check if there is already a bomb in that position - you need to map the grid to 1D array - separete fn for that

        TileAt(spawnPos).SpawnMine();
    }

    //reveal test
  /*  for (int i = 0; i < 120; i++)
    {
        const Vei2 gridPos = { xDist(rng), yDist(rng) };
        if (!TileAt(gridPos).IsRevealed())
            TileAt(gridPos).Reveal();
    }*/

    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
    {
        for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
        {
            TileAt(gridPos).SetNeighborMineCount(CountNeighborMines(gridPos));
        }
    }

}

void MineField::Draw(Graphics& gfx) const
{   
    //the background of the tiles is currently black - draw a big rect for the whole grid underneath the tiles
    gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
    //instead of i,j - we can make one initializer {0,0}
    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) //rows
    {
        for (gridPos.x = 0; gridPos.x < width; gridPos.x++) //cols
        {
            TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize + startPoint, isBlown, gfx);
        }
    }
}

RectI MineField::GetRect() const
{
    //return RectI(0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize);
    return RectI(startPoint.x, startPoint.x + width * SpriteCodex::tileSize, startPoint.y, startPoint.y + height * SpriteCodex::tileSize);
}

MineField::Tile& MineField::TileAt(const Vei2& gridPos)
{
    return field[gridPos.y * width + gridPos.x];
}

const MineField::Tile& MineField::TileAt(const Vei2& gridPos) const
{
    return field[gridPos.y * width + gridPos.x];
}

const Vei2 MineField::ScreenToGrid(const Vei2& screenPos)
{
    //return (screenPos / SpriteCodex::tileSize);
    return ((screenPos - startPoint) / SpriteCodex::tileSize);
}

int MineField::CountNeighborMines(const Vei2& gridPos)
{
    const int xStart = std::max(0, gridPos.x - 1); //if at the edge gridPos.x - 1 will give us 0
    const int yStart = std::max(0, gridPos.y - 1);
    const int xEnd = std::min(width - 1, gridPos.x + 1);
    const int yEnd = std::min(height - 1, gridPos.y + 1);

    int count = 0;
    for (Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; gridPos.y++)
    {
        for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
        {
            if (TileAt(gridPos).HasMine())
                count++;
        }
    }
    return count;
}

bool MineField::areAllRevealed()
{
    bool allRevealed = true;
    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) //rows
    {
        for (gridPos.x = 0; gridPos.x < width; gridPos.x++) //cols
        {
            allRevealed = allRevealed && ((TileAt(gridPos).IsFlagged() && TileAt(gridPos).HasMine()) 
                || (TileAt(gridPos).IsRevealed() && !TileAt(gridPos).HasMine()));
        }
    }

    return allRevealed;
}
