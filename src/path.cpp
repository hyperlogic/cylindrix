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


#include "types.h"
#include "path.h"


extern player_events_type player_events[6];

Float_Point2d centers_array[TUBE_ROWS][TUBE_COLUMNS];
char pylon_grid[TUBE_ROWS][TUBE_COLUMNS];
char path_grid[TUBE_ROWS][TUBE_COLUMNS];

Float_Point   centers_array_3D[TUBE_ROWS][TUBE_COLUMNS][TUBE_HEIGHT];
char pylon_grid_3D[TUBE_ROWS][TUBE_COLUMNS][TUBE_HEIGHT];
char path_grid_3D[TUBE_ROWS][TUBE_COLUMNS][TUBE_HEIGHT];


typedef struct
    {
     Float_Point2d  destination; /* Your current destination */
     Float_Point2d  position;    /* Your current position    */
     
     int          dest_row;      /* Row and column of your destination */
     int          dest_column;
     
     int          row;           /* Your current row and column */
     int          column;   
    } rows_type;

typedef struct
    {
     Float_Point  destination; /* Your current destination */
     Float_Point  position;    /* Your current position    */
     
     int          dest_row;      /* Row, column and height of your destination */
     int          dest_column;
     int          dest_height;     
     
     int          row;           /* Your current row and column and height */
     int          column;   
     int          height;
    } rows_type_3D;


/* This enumeration type is for the recursive function One_Step_2D... it determines what direction
   the function is checking */
typedef enum { GRID_UP, GRID_DOWN, GRID_LEFT, GRID_RIGHT, GRID_START } direction_type;


void Init_Pylon_Grid( WorldStuff *world_stuff )
    {
     Pylons *pylons;
     int i, j, k;
     int row, column;
     Float_Point2d point;
     
     
     pylons = &world_stuff->pylons;
     
     for( i = 0; i < TUBE_ROWS; i++ )
         for( j = 0; j < TUBE_COLUMNS; j++ )
              pylon_grid[i][j] = 0;
     
     for( i = 0; i < pylons->pylons; i++ )
         {
          Pylon_Position_2D( &pylons->pylon[i], point );
          Point_Square_2D( point, &row, &column );
          
          if( pylon_grid[row][column] == 1 )
              {
               printf("ERROR\nERROR\nERROR\n");
              }
          pylon_grid[row][column] = 1;
         }



     for( i = 0; i < TUBE_ROWS; i++ )
         for( j = 0; j < TUBE_COLUMNS; j++ )
             for( k = 0; k < TUBE_HEIGHT; k++ ) 
                 pylon_grid_3D[i][j][k] = 0;

     for( i = 0; i < pylons->pylons; i++ )
         {
          Pylon_Position_2D( &pylons->pylon[i], point );
          Point_Square_2D( point, &row, &column );
          
          if( pylon_grid_3D[row][column][0] == 1 )
              {
               printf("ERROR\nERROR\nERROR\n"); 
              }
          pylon_grid_3D[row][column][0] = 1;
          
          /* Now we loop through the height of the cylinder to see how much
             this pylon fills up.Right now there is a one to one relationship 
             between the radius and the tube height columns  */
          for( j = 0; j < TUBE_HEIGHT; j++ ) 
              {
               if( (pylons->pylon[i].height) >= j )
                   pylon_grid_3D[row][column][j] = 1;
              }
         } /* End of loop through pylons */ 


     /*
     for( k = 0; k < TUBE_HEIGHT; k++ ) 
         {
          printf("Height %d \n", k );
          for( i = 0; i < TUBE_ROWS; i++ )
              {
               for( j = 0; j < TUBE_COLUMNS; j++ )
                   {
                    printf("%d ", pylon_grid_3D[i][j][k] );
                   }
                printf("\n");
               }
         }
     */

    } /* End of Init_Pylon_Grid */
    

    
void Init_Centers_Array( WorldStuff *world_stuff )
    {
     int i, j, k;


     for( i = 0; i < TUBE_ROWS; i++ )
         for( j = 0; j < TUBE_COLUMNS; j++ )
             {
              Center_Of_2D( i, j, centers_array[i][j] );
             }
     
     for( i = 0; i < TUBE_ROWS; i++ )
         for( j = 0; j < TUBE_COLUMNS; j++ )
             for( k = 0; k < TUBE_HEIGHT; k++ )
                 {
                  Center_Of_3D( i, j, k, centers_array_3D[i][j][k] );
                 }

    } /* End of Init_Centers_Array */


