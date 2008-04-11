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

#include "stats.h"
#include "util.h"
#include "keys.h"

#include <stdio.h>

extern char g_DataPath[255];

int Save_Game_Stats( game_stats_type *game_stats, char *filename )
{
    FILE *fp;    
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);

    
    if( (fp = fopen( newfilename, "w" ) ) == NULL ) 
	{
		SYS_Error( "Save_Game_Stats() : fopen() error" );
    }
    
    fprintf( fp, "Game Stats\n");       
    fprintf( fp, "----------\n");
    fprintf( fp, "Pilot: %s\n", game_stats->name );
    fprintf( fp, "Kills: %d\n", (int)game_stats->kills );
    fprintf( fp, "Deaths: %d\n", (int)game_stats->times_killed );
    fprintf( fp, "Total Shots Fired: %d\n", (int)game_stats->shots_fired );
    fprintf( fp, "Enemy Hits: %d\n", (int)game_stats->enemy_hits );
    fprintf( fp, "Friendly Hits: %d\n", (int)game_stats->friendly_hits );
    fprintf( fp, "Missed Shots: %d\n", (int)game_stats->misses );
    fprintf( fp, "Hit Percentage: %.3f%%\n", game_stats->hit_percentage * 100.0 );
    fprintf( fp, "Pylons Grabbed: %d\n", (int)game_stats->pylons_grabbed );
    fprintf( fp, "Average D Level: %.3f\n", game_stats->average_d );
    fprintf( fp, "Enemy Hits Taken: %d\n", (int)game_stats->times_hit );
    fprintf( fp, "\n" );
    if( game_stats->victory ) {
        fprintf( fp, "%s was victorious.\n", game_stats->name );
    }
    else {
        fprintf( fp, "%s was defeted.\n", game_stats->name );
    }
    fprintf( fp, "\n" );
    
    fclose( fp );
    
    return 1;
     
} /* End of Save_Game_Stats() */


int Save_Overall_Stats_Text( overall_stats_type *overall_stats, char *filename )
{
    FILE *fp;    
   	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);

 
    if( (fp = fopen( newfilename, "w" ) ) == NULL ) 
	{
        SYS_Error( "Save_Overall_Stats_Text() : fopen() error" );
    }
    
    fprintf( fp, "Overall Stats\n");       
    fprintf( fp, "-------------\n");
    fprintf( fp, "Pilot: %s\n", overall_stats->name );
    fprintf( fp, "Total Games: %d\n", (int)overall_stats->number_of_games );
    fprintf( fp, "Victories: %d\n", (int)overall_stats->victories );
    fprintf( fp, "Defeats: %d\n", (int)overall_stats->defeats );
    fprintf( fp, "Win Percentage: %.3f\n", overall_stats->win_percentage * 100.0 );
    fprintf( fp, "Kills: %d\n", (int)overall_stats->kills );
    fprintf( fp, "Deaths: %d\n", (int)overall_stats->times_killed );
    fprintf( fp, "Total Shots Fired: %d\n", (int)overall_stats->shots_fired );
    fprintf( fp, "Enemy Hits: %d\n", (int)overall_stats->enemy_hits );
    fprintf( fp, "Friendly Hits: %d\n", (int)overall_stats->friendly_hits );
    fprintf( fp, "Missed Shots: %d\n", (int)overall_stats->misses );
    fprintf( fp, "Hit Percentage: %.3f%%\n", overall_stats->hit_percentage * 100.0 );
    fprintf( fp, "Pylons Grabbed: %d\n", (int)overall_stats->pylons_grabbed );
    fprintf( fp, "Enemy Hits Taken: %d\n", (int)overall_stats->times_hit );
    fprintf( fp, "\n" );    
        
    fclose( fp );
    
    return 1;

} /* End of Save_Overall_Stats_Text() */


