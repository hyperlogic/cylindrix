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

/*   file => gfx_types.h
     programmers => Anthony Thibault & John McCawley III

     Holds all kinds of important defines and typedefs that are used
     throughout the game code.
*/

#ifndef TYPES_H
#define TYPES_H

#include "system/system.h"

#ifdef PROFILE
extern int profile = 0;
extern unsigned long ptime;
#endif

#ifndef PI
	#define PI 3.141592654f
#endif

/* #define __LINUX__ */
/* Only use linux specific __attribute__ tag on Linux */
#ifdef __LINUX__
	#define PACKED_STRUCT __attribute__((packed))
#else
	#define PACKED_STRUCT 
#endif

#define MAX_POINTS 3000    /* Max number of points in PointFace */
#define MAX_FACES 3000     /* Max number of faces in PointFace */
#define MAX_FACE_SIZE 10   /* Max number of points per face */

#define SCREEN_WIDTH  320  /* mode 13h screen width in pixels */
#define SCREEN_HEIGHT 200  /* mode 13h screen height in pixels */

#define X 0  /* index used to access the x-coord in a Point or a Vector */
#define Y 1  /* index used to access the y-coord in a Point or a Vector */
#define Z 2  /* index used to access the z-coord in a Point or a Vector */

/* defines 10-bit resolution for fractional part of fixed point numbers */

#define MAGIC 1024l      /* represents 1.0 in fixed-point */
#define MEXP 10l         /* log base 2 of MAGIC */
#define JUMBO 40960000l  /* a big number */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* HIGH_DETAIL, MEDIUM_DETAIL & LOW_DETAIL are used to determine the detail
   level of an object to be drawn */

#define LOW_DETAIL 0
#define MEDIUM_DETAIL 1
#define HIGH_DETAIL 2
#define SUPER_HIGH_DETAIL 3

/* HACK */

#define NULL_INDEX 9999

/* John's Defines ----------------------- */

/* Indices into input table */

#define INDEX_UP                      0
#define INDEX_DOWN                    1
#define INDEX_RIGHT                   2
#define INDEX_LEFT                    3
#define INDEX_SIDESTEP_LEFT           4
#define INDEX_SIDESTEP_RIGHT          5
#define INDEX_SIDESTEP_UP             6
#define INDEX_SIDESTEP_DOWN           7
#define INDEX_UP_THROTTLE             8
#define INDEX_DOWN_THROTTLE           9
#define INDEX_FIRE_GUN                10
#define INDEX_FIRE_MISSILE            11
#define INDEX_SPECIAL_WEAPON          12
#define INDEX_MODE_SWITCH             14
#define INDEX_L_COMMAND_PYLON         15
#define INDEX_L_COMMAND_ATTACK        16
#define INDEX_L_COMMAND_ATTACK_RADAR  17
#define INDEX_L_COMMAND_DEFEND_RADAR  18
#define INDEX_L_COMMAND_GROUP         19
#define INDEX_L_COMMAND_CANCEL        20
#define INDEX_R_COMMAND_PYLON         21
#define INDEX_R_COMMAND_ATTACK        22
#define INDEX_R_COMMAND_ATTACK_RADAR  23
#define INDEX_R_COMMAND_DEFEND_RADAR  24
#define INDEX_R_COMMAND_GROUP         25
#define INDEX_R_COMMAND_CANCEL        26

/* Number of 'states' the AI can get into */
#define NUM_STATES 13


/* Size of the input table array */
#define INPUT_TABLE_SIZE        27

#define MAX_WORLD_SAMPLES    17 /* This is the maximum number of sounds in the world */

#define NUMBER_CHARACTER_SOUNDS 7 /* Number of things a character can say */

#define MAX_COMPUTER_SAMPLES 15 /* This is the maximum number of computer samples */

/* cd track defines */
                           
#define SCAVENGER_SONG     "music/track09.mp3"
#define BOK_SONG           "music/track07.mp3"
#define SUCCUBI_SONG       "music/track10.mp3"
#define SENTRY_SONG        "music/track04.mp3"
#define HUMAN_SONG         "music/track02.mp3"
#define SLAR_SONG          "music/track05.mp3"
#define BIOMECHANOID_SONG  "music/track12.mp3"
#define PHAROAH_SONG       "music/track06.mp3"
#define OVERLORD_SONG      "music/track11.mp3"
#define WATCHER_SONG       "music/track03.mp3"
#define MENU_SONG          "music/track08.mp3"


/* End of John's defines ------------- */

/* defines the gradients used to draw the vehicles, radar bases, pylons,
   tube, wireframe radar, missiles, lasers, explosions, energy square
   and hubs */


typedef char string[80]; /* Strings for daily flossing */


