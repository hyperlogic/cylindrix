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

#ifndef STATS_H

#define STATS_H


typedef struct
    {
     char           name[80];       /* This characters name */
     unsigned short kills;          /* How many enemies you killed */
     unsigned short times_killed;   /* How many times character was killed */
     unsigned short shots_fired;    /* How many projectiles you've fired */
     unsigned short enemy_hits;     /* How many times you hit an enemy with a projectile */
     unsigned short friendly_hits;  /* How many times you hit a friend with a projectile */
     unsigned short misses;         /* How many shots you fired that did nothing */
     unsigned short times_hit;      /* How many times you were hit with projectiles */
     int            victory;        /* Did this guy win or lose? 1 or 0 */
     float          hit_percentage; /* Hit to miss ratio */
     unsigned short pylons_grabbed; /* How many pylons you grabbed */
     float          average_d;      /* Your average d throughout the game */

     int            user_vehicle;   /* Current vehicle controlled by user...for internal use */
     unsigned long  total_frames;   /* Number of frames a game took...for internal use */
     unsigned long  total_d;        /* Added d from every frame...used to compute average d */
    } game_stats_type;


typedef struct
    {
     char          name[80];        /* This characters name */
     unsigned long number_of_games; /* How many games you have played (unsigned long...hehe) */
     unsigned long victories;       /* Number of victories */
     unsigned long defeats;         /* Number of defeats */
     float         win_percentage;  /* Percent of the time you won */
     unsigned long kills;           /* How many cats has this cat killed */
     unsigned long times_killed;    /* How many times character was killed */
     unsigned long shots_fired;     /* How many projectiles you've fired */
     unsigned long enemy_hits;      /* How many times you hit an enemy with a projectile */
     unsigned long friendly_hits;   /* How many times you hit an friend with a projectile */
     unsigned long misses;          /* How many shots you fired that did nothing */
     unsigned long times_hit;       /* How many times you were hit with projectiles */
     float         hit_percentage;  /* Hit to miss ratio */
     unsigned long pylons_grabbed;  /* How many pylons you grabbed */
    } overall_stats_type;


/* Save the stats for this game into a text file for the user */
int Save_Game_Stats( game_stats_type *game_stats, char *filename );

/* Save this players overall stats for into a text file */
int Save_Overall_Stats_Text( overall_stats_type *overall_stats, char *filename );

/* Save this players overall stats for into a binary file (used by game) */
int Save_Overall_Stats_Binary( overall_stats_type *overall_stats, const char *filename );

/* Load this players overall stats from a binary file */
int Load_Overall_Stats_Binary( overall_stats_type *overall_stats, const char *filename );

/* Update this characters overall stats with this game's stats */
void Add_Game_Stats_To_Overall_Stats( game_stats_type *game_stats, overall_stats_type *overall_stats );

void Clear_Game_Stats( game_stats_type *game_stats );
void Clear_Overall_Stats( overall_stats_type *overall_stats );

void Figure_Game_Stats( game_stats_type *game_stats );

#endif
