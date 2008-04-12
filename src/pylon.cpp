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

#include "stdio.h"
#include "object.h"
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include "util.h"
#include "pylon.h"
#include "ai_util.h"

extern const char* g_DataPath;

void change_pylon_color( Pylon *pylon, Gradient gradient )
{
    long i;

    pylon->gradient = gradient;
    for( i = 0; i < pylon->obj.faces; i++ ) {
        pylon->obj.face[i].gradient = gradient;
    }
}

void pylon_union( Pylons *pylons, PointFace *result )
{
    long i, j, k;
    Point *point_ptr;
    Face *face_ptr;
    PointFace *obj_ptr;
    long point_start;
    long points_added = 0;
    long faces_added = 0;

    /* Remalloc the point array of result. Worst case 16 points per pylon. */

    if( ( result->point = (Point*)realloc( result->point,
        ( result->points + (pylons->pylons * 16) ) * sizeof( Point ) ) ) == NULL ) {
        printf("union : realloc failed\n");
        exit(1);
    }
    point_ptr = &result->point[result->points];
    point_start = result->points;

    /* Remalloc the face array of result. Worst case 5 faces per pylon. */

    if( ( result->face = (Face*)realloc( result->face,
        ( result->faces + (pylons->pylons * 5) ) * sizeof( Face ) ) ) == NULL ) {
        printf("union : realloc failed 2\n");
        exit(1);
    }
    face_ptr = &result->face[result->faces];

    for( i = 0; i < pylons->pylons; i++ ) {

        obj_ptr = &pylons->pylon[i].obj;

        /* Copy this pylons points into result. */

        memcpy( point_ptr, obj_ptr->point, sizeof( Point ) * obj_ptr->points );

        points_added += obj_ptr->points;

        /* Copy this pylons faces into result. */

        for( j = 0; j < obj_ptr->faces; j++ ) {
            for( k = 0; k < obj_ptr->face[j].size; k++ ) {
                face_ptr->index[k] = point_start + obj_ptr->face[j].index[k];
            }
            face_ptr->size = obj_ptr->face[j].size;
            face_ptr->gradient = obj_ptr->face[j].gradient;
            face_ptr->normal[X] = obj_ptr->face[j].normal[X];
            face_ptr->normal[Y] = obj_ptr->face[j].normal[Y];
            face_ptr->normal[Z] = obj_ptr->face[j].normal[Z];
            face_ptr->transparent = obj_ptr->face[j].transparent;
            faces_added++;
            face_ptr++;
        }
        point_start = point_start + obj_ptr->points;
        point_ptr = point_ptr + obj_ptr->points;
    }
    result->points = result->points + points_added;
    result->faces = result->faces + faces_added;
}


/* reads in a .pyl file and initalizes the Pylons data structure */