/* Recursive function to find a path using rows_type, is_visible_2D, 
   and the pylon grid */
int One_Step_2D( WorldStuff *world_stuff, int index, rows_type *rows, 
                 int direction, int pylon_index )
    {
     int temp_row, temp_column, old_row, old_column;
     player_events_type *events;
     int i, j;
     int distances[4];
     int directions[4] = {0,1,2,3};
     int temp;
     
     events =  &player_events[index];
     
     
     old_row    = rows->row;
     old_column = rows->column;
     
     if( direction == GRID_LEFT )
         {
          temp_column = rows->column - 1;
          temp_row    = rows->row;
         }
     else if( direction == GRID_RIGHT )
         {
          temp_column = rows->column + 1;
          temp_row    = rows->row;
         }
     else if( direction == GRID_UP )
         {
          temp_row    = rows->row + 1;
          temp_column = rows->column;
         }
     else if( direction == GRID_DOWN ) 
         {
          temp_row    = rows->row - 1;    
          temp_column = rows->column;
         }
     else /* direction == GRID_START */
         {
          temp_row    = rows->row;
          temp_column = rows->column;
         }
         
     
     /* If this direction sends us out of the cylinder, this is not a valid path */
     if( temp_row < 0 || temp_row > (TUBE_ROWS-1) ) 
         {
          return 0;                             
         }
     
     /* If this direction makes us loop around the grid, loop the index */
     if( temp_column > (TUBE_COLUMNS-1) ) 
         temp_column = 0;
     else if( temp_column < 0 )
         temp_column = (TUBE_COLUMNS-1);
         
     
     if( temp_row == rows->dest_row && temp_column == rows->dest_column )
         {
          return 1;
         }
     
     
     /* If there is a pylon here, or we've already been here...it aint valid */
     if( path_grid[temp_row][temp_column] == 1 || path_grid[temp_row][temp_column] == 2 )
         {
          return 0;                                                      
         }
     
     /* Now mark that we have been here */
     path_grid[temp_row][temp_column] = 2; 
     
     rows->row    = temp_row;
     rows->column = temp_column;
      
     if( events->memory.num_paths_2D >= MAX_DESTINATIONS || events->memory.num_paths_2D < 0 )
         {
          /*
          printf("%d \n", events->memory.num_paths_2D );
          printf("Too many \n");
          */
          Point_Square_2D( rows->position, &temp_row, &temp_column );
         
          path_grid[rows->dest_row][rows->dest_column] = 9;
          path_grid[temp_row][temp_column] = 5;
         
          /*
          for( i = 0; i < TUBE_ROWS; i++ )
              {
               for( j = 0; j < TUBE_COLUMNS; j++ )
                   printf("%d ", path_grid[i][j] );
               printf("\n");
              }
          printf("\n\n\n");
          Get_Keypress();
          
          exit_gracefully();
          */
          return 0;
          
         }
          
     
     /* Temporarily add this point to the array of destinations */
     events->memory.path_2D[events->memory.path_index_2D][X] = centers_array[rows->row][rows->column][X];
     events->memory.path_2D[events->memory.path_index_2D][Y] = centers_array[rows->row][rows->column][Y];
     events->memory.num_paths_2D++;
     events->memory.path_index_2D++;
     
     
     if( rows->dest_row > rows->row )
         {
          distances[GRID_UP]   = rows->dest_row - rows->row;
          distances[GRID_DOWN] = -distances[GRID_UP];
         }                                           
     else if( rows->dest_row < rows->row )
         {
          distances[GRID_DOWN]   = rows->row - rows->dest_row;
          distances[GRID_UP]     = -distances[GRID_DOWN];
         }                    
     else
         {
          distances[GRID_DOWN]   = 0;
          distances[GRID_UP]     = 0;
         }                                             
     
     if( rows->dest_column > rows->column )
         {
          distances[GRID_RIGHT] = rows->dest_column - rows->column;
          distances[GRID_LEFT]  = 30 - distances[GRID_RIGHT];
         }
     else if( rows->dest_column < rows->column )
         {
          distances[GRID_LEFT]   = rows->column - rows->dest_column;
          distances[GRID_RIGHT]  = 30 - distances[GRID_LEFT];
         }
     else
         {
          distances[GRID_LEFT]   = 0;
          distances[GRID_RIGHT]  = 0;
         }
         
     if( distances[GRID_LEFT] > 16 )
         distances[GRID_LEFT] = -distances[GRID_LEFT];
     if( distances[GRID_RIGHT] > 16 )
         distances[GRID_RIGHT] = -distances[GRID_RIGHT];
     
                  
     /* Sort the directions */
     for( i = 3; i > 0; i-- )
         for( j = 0; j < i; j++ )
             {
              if( distances[ directions[j] ] < distances[ directions[j + 1] ] )
                  {
                   temp              = directions[j];
                   directions[j]     = directions[j + 1];
                   directions[j + 1] = temp;
                  }
             }
     
     
     /* Try to find a path in each direction, if any one of them is successful, return success */     
     for( i = 0; i < 4; i++ )
         {
          if( One_Step_2D( world_stuff, index, rows, directions[i], pylon_index ) )
              return 1;
         }
         
     
     /* If we have reached this point there is no solution in this direction,
        reset everything the way it was */     
     rows->row    = old_row;
     rows->column = old_column;
     events->memory.num_paths_2D--;
     events->memory.path_index_2D--;
     
     return 0;
        
    } /* End of One_Step_2D */
               
               
