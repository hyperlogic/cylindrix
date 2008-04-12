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

#include <stdlib.h>
#include "tanks.h"
#include "object.h"
#include "collide.h"

extern const char* g_DataPath;

void Load_Tank( Vehicle *tank, enum VehicleType tank_type )
    {
     FILE *fp;

	char newfilename[512];

	sprintf(newfilename,"%sgamedata/tanks.tbf",g_DataPath);



     if( (fp = fopen( newfilename, "rb" )) == NULL )
         SYS_Error( "Load_Tank() : fopen failed\n" );

     fseek( fp, sizeof( Vehicle ) * tank_type, SEEK_SET );
     fread( tank, sizeof( Vehicle ), 1, fp );

     fclose( fp );

    } /* End of Load_Tank */

void init_vehicle_gradient( Vehicle *tank )
{
    long i;

    if( tank->team == RED_TEAM ) {

        for( i = 0; i < tank->obj->faces; i++ ) {
            if( tank->obj->face[i].gradient == 0 )
                tank->obj->face[i].gradient = RedVehicleFirstGrad;
            else if( tank->obj->face[i].gradient == 1 )
                tank->obj->face[i].gradient = RedVehicleSecondGrad;
            else if( tank->obj->face[i].gradient == 2 )
                tank->obj->face[i].gradient = RedVehicleThirdGrad;
            else
                tank->obj->face[i].gradient = RedVehicleFirstGrad;
        }

        for( i = 0; i < tank->world_obj->faces; i++ ) {
            if( tank->world_obj->face[i].gradient == 0 )
                tank->world_obj->face[i].gradient = RedVehicleFirstGrad;
            else if( tank->world_obj->face[i].gradient == 1 )
                tank->world_obj->face[i].gradient = RedVehicleSecondGrad;
            else if( tank->world_obj->face[i].gradient == 2 )
                tank->world_obj->face[i].gradient = RedVehicleThirdGrad;
            else
                tank->world_obj->face[i].gradient = RedVehicleFirstGrad;
        }

    }
    else {

        for( i = 0; i < tank->obj->faces; i++ ) {
            if( tank->obj->face[i].gradient == 0 )
                tank->obj->face[i].gradient = BlueVehicleFirstGrad;
            else if( tank->obj->face[i].gradient == 1 )
                tank->obj->face[i].gradient = BlueVehicleSecondGrad;
            else if( tank->obj->face[i].gradient == 2 )
                tank->obj->face[i].gradient = BlueVehicleThirdGrad;
            else
                tank->obj->face[i].gradient = BlueVehicleFirstGrad;
        }

        for( i = 0; i < tank->world_obj->faces; i++ ) {
            if( tank->world_obj->face[i].gradient == 0 )
                tank->world_obj->face[i].gradient = BlueVehicleFirstGrad;
            else if( tank->world_obj->face[i].gradient == 1 )
                tank->world_obj->face[i].gradient = BlueVehicleSecondGrad;
            else if( tank->world_obj->face[i].gradient == 2 )
                tank->world_obj->face[i].gradient = BlueVehicleThirdGrad;
            else
                tank->world_obj->face[i].gradient = BlueVehicleFirstGrad;
        }
    }
}



void Init_Tank( Vehicle *tank, enum VehicleType tank_type, team_type team )
    {

    tank->vtype = tank_type;
    tank->team = team;

    if( (tank->obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->obj->point = NULL;
    tank->obj->points = 0;
    tank->obj->face = NULL;
    tank->obj->faces = 0;

    if( (tank->collision_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->collision_obj->point = NULL;
    tank->collision_obj->points = 0;
    tank->collision_obj->face = NULL;
    tank->collision_obj->faces = 0;

    switch( tank_type )
        {
         case Wasp :
             get_object( tank->obj, "3d_data/wasp.pfd" );
             get_object( tank->collision_obj, "3d_data/cwasp.pfd" );
             break;
         case Beetle :
             get_object( tank->obj, "3d_data/beetle.pfd" );
             get_object( tank->collision_obj, "3d_data/cbeetle.pfd" );
             break;
         case Flea :
             get_object( tank->obj, "3d_data/flea.pfd" );
             get_object( tank->collision_obj, "3d_data/cflea.pfd" );
             break;
         case Mosquito :
             get_object( tank->obj, "3d_data/mosquito.pfd" );
             get_object( tank->collision_obj, "3d_data/cmosquit.pfd" );
             break;
         case Spider :
             get_object( tank->obj, "3d_data/spider.pfd" );
             get_object( tank->collision_obj, "3d_data/cspider.pfd" );
             break;
         case Dragonfly :
             get_object( tank->obj, "3d_data/dragon.pfd" );
             get_object( tank->collision_obj, "3d_data/cdragon.pfd" );
             break;
         case Roach :
             get_object( tank->obj, "3d_data/roach.pfd" );
             get_object( tank->collision_obj, "3d_data/croach.pfd" );
             break;
         case Locust :
             get_object( tank->obj, "3d_data/locust.pfd" );
             get_object( tank->collision_obj, "3d_data/clocust.pfd" );
             break;

         default :
             SYS_Error("Invalid tank number in Init_Tank \n");
             break;

        } /* End switch */

    if( ( tank->world_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->world_obj->point = NULL;
    tank->world_obj->points = 0;
    tank->world_obj->face = NULL;
    tank->world_obj->faces = 0;

    init_world_obj( tank );

    init_vehicle_gradient( tank );

    if( ( tank->world_collision_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->world_collision_obj->point = NULL;
    tank->world_collision_obj->points = 0;
    tank->world_collision_obj->face = NULL;
    tank->world_collision_obj->faces = 0;

    init_world_collision_obj( tank );

    init_bounding_box( tank );

    tank->collision_edges.edge = NULL;
    tank->collision_edges.edges = 0;

    init_edge_table( tank );

    tank->alive = TRUE;
    tank->target = NULL;
    tank->vehicle_mode = Surface;

} /* End of Init_Tank */