void get_pylons( Pylons *pylons, char *filename )
{
    FILE *fp;
    long num_pylons;
    long i, x, y, j;
    float height;
    int grad;
    long index;
    float h, z_step, theta;
    const float top_edge = -30.0;
    const float bottom_edge = 30.0;
    const int n = 30; /* 25 */
    const int m = 30; /* 30 */
    const float r = 10.0;
    long point1, point2, point3, point4;
    long temp;
    Point *point_ptr;  /* used to avoid a lot of dereferencing */
    Face *face_ptr;    /* same here */
    long point_size;
    long face_size;
    Float_Point u, v, uxv;
    float uxv_dot_uxv, len;
    Float_Point2d temp2d;
    Float_Point temp3d;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


    z_step = ( top_edge - bottom_edge ) / ( n - 1 );
    theta = (2 * PI) / m;

    if( (fp = fopen( newfilename, "r" )) <= 0 )
        SYS_Error("get_pylons(): Invalid filname!\n");    

    /* get the number of pylons in file */

    fscanf( fp, " %ld ", &num_pylons );
    pylons->pylons = (short)num_pylons;

    /* malloc the pylons */

    if( ( pylons->pylon = (Pylon *)malloc( sizeof( Pylon ) * num_pylons ) ) == NULL )
        SYS_Error("get_pylons() : malloc failed!\n");

    for( i = 0; i < num_pylons; i++ ) {

        /* get the pylons x y and h fields */

        fscanf( fp, " %ld %ld %f ", &x, &y, &height );

        pylons->pylon[i].x = (short)x;
        pylons->pylon[i].y = (short)y;
        pylons->pylon[i].height = height;
        pylons->pylon[i].owner = NO_TEAM;

        /* get the pylons color */

        fscanf( fp, " %d ", &(grad) );
        pylons->pylon[i].gradient = (unsigned char)grad;
    }


    fclose( fp );


    /* initialize each pylons PointFace */

    for( i = 0; i < pylons->pylons; i++ ) {

        /* malloc space for 16 points (worst case) */

        if( ( pylons->pylon[i].obj.point = (Point *) malloc( sizeof( Point ) * 16 ) ) == NULL )
            SYS_Error("get_pylons() : malloc failed\n");

        /* malloc space for 5 faces */

        if( ( pylons->pylon[i].obj.face = (Face *) malloc( sizeof( Face ) * 5 ) ) == NULL )
            SYS_Error("get_pylons() : malloc failed\n");

        /* initialize the two temp pointers */

        point_ptr = pylons->pylon[i].obj.point;
        face_ptr = pylons->pylon[i].obj.face;

        point_size = 0;
        face_size = 0;

        /* insert the correct points into the pointface */

        /* insert the "bottom" four vertices */

        point1 = ( pylons->pylon[i].y + 1 ) * m + pylons->pylon[i].x;

        point_ptr[point_size][X] = ftom( r * (float)cos( (point1 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( r * (float)sin( (point1 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point1 / m ) * z_step + bottom_edge );
        point_size++;

        point2 = ( pylons->pylon[i].y + 1 ) * m + (( pylons->pylon[i].x + 1 )
                 % m );

        point_ptr[point_size][X] = ftom( r * (float)cos( (point2 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( r * (float)sin( (point2 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point2 / m ) * z_step + bottom_edge );
        point_size++;

        point3 = ( pylons->pylon[i].y * m ) + (( pylons->pylon[i].x + 1 ) % m);

        point_ptr[point_size][X] = ftom( r * (float)cos( (point3 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( r * (float)sin( (point3 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point3 / m ) * z_step + bottom_edge );
        point_size++;

        point4 = ( pylons->pylon[i].y * m ) + pylons->pylon[i].x;

        point_ptr[point_size][X] = ftom( r * (float)cos( (point4 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( r * (float)sin( (point4 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point4 / m ) * z_step + bottom_edge );
        point_size++;

        /* insert the "top" four vertices */

        h = pylons->pylon[i].height;

        point_ptr[point_size][X] = ftom( (r - h) * (float)cos( (point1 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( (r - h) * (float)sin( (point1 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point1 / m ) * z_step + bottom_edge );
        point_size++;

        point_ptr[point_size][X] = ftom( (r - h) * (float)cos( (point2 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( (r - h) * (float)sin( (point2 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point2 / m ) * z_step + bottom_edge );
        point_size++;

        point_ptr[point_size][X] = ftom( (r - h) * (float)cos( (point3 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( (r - h) * (float)sin( (point3 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point3 / m ) * z_step + bottom_edge );
        point_size++;

        point_ptr[point_size][X] = ftom( (r - h) * (float)cos( (point4 % m) * theta ) );
        point_ptr[point_size][Y] = ftom( (r - h) * (float)sin( (point4 % m) * theta ) );
        point_ptr[point_size][Z] = ftom( ( point4 / m ) * z_step + bottom_edge );
        point_size++;

        /* insert the faces */

        /* This code doesn't blindly insert faces. First it checks to see
           if the current pylon is adjecent to another pylon, if so the
           shorter pylon will omit its face all together, but the taller
           will begin its face at the top of the shorter pylon.
           It's a hack really. I'll leave it in place until the hidden
           surface alg is better. Then I won't need the hack. */

        if( pylon_exists( pylons, pylons->pylon[i].x, (short)(pylons->pylon[i].y + 1), &index ) ) {
            if( pylons->pylon[i].height > pylons->pylon[index].height ) {

                /* insert 2 new points */

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point1 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point1 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point1 / m ) * z_step + bottom_edge );
                point_size++;

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point2 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point2 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point2 / m ) * z_step + bottom_edge );
                point_size++;

                /* insert north face */

                face_ptr[face_size].size = 4;
                face_ptr[face_size].index[0] = 5;
                face_ptr[face_size].index[1] = 4;
                face_ptr[face_size].index[2] = point_size - 2;
                face_ptr[face_size].index[3] = point_size - 1;
                face_ptr[face_size].gradient = pylons->pylon[i].gradient;
                face_size++;
            }
        }
        else {

            /* insert north face */

            face_ptr[face_size].size = 4;
            face_ptr[face_size].index[0] = 5;
            face_ptr[face_size].index[1] = 4;
            face_ptr[face_size].index[2] = 0;
            face_ptr[face_size].index[3] = 1;
            face_ptr[face_size].gradient = pylons->pylon[i].gradient;
            face_size++;
        }

        if( pylon_exists( pylons, (short)(pylons->pylon[i].x + 1), pylons->pylon[i].y, &index ) ) {
            if( pylons->pylon[i].height > pylons->pylon[index].height ) {

                /* insert 2 new points */

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point2 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point2 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point2 / m ) * z_step + bottom_edge );
                point_size++;

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point3 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point3 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point3 / m ) * z_step + bottom_edge );
                point_size++;

                /* east face */

                face_ptr[face_size].size = 4;
                face_ptr[face_size].index[0] = 6;
                face_ptr[face_size].index[1] = 5;
                face_ptr[face_size].index[2] = point_size - 2;
                face_ptr[face_size].index[3] = point_size - 1;
                face_ptr[face_size].gradient = pylons->pylon[i].gradient;
                face_size++;
            }
        }
        else {

            /* east face */

            face_ptr[face_size].size = 4;
            face_ptr[face_size].index[0] = 6;
            face_ptr[face_size].index[1] = 5;
            face_ptr[face_size].index[2] = 1;
            face_ptr[face_size].index[3] = 2;
            face_ptr[face_size].gradient = pylons->pylon[i].gradient;
            face_size++;
        }

        if( pylon_exists( pylons, pylons->pylon[i].x, (short)(pylons->pylon[i].y - 1), &index ) ) {
            if( pylons->pylon[i].height > pylons->pylon[index].height ) {

                /* insert 2 new points */

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point3 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point3 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point3 / m ) * z_step + bottom_edge );
                point_size++;

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point4 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point4 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point4 / m ) * z_step + bottom_edge );
                point_size++;

                /* south face */

                face_ptr[face_size].size = 4;
                face_ptr[face_size].index[0] = 7;
                face_ptr[face_size].index[1] = 6;
                face_ptr[face_size].index[2] = point_size - 2;
                face_ptr[face_size].index[3] = point_size - 1;
                face_ptr[face_size].gradient = pylons->pylon[i].gradient;
                face_size++;
            }
        }
        else {

            /* south face */

            face_ptr[face_size].size = 4;
            face_ptr[face_size].index[0] = 7;
            face_ptr[face_size].index[1] = 6;
            face_ptr[face_size].index[2] = 2;
            face_ptr[face_size].index[3] = 3;
            face_ptr[face_size].gradient = pylons->pylon[i].gradient;
            face_size++;
        }

        if( pylon_exists( pylons, (short)(pylons->pylon[i].x - 1), pylons->pylon[i].y, &index ) ) {
            if( pylons->pylon[i].height > pylons->pylon[index].height ) {

                /* insert 2 new points */

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point4 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point4 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point4 / m ) * z_step + bottom_edge );
                point_size++;

                point_ptr[point_size][X] = ftom( ( r - pylons->pylon[index].height ) * (float)cos( (point1 % m) * theta ) );
                point_ptr[point_size][Y] = ftom( ( r - pylons->pylon[index].height ) * (float)sin( (point1 % m) * theta ) );
                point_ptr[point_size][Z] = ftom( ( point1 / m ) * z_step + bottom_edge );
                point_size++;

                /* west face */

                face_ptr[face_size].size = 4;
                face_ptr[face_size].index[0] = 4;
                face_ptr[face_size].index[1] = 7;
                face_ptr[face_size].index[2] = point_size - 2;
                face_ptr[face_size].index[3] = point_size - 1;
                face_ptr[face_size].gradient = pylons->pylon[i].gradient;
                face_size++;
            }
        }
        else {

            /* west face */

            face_ptr[face_size].size = 4;
            face_ptr[face_size].index[0] = 4;
            face_ptr[face_size].index[1] = 7;
            face_ptr[face_size].index[2] = 3;
            face_ptr[face_size].index[3] = 0;
            face_ptr[face_size].gradient = pylons->pylon[i].gradient;
            face_size++;
        }

        /* top face */

        face_ptr[face_size].size = 4;
        face_ptr[face_size].index[0] = 6;
        face_ptr[face_size].index[1] = 7;
        face_ptr[face_size].index[2] = 4;
        face_ptr[face_size].index[3] = 5;
        face_ptr[face_size].gradient = pylons->pylon[i].gradient;
        face_size++;

        pylons->pylon[i].obj.points = point_size;
        pylons->pylon[i].obj.faces = face_size;

        if( ( pylons->pylon[i].obj.point = (Point *)realloc((void *)pylons->pylon[i].obj.point, sizeof( Point ) * point_size ) ) == NULL )
            SYS_Error("get_pylons() : realloc failed\n");

        point_ptr = pylons->pylon[i].obj.point;

        /* now find this pylons bounding box */

        /* find min_x */

        temp = point_ptr[0][X];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][X] < temp ) {
                temp = point_ptr[j][X];
            }
        }
        pylons->pylon[i].box.min_x = mtof( temp );
        pylons->pylon[i].mbox.min_x = temp;

        /* find max_x */

        temp = point_ptr[0][X];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][X] > temp ) {
                temp = point_ptr[j][X];
            }
        }
        pylons->pylon[i].box.max_x = mtof( temp );
        pylons->pylon[i].mbox.max_x = temp;

        /* find min_y */

        temp = point_ptr[0][Y];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][Y] < temp ) {
                temp = point_ptr[j][Y];
            }
        }
        pylons->pylon[i].box.min_y = mtof( temp );
        pylons->pylon[i].mbox.min_y = temp;

        /* find max_y */

        temp = point_ptr[0][Y];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][Y] > temp ) {
                temp = point_ptr[j][Y];
            }
        }
        pylons->pylon[i].box.max_y = mtof( temp );
        pylons->pylon[i].mbox.max_y = temp;

        /* find min_z */

        temp = point_ptr[0][Z];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][Z] < temp ) {
                temp = point_ptr[j][Z];
            }
        }
        pylons->pylon[i].box.min_z = mtof( temp );
        pylons->pylon[i].mbox.min_z = temp;

        /* find max_z */

        temp = point_ptr[0][Z];
        for( j = 1; j < 8; j++ ) {
            if( point_ptr[j][Z] > temp ) {
                temp = point_ptr[j][Z];
            }
        }
        pylons->pylon[i].box.max_z = mtof( temp );
        pylons->pylon[i].mbox.max_z = temp;

        /* find this pylons boundingbox2d */

        temp3d[X] = mtof( pylons->pylon[i].obj.point[1][X] );
        temp3d[Y] = mtof( pylons->pylon[i].obj.point[1][Y] );
        temp3d[Z] = mtof( pylons->pylon[i].obj.point[1][Z] );

        Cylinder_x_y( temp3d, temp2d );

        pylons->pylon[i].box2d.max_x = temp2d[X];
        pylons->pylon[i].box2d.min_y = temp2d[Y];  /* home */

        temp3d[X] = mtof( pylons->pylon[i].obj.point[3][X] );
        temp3d[Y] = mtof( pylons->pylon[i].obj.point[3][Y] );
        temp3d[Z] = mtof( pylons->pylon[i].obj.point[3][Z] );

        Cylinder_x_y( temp3d, temp2d );

        pylons->pylon[i].box2d.min_x = temp2d[X];
        pylons->pylon[i].box2d.max_y = temp2d[Y];  /* home */

        /* Hack alert!!!! fixes case when a pylons x-value = 29 */

        if( pylons->pylon[i].box2d.min_x > pylons->pylon[i].box2d.max_x ) {
            pylons->pylon[i].box2d.max_x = 40.0;
        }
        /*
        printf("min_x = %.2f, max_x = %.2f\n", pylons->pylon[i].box2d.min_x,
            pylons->pylon[i].box2d.max_x );
        printf("min_y = %.2f, max_y = %.2f\n\n", pylons->pylon[i].box2d.min_y,
            pylons->pylon[i].box2d.max_y );
        */

        /* now figure out the normal vectors for each face */

        for( j = 0; j < face_size; j++ ) {

            /* u is a vector from the second vertex to the first */

            u[X] = mtof( point_ptr[face_ptr[j].index[0]][X] -
                         point_ptr[face_ptr[j].index[1]][X] );
            u[Y] = mtof( point_ptr[face_ptr[j].index[0]][Y] -
                         point_ptr[face_ptr[j].index[1]][Y] );
            u[Z] = mtof( point_ptr[face_ptr[j].index[0]][Z] -
                         point_ptr[face_ptr[j].index[1]][Z] );

            /* v is a vector from the second vertex to the third */

            v[X] = mtof( point_ptr[face_ptr[j].index[2]][X] -
                         point_ptr[face_ptr[j].index[1]][X] );
            v[Y] = mtof( point_ptr[face_ptr[j].index[2]][Y] -
                         point_ptr[face_ptr[j].index[1]][Y] );
            v[Z] = mtof( point_ptr[face_ptr[j].index[2]][Z] -
                         point_ptr[face_ptr[j].index[1]][Z] );

            /* uxv is the cross product of u and v, and therefore the
               normal vector for that face.  But we still have to
               normalize it */

            uxv[X] = (u[Y] * v[Z]) - (u[Z] * v[Y] );
            uxv[Y] = (u[Z] * v[X]) - (u[X] * v[Z] );
            uxv[Z] = (u[X] * v[Y]) - (u[Y] * v[X] );

            /* Dot uxv with itself */

            uxv_dot_uxv = (uxv[X] * uxv[X]) + (uxv[Y] * uxv[Y]) +
                          (uxv[Z] * uxv[Z] );

            /* get the length of uxv */

            len = (float)sqrt( uxv_dot_uxv );

            /* normalize uxv */

            uxv[X] = uxv[X] / len;
            uxv[Y] = uxv[Y] / len;
            uxv[Z] = uxv[Z] / len;

            /* initialize the normal in face */

            face_ptr[j].normal[X] = ftom( uxv[X] );
            face_ptr[j].normal[Y] = ftom( uxv[Y] );
            face_ptr[j].normal[Z] = ftom( uxv[Z] );
            
            /* initialize each faces transparent flag */
            
            face_ptr[j].transparent = FALSE;
        }
        
        /* initailize each faces color */
        
        change_pylon_color( &(pylons->pylon[i]), NeutralPylonGrad );
    }
}