/* Make a path consisting of centers of squares in the world grid that avoids pylons.  Pylon_index
   is the number of the pylon we are moving to, if none it should be set to -1...the purpose of that
   variable is to make sure we don't check against that pylon in is_visible */
void Make_Path_2D( WorldStuff *world_stuff, int index, Float_Point2d destination, int pylon_index )
    {
     player_events_type *events;
     int i, j;
     rows_type rows;
     Float_Point2d position;
     
     
     events = &player_events[index];
     
     events->memory.num_paths_2D = 0;
     events->memory.path_index_2D = 0;
     events->memory.following_path = 1;
                    
     
     /* Convert our point into 2D */
     Cylinder_x_y( world_stuff->player_array[index].tank.orient.position, position );
     
     rows.destination[X] = destination[X];
     rows.destination[Y] = destination[Y];

     rows.position[X] = position[X];
     rows.position[Y] = position[Y];
     
     events->memory.final_destination_2D[X] = destination[X];
     events->memory.final_destination_2D[Y] = destination[Y];
     
     Point_Square_2D( position, &rows.row, &rows.column ); 
     Point_Square_2D( destination, &rows.dest_row, &rows.dest_column );
     
          
     /* Copy the pylon grid into the path making grid */     
     for( i = 0; i < TUBE_ROWS; i++ )      
         for( j = 0; j < TUBE_COLUMNS; j++ )
             path_grid[i][j] = pylon_grid[i][j];
     
     
     if( !One_Step_2D( world_stuff, index, &rows, GRID_START, pylon_index ) )
         {
          events->memory.following_path = 0;
          /* printf("Failed\n"); */
         }
    
             
     player_events[index].memory.path_index_2D = 0;        
    
     
     Point_Square_2D( position, &rows.row, &rows.column ); 
     
     path_grid[rows.row][rows.column] = 5;
     path_grid[rows.dest_row][rows.dest_column] = 9;

       
     
    } /* End of Make_Path_2D */
    
                                                   

