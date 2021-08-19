/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	field(20),
	soundGameOver(L"spayed.wav")
	
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	//flag blinking - if clicked, the event spans accross frames until the btn is realeased
	//fns are called each time
	//detect when it actually gets pressed - dont check it each frame
	//event processing in this framework in the mouse class
		//if (wnd.mouse.LeftIsPressed())
		//{
		//	//check if the cursor is inside the rect of the field
		//	const Vei2 mousePos = wnd.mouse.GetPos();
		//	if (field.GetRect().Contains(mousePos))
		//		field.OnRevealClick(mousePos);
		//}
		//else if (wnd.mouse.RightIsPressed())
		//{
		//	//check if the cursor is inside the rect of the field
		//	const Vei2 mousePos = wnd.mouse.GetPos();
		//	if (field.GetRect().Contains(mousePos))
		//		field.OnFlagClick(mousePos);
		//}

	//while there are events in the queue to be processed
	while (!wnd.mouse.IsEmpty())
	{
		const auto e = wnd.mouse.Read();
		if (e.GetType() == Mouse::Event::Type::LPress)
		{
			//check if the cursor is inside the rect of the field
			const Vei2 mousePos = e.GetPos();
			if (field.GetRect().Contains(mousePos))
				field.OnRevealClick(mousePos);
		}
		else if (e.GetType() == Mouse::Event::Type::RPress)
		{
			//check if the cursor is inside the rect of the field
			const Vei2 mousePos = e.GetPos();
			if (field.GetRect().Contains(mousePos))
				field.OnFlagClick(mousePos);
		}
	}
}

void Game::ComposeFrame()
{
	if (field.playerState == MineField::PlayerState::Ongoing)
		field.Draw(gfx);
	else if (field.playerState == MineField::PlayerState::Winner)
	{
		Vei2 winPos = { 400,300 };
		SpriteCodex::DrawWin(winPos, gfx);
	}
	else
		soundGameOver.Play();
}