void print_pylons( Pylons *pylons )
{
    short i;

    printf( "%d\n", pylons->pylons );

    for( i = 0; i < pylons->pylons; i++ ) {
        printf("%d %d %.5f ", pylons->pylon[i].x, pylons->pylon[i].y,
                              pylons->pylon[i].height );

        printf("%d\n", pylons->pylon[i].gradient );

        printf("obj\n");
        print_object( &(pylons->pylon[i].obj) );
        printf("BoundingBox (%.5f,%.5f,%.5f,%.5f,%.5f,%.5f)\n",
            pylons->pylon[i].box.min_x,
            pylons->pylon[i].box.max_x,
            pylons->pylon[i].box.min_y,
            pylons->pylon[i].box.max_y,
            pylons->pylon[i].box.min_z,
            pylons->pylon[i].box.max_z );
    }
}

int pylon_exists( Pylons* pylons, short x, short y, long* index )
{
    long i;

    for( i = 0; i < pylons->pylons; i++ ) {
        if( ( pylons->pylon[i].x == x ) && ( pylons->pylon[i].y == y ) ) {
            *index = i;
            return TRUE;
        }
    }
    return FALSE;
}

void Tally_Pylons( Player player[], Pylons *pylons )
    {
     int i;
     int red_pylons, blue_pylons, total_pylons;
     float red_strength, blue_strength;


     red_pylons = blue_pylons = 0;
     total_pylons = pylons->pylons;


     for( i = 0; i < pylons->pylons; i++ )
         {
          if( pylons->pylon[i].owner == BLUE_TEAM )
              blue_pylons++;
          else if( pylons->pylon[i].owner == RED_TEAM )
              red_pylons++;
         }

     red_strength  = ( (float)red_pylons  / (float)total_pylons ) * 10;
     blue_strength = ( (float)blue_pylons / (float)total_pylons ) * 10;

     for( i = 0; i < 6; i++ )
         {
         if( player[i].team == RED_TEAM )
             {
             player[i].tank.laser_damage = (short)red_strength;
             player[i].tank.missile_damage = (short)red_strength + 4;
             }
         else if( player[i].team == BLUE_TEAM )
             {
             player[i].tank.laser_damage = (short)blue_strength;
             player[i].tank.missile_damage = (short)blue_strength + 4;
             }

         }

    } /* End of tally_pylons */