void Follow_Path_2D( WorldStuff *world_stuff, int index )
    {
     short angle, enemy_angle;
     float dist, enemy_dist;
     player_events_type *events;
     Float_Point2d point;
     short skill_level;
     short current_enemy;
     
     enemy_angle = 0;
	 enemy_dist = 0;

     events = &player_events[index];
     Cylinder_x_y( world_stuff->player_array[index].tank.orient.position, point );
     current_enemy = player_events[index].memory.current_enemy;
     skill_level = world_stuff->player_array[index].character.skill_level;
     
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          enemy_dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          enemy_angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               enemy_dist  = player_events[index].angles.enemy_radar_dist_2D;
               enemy_angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }
     
     
     dist  = Point_Distance_2D( events->memory.path_2D[events->memory.path_index_2D], point );
     angle = Point_Angle_2D( events->memory.path_2D[events->memory.path_index_2D], &world_stuff->player_array[index].tank.orient );

     
     /* If we are close to the end of this 'node', move to the next node */
     if( dist < 0.1 )
         {
          events->memory.path_index_2D++;
          dist  = Point_Distance_2D( events->memory.path_2D[events->memory.path_index_2D], point );
          angle = Point_Angle_2D( events->memory.path_2D[events->memory.path_index_2D], &world_stuff->player_array[index].tank.orient );
         }
     
     /* If we have more nodes */
     if( events->memory.path_index_2D < events->memory.num_paths_2D )
         {
          if( events->memory.path_index_2D < events->memory.num_paths_2D - 1 )
              {  
               
               if( Is_Visible_2D( point, events->memory.path_2D[events->memory.path_index_2D + 1], &world_stuff->pylons ) )
                    {
                      events->memory.path_index_2D++;                   
                      dist  = Point_Distance_2D( events->memory.path_2D[events->memory.path_index_2D], point );
                      angle = Point_Angle_2D( events->memory.path_2D[events->memory.path_index_2D], &world_stuff->player_array[index].tank.orient );
                    }
               
              }
          
          
          if( skill_level == 0 )
              {
               Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
          else if( skill_level == 1 )   
              {
               Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
          else if( skill_level == 2 )   
              {
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
               if( current_enemy != 255 )
                   Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index );
              }
          else /* skill_level == 3 */
              {
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
               if( current_enemy != 255 )
                   Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index );
               Missiles( world_stuff, index );                           
              }
         } 
     else /* No nodes left */
         events->memory.following_path = 0;

     
    } /* End of Follow_Path_2D */                                                   


/* Recursive function to find a path using rows_type, is_visible_3D, 
   and the pylon grid */
