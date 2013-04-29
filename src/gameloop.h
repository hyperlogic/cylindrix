/*
    Copyright (C) 2001 Hotwarez LLC, Goldtree Enterprises
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef GAMELOOP_H
#define GAMELOOP_H

void GameInit( int argc, const char* argv[] );
int GameLoop();
void GameShutdown();

void IntroEnter();
void IntroProcess();
void IntroExit();

void MainMenuEnter();
void MainMenuProcess();
void MainMenuExit();

void PlayCustomGameEnter();
void PlayCustomGameProcess();
void PlayCustomGameExit();

void PlayTournamentGameEnter();
void PlayTournamentGameProcess();
void PlayTournamentGameExit();

void OutroEnter();
void OutroProcess();
void OutroExit();

void PlayGameMovieEnter();
void PlayGameMovieProcess();
void PlayGameMovieExit();

void PlayGameOpponentEnter();
void PlayGameOpponentProcess();
void PlayGameOpponentExit();

void PlayGameIntroEnter();
void PlayGameIntroProcess();
void PlayGameIntroExit();

void PlayGameEnterVehicleEnter();
void PlayGameEnterVehicleProcess();
void PlayGameEnterVehicleExit();

void PlayGameFightEnter();
void PlayGameFightProcess();
void PlayGameFightExit();

void PlayGameOutroEnter();
void PlayGameOutroProcess();
void PlayGameOutroExit();

void PlayGameMenuEnter();
void PlayGameMenuProcess();
void PlayGameMenuExit();

void PlayGameStatsEnter();
void PlayGameStatsProcess();
void PlayGameStatsExit();

void PlayGameWinEnter();
void PlayGameWinProcess();
void PlayGameWinExit();

#endif