int Save_Overall_Stats_Binary( overall_stats_type *overall_stats, char *filename )
    {
     FILE *fp;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);



     fp = fopen( newfilename, "wb" );

     if( fp == NULL )
         return(0);

     fwrite( overall_stats, sizeof(overall_stats_type), 1, fp );

     fclose(fp);

     return(1);
    } /* End of Save_Overall_Stats_Binary() */


int Load_Overall_Stats_Binary( overall_stats_type *overall_stats, char *filename )
    {
     FILE *fp;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);



     fp = fopen( newfilename, "rb" );

     if( fp == NULL )
         return(0);

     fread( overall_stats, sizeof(overall_stats_type), 1, fp );

     fclose(fp);

     return(1);
    } /* End of Load_Overall_Stats_Binary() */


void Add_Game_Stats_To_Overall_Stats( game_stats_type *game_stats, overall_stats_type *overall_stats )
    {
     overall_stats->number_of_games++;

     if( game_stats->victory )
         overall_stats->victories++;
     else
         overall_stats->defeats++;


     overall_stats->times_hit      += game_stats->times_hit;
     overall_stats->shots_fired    += game_stats->shots_fired;
     overall_stats->enemy_hits     += game_stats->enemy_hits;
     overall_stats->friendly_hits  += game_stats->friendly_hits;
     overall_stats->misses         += game_stats->misses;

     overall_stats->pylons_grabbed += game_stats->pylons_grabbed;

     overall_stats->times_killed   += game_stats->times_killed;
     overall_stats->kills          += game_stats->kills;


     if( overall_stats->number_of_games == 0 )
         overall_stats->win_percentage = 0;
     else
         overall_stats->win_percentage = (float)overall_stats->victories  / (float)overall_stats->number_of_games;

     if( overall_stats->shots_fired == 0 )
         overall_stats->hit_percentage = 0;
     else
         overall_stats->hit_percentage = (float)overall_stats->enemy_hits / (float)overall_stats->shots_fired;


    } /* End of Add_Game_Stats_To_Overall_Stats() */


void Clear_Game_Stats( game_stats_type *game_stats )
    {     
     game_stats->kills          = 0;
     game_stats->times_killed   = 0;
     game_stats->shots_fired    = 0;
     game_stats->enemy_hits     = 0;
     game_stats->friendly_hits  = 0;
     game_stats->misses         = 0;
     game_stats->times_hit      = 0;
     game_stats->victory        = 0;
     game_stats->hit_percentage = 0;
     game_stats->pylons_grabbed = 0;
     game_stats->average_d      = 0;
     game_stats->total_frames   = 0;
     game_stats->user_vehicle   = 0;
     game_stats->total_d        = 0;
    } /* End of Clear_Game_Stats() */


void Clear_Overall_Stats( overall_stats_type *overall_stats )
    {     
     overall_stats->number_of_games = 0;
     overall_stats->victories       = 0;
     overall_stats->defeats         = 0;
     overall_stats->win_percentage  = 0;
     overall_stats->kills           = 0;
     overall_stats->times_killed    = 0;
     overall_stats->shots_fired     = 0;
     overall_stats->enemy_hits      = 0;
     overall_stats->friendly_hits   = 0;
     overall_stats->misses          = 0;
     overall_stats->times_hit       = 0;
     overall_stats->hit_percentage  = 0;
     overall_stats->pylons_grabbed  = 0;
    } /* End of Clear_Overall_Stats() */


void Figure_Game_Stats( game_stats_type *game_stats )
    {
     game_stats->misses         = game_stats->shots_fired - (game_stats->friendly_hits + game_stats->enemy_hits);


     if( game_stats->shots_fired == 0 )
         game_stats->hit_percentage = 0;
     else
         game_stats->hit_percentage = (float)(game_stats->friendly_hits + game_stats->enemy_hits) / (float)game_stats->shots_fired;

     if( game_stats->total_frames == 0 )
         game_stats->average_d = 0;
     else
         game_stats->average_d      = (float)game_stats->total_d / game_stats->total_frames;

    } /* End of Figure_Game_Stats() */