int One_Step_3D( WorldStuff *world_stuff, int index, rows_type_3D *rows, 
                 int direction, int pylon_index )
    {
     int temp_row, temp_column,temp_height, old_row, old_column, old_height;
     player_events_type *events;
     int i, j;
     int distances[4];
     int directions[4] = {0,1,2,3};
     int temp;
     

     events =  &player_events[index];
     
     old_row    = rows->row;
     old_column = rows->column;
     old_height = rows->height;

     if( direction == GRID_LEFT )
         {
          temp_column = rows->column - 1;
          temp_row    = rows->row;
          temp_height = rows->height;
         }
     else if( direction == GRID_RIGHT )
         {
          temp_column = rows->column + 1;
          temp_row    = rows->row;
          temp_height = rows->height;
         }
     else if( direction == GRID_UP )
         {
          temp_row    = rows->row + 1;
          temp_column = rows->column;
          temp_height = rows->height;
         }
     else if( direction == GRID_DOWN ) 
         {
          temp_row    = rows->row - 1;    
          temp_column = rows->column;
          temp_height = rows->height;
         }
     else /* direction == GRID_START */
         {
          temp_row    = rows->row;
          temp_column = rows->column;
          temp_height = rows->height;
         }
         
     
     /* If this direction sends us out of the cylinder, this is not a valid path */
     if( temp_row < 0 || temp_row > (TUBE_ROWS-1) ) 
         {
          return 0;                             
         }
     
     /* If this direction makes us loop around the grid, loop the index */
     if( temp_column > (TUBE_COLUMNS-1) ) 
         temp_column = 0;
     else if( temp_column < 0 )
         temp_column = (TUBE_COLUMNS-1);
         
     
     if( temp_row == rows->dest_row && temp_column == rows->dest_column )
         {
          return 1;
         }
     
     /* If there is a pylon here, or we've already been here...it aint valid */
     if( path_grid_3D[temp_row][temp_column][temp_height] == 1 || path_grid_3D[temp_row][temp_column][temp_height] == 2 )
         {
          return 0;                                                      
         }
     
     /* Now mark that we have been here */
     path_grid_3D[temp_row][temp_column][temp_height] = 2; 
     
     rows->row    = temp_row;
     rows->column = temp_column;
     rows->height = temp_height; 
     
     if( events->memory.num_paths_3D >= MAX_DESTINATIONS || events->memory.num_paths_3D < 0 )
         {
          /* printf("Too many \n"); */
          return 0;
         }
          
     
     /* Temporarily add this point to the array of destinations */
     events->memory.path_3D[events->memory.path_index_3D][X] = centers_array_3D[rows->row][rows->column][rows->height][X];
     events->memory.path_3D[events->memory.path_index_3D][Y] = centers_array_3D[rows->row][rows->column][rows->height][Y];
     events->memory.path_3D[events->memory.path_index_3D][Z] = centers_array_3D[rows->row][rows->column][rows->height][Z];
     
     events->memory.num_paths_3D++;
     events->memory.path_index_3D++;
     
     
     if( Is_Visible_3D_Pylon( centers_array_3D[rows->row][rows->column][rows->height], centers_array_3D[rows->dest_row][rows->dest_column][rows->dest_height], &world_stuff->pylons, pylon_index ) )
         return 1;
     
     
     if( rows->dest_row > rows->row )
         {
          distances[GRID_UP]   = rows->dest_row - rows->row;
          distances[GRID_DOWN] = -distances[GRID_UP];
         }                                           
     else if( rows->dest_row < rows->row )
         {
          distances[GRID_DOWN]   = rows->row - rows->dest_row;
          distances[GRID_UP]     = -distances[GRID_DOWN];
         }                    
     else
         {
          distances[GRID_DOWN]   = 0;
          distances[GRID_UP]     = 0;
         }                                             
     
     if( rows->dest_column > rows->column )
         {
          distances[GRID_RIGHT] = rows->dest_column - rows->column;
          distances[GRID_LEFT]  = 30 - distances[GRID_RIGHT];
         }
     else if( rows->dest_column < rows->column )
         {
          distances[GRID_LEFT]   = rows->column - rows->dest_column;
          distances[GRID_RIGHT]  = 30 - distances[GRID_LEFT];
         }
     else
         {
          distances[GRID_LEFT]   = 0;
          distances[GRID_RIGHT]  = 0;
         }
         
     if( distances[GRID_LEFT] > 16 )
         distances[GRID_LEFT] = -distances[GRID_LEFT];
     if( distances[GRID_RIGHT] > 16 )
         distances[GRID_RIGHT] = -distances[GRID_RIGHT];
     
                  
     /* Sort the directions */
     for( i = 3; i > 0; i-- )
         for( j = 0; j < i; j++ )
             {
              if( distances[ directions[j] ] < distances[ directions[j + 1] ] )
                  {
                   temp              = directions[j];
                   directions[j]     = directions[j + 1];
                   directions[j + 1] = temp;
                  }
             }
     
     
     /* Try to find a path in each direction, if any one of them is successful, return success */     
     for( i = 0; i < 4; i++ )
         {
          if( One_Step_3D( world_stuff, index, rows, directions[i], pylon_index ) )
              return 1;
         }
         
     
     /* If we have reached this point there is no solution in this direction,
        reset everything the way it was */     
     rows->row    = old_row;
     rows->column = old_column;
     rows->height = old_height;
     events->memory.num_paths_3D--;
     events->memory.path_index_3D--;
     
     return 0;
        
    } /* End of One_Step_3D */
               
               
/* Make a path consisting of centers of squares in the world grid that avoids pylons.  Pylon_index
   is the number of the pylon we are moving to, if none it should be set to -1...the purpose of that
   variable is to make sure we don't check against that pylon in is_visible */