typedef enum { RedVehicleFirstGrad, RedVehicleSecondGrad,
               RedVehicleThirdGrad, RedRadarFirstGrad,
               RedRadarSecondGrad, RedRadarThirdGrad,
               RedCapturedPylonGrad, BlueVehicleFirstGrad,
               BlueVehicleSecondGrad, BlueVehicleThirdGrad,
               BlueRadarFirstGrad, BlueRadarSecondGrad,
               BlueRadarThirdGrad, BlueCapturedPylonGrad,
               NeutralPylonGrad, WireFrameRadarGrad, TubeFirstGrad,
               TubeSecondGrad, TubeThirdGrad, MissileGrad,
               ExplosionCloudGrad, EnergySquareGrad, HubFirstGrad,
               HubSecondGrad, HubThirdGrad, LaserGrad0,
               LaserGrad1, LaserGrad2, LaserGrad3, LaserGrad4,
               LaserGrad5, LaserGrad6, LaserGrad7, LaserGrad8,
               LaserGrad9, LaserGrad10 } GameGradients;


typedef unsigned char boolean; /* For john's stuff */

typedef struct RGB_color_typ {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB_color, *RGB_color_ptr;


/* The packed is so we can read in the header using sizeof header */
typedef struct pcx_header_typ {
    unsigned char manufacturer    PACKED_STRUCT;
    unsigned char version         PACKED_STRUCT;
    unsigned char encoding        PACKED_STRUCT;
    unsigned char bits_per_pixel  PACKED_STRUCT;
    unsigned short xstart,ystart  PACKED_STRUCT;
    unsigned short xend, yend     PACKED_STRUCT;
    unsigned short horz_res       PACKED_STRUCT;
    unsigned short vert_res       PACKED_STRUCT;
    unsigned char ega_palette[48] PACKED_STRUCT;
    unsigned char reserved        PACKED_STRUCT;
    unsigned char num_bit_planes  PACKED_STRUCT;
    unsigned short bytes_per_line PACKED_STRUCT;
    unsigned short palette_type   PACKED_STRUCT;
    unsigned char padding[58]     PACKED_STRUCT;
} pcx_header, *pcx_header_ptr;


typedef struct pcx_picture_typ { /* Structure to hold a PCX picture */
    RGB_color palette[256];   /* Color palette */
    unsigned char *buffer;    /* Buffer to hold uncompressed picture data */
    int xpixels;              /* Number of pixels in x direction */
    int ypixels;              /* Number of pixels in y direction */
    int num_bytes;
    pcx_header header;
} pcx_picture, *pcx_picture_ptr;


/* Point2d is a 2d point in fixed-point */

typedef long Point2d[2];

/* Point is a 3d point in fixed-point */

typedef long Point[3];

/* Vector is a 3d vector in fixed-point (equivilent to Point) */

typedef long Vector[3];

/* Float_Point2d is a 2d point in floating-point */

typedef float Float_Point2d[2];

/* Float_Point is a 3d point in floating-point */

typedef float Float_Point[3];

/* Float_Vector is a 3d vector in floating-point (equivilent to Float_Point) */

typedef float Float_Vector[3];

/* Orientation contains information about a 3d objects position and
   orientation */

typedef struct {
    Float_Point position;  /* absolute x-y-z position */
    Float_Vector front;    /* the direction considered forward */
    Float_Vector up;       /* the direction considered up */
} Orientation;

/* MagicOrientation contains information about 3d objects position and
   orientation using fixed-point numbers */

typedef struct {
    Point position;  /* absolute x-y-z position */
    Vector front;    /* the direction considered forward */
    Vector up;       /* the direction considered up */
} MagicOrientation;

/* Gradient will uniquely define one gradient contained in
   ColorInfo.gradient.  It's kinda like color */

typedef unsigned char Gradient;

/* VehicleMode will identify this vehicles state */

enum VehicleMode { Air, Surface, Landing };

/* Face holds all the important information about 3d polygon */

typedef struct {
    long index[MAX_FACE_SIZE];  /* index into a list of vertices */
    unsigned char size;         /* number of vertices in this face */
    Vector normal;              /* normal vector of this face */
    long max_z;                 /* maximum z-coord in this face */
    Gradient gradient;          /* gradient to use when shading this face */
    unsigned char transparent;  /* true if this face is transparent */
} Face;

/* PointFace holds all the points & faces of an object */

typedef struct {
    Point *point;   /* pointer to an array of Points */
    long points;    /* length of the point array */
    Face *face;     /* pointer to an array of Faces */
    long faces;     /* length of the face array */
} PointFace;

/* Edge identifies a line between to vertices */

typedef long Edge[2];

/* PointEdge is a collection of 3d points and edges that define a wireframe
   object */

typedef struct {
    Point *point;  /* array of points */
    long points;   /* length of point array */
    Edge *edge;    /* array of edges */
    long edges;    /* length of edge aray */
} PointEdge;

/* EdgeTable is just a list of edges, there are no vertices */

typedef struct {
    Edge *edge;
    long edges;
} EdgeTable;

/* Window used by the polygon scanner's 2D-clipper */

typedef struct {
    int x0, y0, x1, y1;
} Window;

/* Matrix type is a 4x4 matrix of longs */

typedef long Matrix[16];

/* Float_Matrix is a 4x4 matrix of floats */

typedef float Float_Matrix[16];

/* BoundingBox defines a cube that is used during collision detection */

typedef struct {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    float min_z;
    float max_z;
} BoundingBox;

/* MagicBoundingBox defines a cube in fixed_point coords */

typedef struct {
    long min_x;
    long max_x;
    long min_y;
    long max_y;
    long min_z;
    long max_z;
} MagicBoundingBox;

/* BoundingBox2d defines a 2d square */

typedef struct {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
} BoundingBox2d;

/* projectile_type identifies the type of a projectile */

enum projectile_type { MISSILE, LASER, MINE, CS_MISSILE,
                       DECOY_0, DECOY_1, DECOY_2, TRAITOR_MISSILE };

/* team_type idenifies the team that something belongs to */

typedef enum { RED_TEAM, BLUE_TEAM, NO_TEAM } team_type;

/* Projectile contains all the information about a projectile needed to
   draw it and manipulate it (Note Projectile can be a linked list of
   projectiles) */

typedef struct projectile {
    enum projectile_type type;  /* Kind of projectile */
    PointFace *obj;             /* 3d object centered at the origin */
    PointFace *world_obj;       /* 3d object in world space */
    MagicBoundingBox box;       /* bounding box */
    EdgeTable edge_table;       /* all the edges of this projectile */
    MagicOrientation orient;    /* orientation in world_space */
    short on_surface;           /* True if this projectile huggs the surface */
    short frames_left;          /* Number of frame until projectiles goes poof! */
    struct projectile *next;    /* Link to the next projectile */
    float *target;              /* target pointer (missiles only) */
    Float_Vector vel;           /* velocity vector (decoys only) */
    float air_rise_rot_speed;   /* rotation value used by decoys only */
    float air_spin_rot_speed;   /* rotation value used by decoys only */
    float surface_rot_speed;    /* rotation value used by decoys only */
    unsigned char owner;        /* who shot this bullet?? 0-5 indices into player array
                                   6 red_radar_base 7 blue_radar_base */
} Projectile;

/* Pylon contains information about one pylon */

typedef struct {
    short x;                /* x position on x-y grid */
    short y;                /* y position on x-y grid */
    float height;           /* height of this pylon */
    team_type owner;        /* The team that owns this pylon */
    Gradient gradient;      /* gradient to use when shading this pylon */
    BoundingBox box;        /* bounding box */
    MagicBoundingBox mbox;  /* bounding box in fixed point */
    PointFace obj;          /* 3d object in world-space */
    BoundingBox2d box2d;    /* 2d bounding box */
} Pylon;

/* Pylons contains information about all the pylons */

typedef struct {
    short pylons;
    Pylon *pylon;
} Pylons;

/* Different vehicles used in the game */

enum VehicleType { Wasp, Beetle, Flea, Mosquito, Spider, Dragonfly, Roach,
                   Locust };

/* Holds all information needed about this vehicle */

typedef struct {

    /* General information about this vehicle */

    enum VehicleType vtype;         /* type of vehicle */
    team_type team;                 /* Team that this vehicle is on */
    enum VehicleMode vehicle_mode;  /* Is the vehicle on the surface? */
    short alive;                    /* Is the vehicle still alive? */
    float surface_rad;              /* radius at which vehicle skims the surface */

    /* Information about this vehicles 3d object */

    PointFace *obj;                  /* the 3d-object centered at the origin */
    PointFace *world_obj;            /* the 3d-object in world coordinates */
    PointFace *collision_obj;        /* 3d-object used for collisions centered at the origin */
    PointFace *world_collision_obj;  /* 3d-object used for collisions in world coords */
    BoundingBox box;                 /* the x-y-z extents of the vehicles collision_obj */
    MagicBoundingBox mbox;           /* the x-y-z extents in fixed-point */
    EdgeTable collision_edges;       /* all the edges in the vehicle collision obj */

    Orientation orient;  /* the vehicles current orientation */

    /* Information about this vehicles movement and rotation */

    Float_Vector vel;  /* current velocity */

    float air_forward_speed;      /* current air speed (units per frame) */
    float air_max_forward_speed;  /* maximum air speed (units per frame) */
    float air_inc_forward_speed;  /* incremental forward thrust */
    float air_max_sidestep_speed;  /* maximum sidesteping speed */
    float air_inc_sidestep_speed;  /* incremental sidesteping speed */

    float air_rise_rot_speed;  /* current rotation speed about the right axis */
    float air_spin_rot_speed;  /* current rotation speed about the front axis */
    float air_inc_rot_speed;  /* incremental rotation speed */
    float air_max_rot_speed;  /* rotation speed (radians per frame) */

    float surface_max_speed;  /* max surface speed of vehicle (units per frame) */
    float surface_inc_speed;  /* incremental surface speed */
    float surface_inc_sidestep_speed;  /* incremental sidesteping speed */

    float surface_rot_speed;      /* current rotation speed */
    float surface_inc_rot_speed;  /* incremental rotation speed */
    float surface_max_rot_speed;  /* max surface rotation speed (radians per frame) */

    /* Information about this vehicles weapons */

    float *target;  /* point that the missiles are locked on */

    float laser_speed;             /* speed of lasers */
    short laser_life;              /* frames a laser remains active */
    short laser_damage;            /* Number of hit points each laser takes off */
    short laser_reload_time;       /* time it takes to reload in frames */
    short frames_till_fire_laser;  /* number of frames until we can shoot */

    float missile_speed;             /* speed of missiles */
    float turning_angle;             /* radians that a missile can turn per frame */
    short missile_life;              /* frames that a missile remains active */
    short missile_damage;            /* damage done on collision in hitpoints */
    short missile_reload_time;       /* time it takes a missile to be reloaded */
    short frames_till_fire_missile;  /* time till we can fire another missile */
    short missile_generation_time;   /* time it takes a missile to be created */
    short frames_till_new_missile;   /* time left until a new missile is generated */
    short max_missile_storage;       /* maximum number of missiles that can be held */
    short missiles_stored;           /* current number of stored missiles */

    short max_projectiles;        /* max number of active projectiles */
    Projectile *projectile_list;  /* linked list of active projectiles */

    /* Information about this vehicles hitpoints */

    long max_hitpoints;      /* maximum hitpoints allowed */
    long current_hitpoints;  /* current number of hitpoints */

    short ramming_active;    /* ramming on indicator */
    short ramming_damage;    /* hitpoints of damage resulting from one ram */

    short double_lasers_active;  /* does this vehicle shoot two lasers? */

    short mine_reload_time;          /* time it takes to reload a mine */
    short mine_damage;               /* amount of damage a mine will inflect */
    short mine_life;                 /* number of frames a mine will remain active */

    short cs_missile_reload_time;    /* time to reload a cs_missile */
    short cs_missile_life;           /* time a cs_missile remains active */
    float cs_missile_speed;          /* speed of a cs_missile in units per frame */

    short controls_scrambled;        /* true when hit by a cs_missile */
    short frames_till_unscramble;    /* number of frames till controls will be normal */
    short scramble_life;             /* total number of frames controls will be scrambled */

    short traitor_missile_reload_time; /* time to reload a traitor_missile */
    short traitor_missile_life;        /* time a traitor_missile remains active */
    float traitor_missile_speed;       /* speed of a traitor_missile */

    short traitor_life;                   /* amount of time a vehicle is a traitor */
    short traitor_active;                 /* true when hit by a traitor_missile */
    short frames_till_traitor_deactivate; /* frames left for this vehicle to be a traitor */

    short anti_missile_active;       /* TRUE if anti-missile system is on */

    short cloaking_active;        /* TRUE if clocking is enabled */
    short cloak_reload_time;      /* number of frames till x key becomes active again */
    short frames_till_cloak;      /* number of frames till x key becomes active again (decrements every frame) */
    short cloak_time;             /* number of frames cloaking remains active until you suck another missile */
    short frames_till_cloak_suck; /* number of frames until cloak will suck a missile (decrements every frame) */

    short decoy_life;         /* time a decoy ships remains active */
    short decoy_reload_time;  /* number of frames till you can shoot a missile or another decoy */

} Vehicle;

#define NUM_GRADIENTS 50  /* the total number of gradients allowed in
                             the pallete */

/* GradientInfo contains information about one gradient in the pallete */

typedef struct {
    boolean active;       /* True if this gradient is not empty */
    unsigned char first;  /* The starting color of this gradient */
    unsigned char last;   /* The last color of this gradient */
    unsigned num_colors;  /* The number of colors in this gradient */
} GradientInfo;

/* ColorInfo contains information about all the gradients in a pallete */

typedef struct {
    GradientInfo gradient[ NUM_GRADIENTS ]; /* array of gradients */
    long size;                              /* number of active gradients */
} ColorInfo;





/* John's Typedefs ------------------------- */

typedef unsigned char input_table[ INPUT_TABLE_SIZE ];

/* Is the vehicle controlled by the local player,
   computer, or remote player? */
typedef enum { USER_CONTROL, AI_CONTROL, REMOTE_CONTROL, NO_CONTROL } controller_type;


typedef enum { ATTACK, GET_ENERGY, PANIC, BEZERK,
               HIDE, GROUP, GET_PYLONS, PROTECT,
               KILL_RADAR_BASE, PROTECT_RADAR_BASE, EVADE } state_type;

typedef struct
    {
     char            name[40];
     char            filename[15]; /* Name of the pcx file */
     string          sample_filenames[NUMBER_CHARACTER_SOUNDS];
     SYS_SOUNDHANDLE samples[NUMBER_CHARACTER_SOUNDS];
     state_type      state;

     unsigned char   passive_aggressive;     /* 1-100...1-50 is passive 51-100 is aggressive */
     unsigned char   bravery_cowardice;      /* 1-100...same scheme */
     unsigned char   aerial_ground;          /* 1-100...same scheme */
     unsigned char   obedience_disobedience; /* 1-100...same scheme */
     unsigned char   pylon_grab;             /* 1-50 hates pylons...50-100 likes em */
     unsigned char   radar_kill;             /* 1-50 wont kill, 50-100 loves to kill */
     unsigned char   radar_protect;          /* 1-50 protects, 50-100 doesn't care */
     unsigned char   skill_level;            /* Skill level 1-5  */
     unsigned char   preferred_vehicle;      /* See vehicletype */
    } character_type;


typedef struct
    {
     Vehicle          tank;        /* Vehicle data structure... */
     enum VehicleType tank_type;   /* What type of tank is this? */
     controller_type  controller;  /* Ai character? Player character? Remote? */
     input_table      table;       /* Joystick control, ai control etc all go through this */
     team_type        team;        /* What team is this character on? */
     boolean          leader;      /* Is this character the leader? */
     character_type   character;   /* Personality of the ai */
    } Player;


/* These are the different sound events that will happen in every cylinder...
   they should be stored in world_stuff...and the filenames of the samples
   should be stored in the level file */

     typedef enum { CAP_COLLISION, CYLINDER_COLLISION, VEHICLE_COLLISION,
                    PYLON_COLLISION, BULLET_FIRE, BULLET_HIT, BULLET_MISS,
                    MISSILE_FIRE, MISSILE_HIT, MISSILE_MISS,
                    VEHICLE_EXPLOSION, PYLON_TURN_BLUE, PYLON_TURN_RED,
                    ENERGY_RECHARGE, VEHICLE_MOTION, MODE_SWITCH_INTO_AIR,
                    MODE_SWITCH_ONTO_GROUND } sound_index_type;

/* These are the different things the computer will say */
typedef enum { RADAR_DESTROYED, TRANSFER, RED_TEAM_WINS, BLUE_TEAM_WINS,
               ATTACK_RADAR, DEFEND_RADAR, ATTACK_ENEMY,
               GRAB_PYLONS, GROUP_TOGETHER, CANCEL_ORDER,
               CONTROLS_LOST, CONTROLS_REGAINED, ROUND_ONE,
               ROUND_TWO, FINAL_ROUND
             } computer_sound_event_type;

typedef enum { GREETING, AFFIRMATION, NEGATION, GLOAT, DESPAIR,
               DEATH, VICTORY } character_sound_event_type;



/* Basic variables that will be stored in game.cfg */

typedef struct {
    unsigned int laser;
    unsigned int missile;
    unsigned int special;
    unsigned int mode_switch;
    unsigned int up;
    unsigned int down;
    unsigned int left;
    unsigned int right;
    unsigned int up_throttle;
    unsigned int down_throttle;
    unsigned int strafe;
    unsigned int wing_1_pylon;
    unsigned int wing_1_attack;
    unsigned int wing_1_attack_rb;
    unsigned int wing_1_defend_rb;
    unsigned int wing_1_group;
    unsigned int wing_1_cancel;
    unsigned int wing_2_pylon;
    unsigned int wing_2_attack;
    unsigned int wing_2_attack_rb;
    unsigned int wing_2_defend_rb;
    unsigned int wing_2_group;
    unsigned int wing_2_cancel;

} KeyboardConfig;

typedef enum {
    ShootLaser, ShootMissile, ActivateSpecial, ActivateStrafe,
    ThrottleUp, ThrottleDown, SwitchModes, DoNothing,
    StrafeUp, StrafeDown, StrafeLeft, StrafeRight
} JoystickButtonAction;

typedef struct {
    JoystickButtonAction button_1;
    JoystickButtonAction button_2;
    JoystickButtonAction button_3;
    JoystickButtonAction button_4;
    JoystickButtonAction hat_up;
    JoystickButtonAction hat_down;
    JoystickButtonAction hat_left;
    JoystickButtonAction hat_right;
} JoystickConfig;

typedef enum { NoJoystick, StandardJoystick, FlightStickPro } JoystickType;

typedef enum { NullModemGame, IpxNetworkGame, CustomGame, TournamentGame, TrainingGame } GameType;

typedef struct {
    char slot0[80];
    char slot1[80];
    char slot2[80];
    char slot3[80];
    char slot4[80];
    char slot5[80];
    char slot6[80];
    char slot7[80];    
} PilotNamesType;
    

typedef struct
    {
     int serial_com_port; /* Com port the null modem is hooked up to */
     int serial_baud;     /* Baud rate of the null modem */
     int modem_com_port;  /* Com port the modem is hooked up to */
     int modem_baud;      /* Baud rate of the modem */
     JoystickType joystick;    /* Is the joystick active and what type is it? */
     boolean mouse_active;    /* Is the mouse active */
     int world_detail_level; /* One of HIGH_DETAIL MED_DETAIL or LOW_DETAIL */
     int tube_detail_level;  /*                 ditto                       */
     boolean hubs_on;        /* Should we draw the hubs? */

     boolean sound_on;       /* Are we using sound? */
     boolean music_on;       /* Are we using cd-rom music? */
     boolean voices_on;      /* Are the voices on? */

     unsigned char sound_vol;  /* the sound fx volume 0-255 */
     unsigned char music_vol;  /* CD-rom music volume 0-255 */
     unsigned char voice_vol;  /* the voice volume 0-255 */

     int red0_vehicle;      /* red team's first vehicle */
     int red1_vehicle;      /* red team's second vehicle */
     int red2_vehicle;      /* red team's third vehicle */

     int blue0_vehicle;      /* blue team's first vehicle */
     int blue1_vehicle;      /* blue team's second vehicle */
     int blue2_vehicle;      /* blue team's third vehicle */

     int red0_ai;            /* ai controlling red team's first vehicle */
     int red1_ai;            /* ai controlling red team's second vehicle */
     int red2_ai;            /* ai controlling red team's third vehicle */

     int blue0_ai;           /* ai controlling blue team's first vehicle */
     int blue1_ai;           /* ai controlling blue team's second vehicle */
     int blue2_ai;           /* ai controlling blue team's third vehicle */

     boolean modem_game;     /* Is this a modem game? */
     boolean serial_game;    /* Is this a serial game? */

     char cylinder_filename[30];  /* Holds the level filename */

     KeyboardConfig keyboard_config;  /* holds scancodes for keyboard controls */
     JoystickConfig joystick_config;  /* holds actions for each button and hat */
     
     GameType game_type;  /* what kind of game are we playing? */     
     
     char pilot_name[80];  /* the current pilot's name */
     int slot_num;         /* which slot is this pilot in */
     
     PilotNamesType pilot_names;  /* all the saved pilot's names from game.cfg file */
     
     int animations_on;  /* does the user want to play animations? */
     int two_player; /* is the custom game two players? */
     int fullscreen; /* should rendering be fullscreen or in a window? */
    } game_configuration_type;

typedef struct
    {
     char pylon_config_filename[40];
     char energy_square_filename[40];
     char palette_filename[40];
     char animation_filename[40];
     char tube_filename[40];
     char turret_filename[40];
     char base_filename[40];
     char north_hub_filename[40];
     char south_hub_filename[40];
     boolean wire_tube;

     float angular_friction;  /* Radians per frame */
     float surface_friction;   /* Units per frame */
     float air_friction;       /* Units per frame */
     long yon_clipping_plane; /* Magic Fixed point */

     Orientation base_orientations[2];
     Orientation turret_orientations[2];
     Orientation vehicle_orientations[6];

     ColorInfo color_info;
    } level_type;



/* end of John's Typedefs ------------------ */

/* EnergyPowerUp contains all the information about an energy square */

typedef struct {
    PointFace *world_obj;    /* 3d object in (world coords) */
    BoundingBox2d box;       /* the 2d bounding box */
    long refresh_rate;       /* number of frames between bursts */
    long frames_till_burst;  /* frames till next energy burst. */
    Float_Point center;      /* used by ai */
} EnergyPowerUp;

/* RadarBase contains all the information about a radar base */

typedef struct {

    PointFace *base_obj;    /* 3d object of the turret base centered at the origin */
    PointFace *turret_obj;  /* 3d object of turret centered at the origin */

    PointFace *world_base_obj;    /* 3d object of turret base in world coords */
    PointFace *world_turret_obj;  /* 3d object of turret in world coords */

    PointFace *world_union_obj;  /* the union of world_base_obj & world_turret_obj */

    BoundingBox box;       /* the x, y & z extents of world_union_obj */
    EdgeTable edge_table;  /* the table of edges of world_union_obj */

    Orientation base_orient;    /* the orientation of base_obj */
    Orientation turret_orient;  /* the orientation of turret_obj */

    short current_hitpoints;  /* current number of hitpoints left */
    short max_hitpoints;      /* maximum number of hitpoint allowed */

    short alive;  /* tells if radar base is destroyed or not */

    float *target;  /* point that missiles are locked on (not used) */

    short max_projectiles;        /* maximum number of active projectiles allowed */
    Projectile *projectile_list;  /* list of active projectiles */

    short laser_life;              /* number of frames a laser remains active */
    float laser_speed;             /* speed of lasers */
    short frames_till_fire_laser;  /* frames left until base can fire */
    short laser_reload_time;       /* time it takes to reload after fire */
    short laser_damage;            /* hitpoints of damage when hit */

    team_type team;  /* team that owns this base */

} RadarBase;

/* BarType identifies the kind of bar is to be drawn */

typedef enum { VerticalTopJustified, VerticalBottomJustified,
               HorizontalLeftJustified, HorizontalRightJustified } BarType;

/* BarInfo contains all the necessary information about a bar so that
   it can be drawn on the screen */

typedef struct {
    Point2d upper_left;
    Point2d lower_right;
    long min_val;
    long max_val;
    long val;
    unsigned char bar_color;
    unsigned char outline_color;
    BarType type;
} BarInfo;

typedef struct {
    short x;
    short y;
    unsigned char color;
} Pixel;

typedef struct {
    Pixel *pixel;
    long pixels;
} PixMap;


/* CollisionInfo holds collision info for 1 vehicles */

typedef struct {
    long collision;        /* True if vehicle is colliding with something */
    Float_Vector reflect;  /* The sum of the normal vectors of the faces that
                              this vehicle is colliding with */
} CollisionInfo;

/* CollisionTable will hold the collision info for all the vehicles */

typedef CollisionInfo CollisionTable[6];

typedef enum { ship_explosion, laser_explosion, missile_explosion,
               radar_base_explosion, mine_explosion, cs_missile_explosion,
               ramming_explosion, traitor_missile_explosion } ExplosionType;

typedef struct explosion {
    ExplosionType type;
    Orientation orient;
    PointFace debris;
    PointFace world_debris;
    PointFace cloud;
    PointFace world_cloud;
    long frames_left;
    struct explosion *next;
} Explosion;


typedef struct {

    ColorInfo color_info;    /* information about the pallette */

    PointFace tube;          /* 3d-tube object */

    PointEdge wire_tube;     /* wireframe 3d-tube object */

    PointEdge wire_tinytube; /* wireframe tube used for radar */

    PointFace everything;    /* 3d-object that will hold all the pylons,
                                missiles, lasers & vehicles, ready to be
                                drawn.*/

    PointFace laser_obj;     /* 3d-object used for every vehicles lasers */

    PointFace missile_obj;   /* 3d-object used for every vehicles missiles */

    PointFace mine_obj;      /* 3d-object used for drawing a mine */

    PointFace cs_missile_obj;  /* 3d-object used for drawing a cs_missile */

    PointFace traitor_missile_obj;  /* 3d-object used for drawing a traitor_missile */

    PointFace laser_debris_obj;   /* 3d-object used for laser explosions */

    PointFace missile_debris_obj; /* 3d-object used for missile explosions */

    PointFace mine_debris_obj;    /* 3d-object used for mine explosions */

    PointFace cs_missile_debris_obj; /* 3d-object used for cs_missile_explosions */

    PointFace traitor_missile_debris_obj; /* 3d-object used for a traitor_missile_explosion */

    PointFace ramming_debris_obj; /* 3d-object used for ramming debris */

    PointFace explosion_cloud_obj;  /* 3d-object used for exploding vehicles & radar_bases */

    PointFace north_hub;  /* contains the hub for the north (pos z) side of the cylinder */

    PointFace south_hub;  /* contains the hub for the south (neg z) side of the cylinder */

    RadarBase red_radar_base;  /* contains info about the red teams radar base */

    RadarBase blue_radar_base; /* contains info about the blue teams radar base */

    Pylons pylons;       /* info about all the pylons */

    Point light;         /* current light position relative to the view_vehicle */

    Orientation view_orientation;  /* orientation that the world will be seen from */

    Player player_array[6];  /* array of all 6 players */

    EnergyPowerUp energy_square;  /* info about the energy_square */

    Explosion *explosion_list;  /* all the explosions in the world */

    pcx_picture text;           /* holds this levels pallete info */

    PixMap beetle_hud;     /* hud for each vehicle */
    PixMap wasp_hud;
    PixMap flea_hud;
    PixMap mosquito_hud;
    PixMap spider_hud;
    PixMap dragonfly_hud;
    PixMap roach_hud;
    PixMap locust_hud;

} WorldStuff;



typedef struct
    {
     SYS_SOUNDHANDLE enter_sound;
     SYS_SOUNDHANDLE esc_sound;
     SYS_SOUNDHANDLE toggle_sound;
     SYS_SOUNDHANDLE error_sound;
    } menu_sound_type;

typedef struct
    {
     SYS_SOUNDHANDLE flea_sound;
     SYS_SOUNDHANDLE locust_sound;
     SYS_SOUNDHANDLE spider_sound;
     SYS_SOUNDHANDLE wasp_sound;
     SYS_SOUNDHANDLE beetle_sound;
     SYS_SOUNDHANDLE mosquito_sound;
     SYS_SOUNDHANDLE dragonfly_sound;
     SYS_SOUNDHANDLE roach_sound;
    } vehicle_name_sound_type;

typedef struct
    {
     SYS_SOUNDHANDLE cap_sound;
     SYS_SOUNDHANDLE maj_sound;
     SYS_SOUNDHANDLE ric_sound;
     SYS_SOUNDHANDLE cin_sound;
     SYS_SOUNDHANDLE pre_sound;
     SYS_SOUNDHANDLE mrk_sound;
     SYS_SOUNDHANDLE kri_sound;
     SYS_SOUNDHANDLE sha_sound;
     SYS_SOUNDHANDLE kel_sound;
     SYS_SOUNDHANDLE meg_sound;

     SYS_SOUNDHANDLE bok_sound;
     SYS_SOUNDHANDLE nbo_sound;
     SYS_SOUNDHANDLE tbo_sound;

     SYS_SOUNDHANDLE pub_sound;
     SYS_SOUNDHANDLE gal_sound;
     SYS_SOUNDHANDLE sep_sound;

     SYS_SOUNDHANDLE hot_sound;
     SYS_SOUNDHANDLE cru_sound;
     SYS_SOUNDHANDLE gre_sound;

     SYS_SOUNDHANDLE cry_sound;
     SYS_SOUNDHANDLE gla_sound;
     SYS_SOUNDHANDLE azu_sound;

     SYS_SOUNDHANDLE baa_sound;
     SYS_SOUNDHANDLE mep_sound;
     SYS_SOUNDHANDLE mal_sound;

     SYS_SOUNDHANDLE anu_sound;
     SYS_SOUNDHANDLE bas_sound;
     SYS_SOUNDHANDLE hor_sound;

     SYS_SOUNDHANDLE neb_sound;
     SYS_SOUNDHANDLE pol_sound;
     SYS_SOUNDHANDLE qua_sound;

     SYS_SOUNDHANDLE r_sound;
     SYS_SOUNDHANDLE g_sound;
     SYS_SOUNDHANDLE b_sound;

     SYS_SOUNDHANDLE b00_sound;
     SYS_SOUNDHANDLE d23_sound;
     SYS_SOUNDHANDLE jld_sound;

    } character_sound_type;

typedef struct {

    pcx_picture menu_text;  /* holds the character data for menu text */

    pcx_picture micro_text; /* holds the micro character data */

    pcx_picture general_menu_background;  /* holds the general menu background screen */

    pcx_picture wingman_menu_background;  /* holds the wingman menu background picture */

    pcx_picture vehicle_menu_background;  /* holds the vehicle menu background picture */

    pcx_picture human_1;    /* first 3 humans head shots */
    pcx_picture human_2;    /* next 3 humans head shots  */
    pcx_picture human_3;    /* another 3 humans */
    pcx_picture human_4;    /* last human */
    pcx_picture overlords;  /* 3 overlords */
    pcx_picture pharoahs;   /* 3 pharoahs */
    pcx_picture scavengers; /* 3 scavengers */
    pcx_picture slar;       /* 3 slar */
    pcx_picture succubi;    /* 3 succubi */
    pcx_picture watchers;   /* 3 watchers */
    pcx_picture bok;        /* 3 boks */
    pcx_picture biomech;    /* 3 biomechinoids */
    pcx_picture sentry;     /* 3 sentry */

    PointFace wasp_obj;       /* 3d-object for the wasp (centered at origin) */
    PointFace beetle_obj;     /* 3d-object for the beetle */
    PointFace flea_obj;       /* 3d-object for the flea */
    PointFace mosquito_obj;   /* 3d-object for the mosquito */
    PointFace spider_obj;     /* 3d-object for the spider */
    PointFace locust_obj;     /* 3d-object for the locust */
    PointFace roach_obj;      /* 3d-object for the roach */
    PointFace dragonfly_obj;  /* 3d-object for the dragonfly */
    menu_sound_type menu_sounds; /* Sounds in the menu */
    vehicle_name_sound_type vehicle_name_sounds;
    character_sound_type    character_sounds;

} MenuStuff;

/* used by move_vehicles() to process input */

typedef struct {
    input_table table;
    controller_type type;
} InputPackage;

typedef struct {
    InputPackage input_package[6];
} Snapshot;

typedef struct {
    Snapshot snapshot[15];
    long size;
} SnapshotQueue;

typedef RGB_color palette_type[256];

#endif