void Make_Path_3D( WorldStuff *world_stuff, int index, Float_Point destination, int pylon_index )
    {
     player_events_type *events;
     int i, j, k;
     rows_type_3D rows;
     Float_Point position;
     
     
     events = &player_events[index];
     
     events->memory.num_paths_3D = 0;
     events->memory.path_index_3D = 0;
     events->memory.following_path = 1;
     
     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, position );

     rows.destination[X] = destination[X];
     rows.destination[Y] = destination[Y];
     rows.destination[Z] = destination[Z];
     
     rows.position[X] = position[X];
     rows.position[Y] = position[Y];
     rows.position[Z] = position[Z];

     events->memory.final_destination_3D[X] = destination[X];
     events->memory.final_destination_3D[Y] = destination[Y];
     events->memory.final_destination_3D[Z] = destination[Z];

     Point_Square_3D( position, &rows.row, &rows.column, &rows.height ); 
     Point_Square_3D( destination, &rows.dest_row, &rows.dest_column, &rows.dest_height );
     
          
     /* Copy the pylon grid into the path making grid */     
     for( i = 0; i < TUBE_ROWS; i++ )      
         for( j = 0; j < TUBE_COLUMNS; j++ )
             for( k = 0; k < TUBE_HEIGHT; k++ )
                 path_grid_3D[i][j][k] = pylon_grid_3D[i][j][k];
     
     /* printf("Trying from r=%d c=%d h=%d to r=%d c=%d h=%d \n", rows.row, rows.column, rows.height, rows.dest_row, rows.dest_height, rows.dest_height);  */
     if( !One_Step_3D( world_stuff, index, &rows, GRID_START, pylon_index ) )
         {
          events->memory.following_path = 0;

          /*
          printf("Failed from r=%d c=%d h=%d to r=%d c=%d h=%d \n", rows.row, rows.column, rows.height, rows.dest_row, rows.dest_height, rows.dest_height);
         
          path_grid_3D[rows.row][rows.column][rows.height] = 5;
          path_grid_3D[rows.dest_row][rows.dest_column][rows.dest_height] = 9;
          
          for( i = 0; i < TUBE_ROWS; i++ )      
              {
               for( j = 0; j < TUBE_COLUMNS; j++ )
                   printf("%d ", path_grid_3D[i][j][rows.height] );
               printf("\n");
              }
          printf("\n\n");
          */
         }
             
     player_events[index].memory.path_index_3D = 0;        
     
     
     path_grid_3D[rows.row][rows.column][rows.height] = 5;
     path_grid_3D[rows.dest_row][rows.dest_column][rows.dest_height] = 9;

     /*
     for( i = 0; i < TUBE_ROWS; i++ )      
         {
          for( j = 0; j < TUBE_COLUMNS; j++ )
              printf("%d ", path_grid_3D[i][j][rows.height] );
          printf("\n");
         }
     printf("\n\n");
     */

    } /* End of Make_Path_3D */
    
                                                   


void Follow_Path_3D( WorldStuff *world_stuff, int index )
    {
     short horizontal_angle, vertical_angle;
     float dist;
     player_events_type *events;
     Float_Point point;
     short skill_level;
     short current_enemy;
     
     events = &player_events[index];
     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, point );
     current_enemy = player_events[index].memory.current_enemy;
     skill_level = world_stuff->player_array[index].character.skill_level;
     
     dist  = Point_Distance_3D( events->memory.path_3D[events->memory.path_index_3D], point );
     horizontal_angle = Horizontal_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
     vertical_angle   = Vertical_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
     
     /* If we are close to the end of this 'node', move to the next node */
     if( dist < 0.1 )
         {
          events->memory.path_index_3D++;
          
          dist  = Point_Distance_3D( events->memory.path_3D[events->memory.path_index_3D], point );
          horizontal_angle = Horizontal_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
          vertical_angle   = Vertical_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
         }
     
     /* If we have more nodes */
     if( events->memory.path_index_3D < events->memory.num_paths_3D )
         {
          if( events->memory.path_index_3D < events->memory.num_paths_3D - 1 )
              {  
               if( Is_Visible_3D( point, events->memory.path_3D[events->memory.path_index_3D + 1], &world_stuff->pylons ) )
                    {
                      events->memory.path_index_3D++;                   
                      dist  = Point_Distance_3D( events->memory.path_3D[events->memory.path_index_3D], point );
                      horizontal_angle = Horizontal_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
                      vertical_angle   = Vertical_Point_Angle_3D( events->memory.path_3D[events->memory.path_index_3D], &world_stuff->player_array[index].tank.orient );
                    }
              }
               Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 2, 0.5 );
         }
     else /* No nodes left */
         events->memory.following_path = 0;

     
    } /* End of Follow_Path_3D */                                                   
