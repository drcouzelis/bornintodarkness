/**
 * Copyright 2009 David Couzelis
 * 
 * This file is part of "Born into Darkness".
 * 
 * "Born into Darkness" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * "Born into Darkness" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with "Born into Darkness".  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "allegro.h"


#define SCREEN_X_SIZE  640
#define SCREEN_Y_SIZE  480
#define TILE_SIZE  32
#define SECONDS  1000



typedef struct {
   int x;
   int y;
} DIRECTION;

typedef struct WARP_SPACE_T {
   int to_map;                     //map warping to
   int from_x;                     //from location on original map
   int from_y;
   int to_x;                       //to location on map warping to
   int to_y;
   struct WARP_SPACE_T *nextWARP_SPACE;  //points to the next warp =)
} WARP_SPACE;

typedef struct WARP_TILE_T {
   int to_map;
   int from_tile;
   int to_x;
   int to_y;
   struct WARP_TILE_T *nextWARP_TILE;
} WARP_TILE;

typedef struct {
   int passable;                   //torf: if you can walk through or not
   BITMAP *bmp;                    //pic name
   int place_number;               //where in pic the tile is stored
} TILE;

typedef struct FRAME_T {
   BITMAP *frame_bmp;
   struct FRAME_T *next_frame;
} FRAME;

typedef struct {
   char name[150];
   int key;
   DIRECTION facing;
   int frames;
   FRAME *frame;
} ANIMATION;

typedef struct {
   int x_location;                 //stores current location of char
   int y_location;
   DIRECTION facing;
   BITMAP *bmp[100];               //pic frames
   ANIMATION *animation[50];
   int current_anim;
   int num_of_anims;
   int frames;                     //how many pics/frames to be stored
   int hit_points;
} CHARACTER;

typedef struct {
   char map_name[500];
   int start_x, start_y;           //default character starting placement
   int x_size, y_size;             //size of map
   int *passable_map;
   int *tile_map;
   BITMAP *tiles;
   //BITMAP *background;
   WARP_SPACE *warp_space;
   WARP_TILE *warp_tile;
} MAP;



//int get_char_inkey (void);             //gets keyboard movement for current char
int check_passable (int, int);           //checks if char is allowed to mover there
int check_for_warp (void);               //returns 1 if you're on a warp, and it moves you
void play_the_game (void);

int load_data (char *);                  //this function calls load_map, load_tile, load_char
int load_map (FILE *, int);
int load_tile (FILE *, int);
int load_char (FILE *, int);
void configure_map_name (char *);

//void animate_walk (void);
void draw_screen (void);                 //redraws the screen
void animate ();
void animate_walk (int, int);
int initialize_game (void);              //initializes graphics modes and such
void initialize_graphics (void);         //initializes all pics / pic files, data file
void initialize_map (int);               //initializes the map number sent in
void initialize_char (int);            //initializes the char number sent in
void add_frame (FRAME *, BITMAP *, int);



MAP *map[999];
TILE *tile[999];
CHARACTER *character[999];

MAP *currentMAP;
CHARACTER *currentCHAR;
BITMAP *camera_buffer;
PALLETE game_pallete;

int current_anim;

int game_over;
int quitting = 0;
int fullscreen = 0;


char blank;


int main (int argc, char *argv[])
{


   /*int x, y, i;*/

   if (!initialize_game ())
      {printf ("COULD NOT INITIALIZE GAME. \n");  return 0;}
   printf ("initialized \n");

   initialize_graphics ();
   //readkey ();

   /*
   for (y=0; y<map[1]->y_size; y+=1)
   {
      for (x=0; x<map[1]->x_size; x+=1)
      {
         printf ("%d", map[1]->tile_map[y*map[1]->x_size+x]);
      }
      printf ("\n");
   }
   */


   //this is the display part
   //draw_screen ();

   play_the_game ();

   //while (get_char_inkey());
      //draw_screen ();

   printf ("DONE \n");
   //readkey();
   return 1;


}
END_OF_MAIN()


void play_the_game (void)
{

   int i;
   /*int frame_counter=1;*/
   float start_timer;

   game_over = 0;
   initialize_map (1);
   initialize_char (1);

   while (!game_over)
   {
      //while (!keypressed ())
      //{
         start_timer = clock ();
         do
         {
            draw_screen ();
            //get_character_inkey ();

            if (keypressed ())
            {
               if (key[KEY_ESC])
                  game_over = 1;
               else if (key[KEY_F]) {
                  if (fullscreen == 1) {
                     set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_X_SIZE, SCREEN_Y_SIZE, 0, 0);
                     set_pallete (game_pallete);
                     fullscreen = 0;
                  } else {
                     set_gfx_mode(GFX_AUTODETECT, SCREEN_X_SIZE, SCREEN_Y_SIZE, 0, 0);
                     set_pallete (game_pallete);
                     fullscreen = 1;
                  }
               }
               else
               {

                  //currentCHAR->x_location += 1;


                  //printf ("past esc \n");   readkey ();
                  //while (keypressed ())
                  //{
                     for (i=0; i<currentCHAR->num_of_anims; i+=1)
                        if (currentCHAR->animation[i]->key != 0 && key[currentCHAR->animation[i]->key])
                        {

                           //printf ("i = %d \n", i);  readkey ();
                           currentCHAR->current_anim = i;
                           //arrow_was_press = 1;

                           //printf ("changing facing... \n");  readkey ();
                           //printf ("...to x: %d  y: %d \n", currentCHAR->animation[i]->facing.x, currentCHAR->animation[i]->facing.y);  readkey ();

                           currentCHAR->facing.x = currentCHAR->animation[currentCHAR->current_anim]->facing.x;
                           currentCHAR->facing.y = currentCHAR->animation[currentCHAR->current_anim]->facing.y;

                           while (key[currentCHAR->animation[i]->key] && check_passable ((currentCHAR->x_location/32)+currentCHAR->facing.x-9, (currentCHAR->y_location/32)+currentCHAR->facing.y-7))
                           {
                              //printf ("ready to check passable \n");  readkey ();
                              //if (check_passable ((currentCHAR->x_location/32)+currentCHAR->facing.x-9, (currentCHAR->y_location/32)+currentCHAR->facing.y-7))
                              {
                                 //printf ("animating \n");  readkey ();
                                 animate_walk (currentCHAR->facing.x, currentCHAR->facing.y);
                                 check_for_warp ();
                              }
                           }
                        }
                  //}

                  for (i=0; i < currentCHAR->num_of_anims; i+=1)
                     if (currentCHAR->animation[i]->key == 0)
                        if (currentCHAR->facing.x == currentCHAR->animation[i]->facing.x && currentCHAR->facing.y == currentCHAR->animation[i]->facing.y)
                        {
                           currentCHAR->current_anim = i;
                           i = currentCHAR->num_of_anims;
                        }

                  //i = currentCHAR->num_of_anims;



               }
            }

         }
         while (clock () - start_timer < .04*SECONDS);
         //if (frame_counter == 4)
         {
            animate ();
            //frame_counter=0;
         }
         //frame_counter+=1;
      //}
      //readkey ();
      //game_over = 1;

   }



}


void animate_walk (int x_dir, int y_dir)
{

   int init_x = currentCHAR->x_location;
   int init_y = currentCHAR->y_location;
   /*float start_timer;*/
   int i, frame_counter=1;

   //printf ("in animation!! \n");  readkey ();

   i = 1;

   do
   {
      draw_screen ();
      //rest (.01*SECONDS);

      if (frame_counter == 4)
      {
         animate ();
         frame_counter=0;
      }
      currentCHAR->x_location += 1*x_dir;
      currentCHAR->y_location += 1*y_dir;
      frame_counter+=1;

   }
   while ((currentCHAR->x_location - init_x)*x_dir < 32 && (currentCHAR->y_location - init_y)*y_dir < 32);



      /*
      start_timer = clock ();

      do
      {

         //printf ("in animation!! \n");  readkey ();
         currentCHAR->x_location += x_dir;
         currentCHAR->y_location += y_dir;

         draw_screen ();
         if (clock () - start_timer > .04*SECONDS)
         {
            animate ();
            start_timer = clock ();
         }
      }
      while ((currentCHAR->x_location - init_x)*x_dir < 32 && (currentCHAR->y_location - init_y)*y_dir < 32);
      */
   //currentCHAR->x_location += x_dir*TILE_SIZE;
   //currentCHAR->y_location += y_dir*TILE_SIZE;


}



int check_for_warp (void)
{

   WARP_SPACE *tempWARP_SPACE;
   WARP_TILE *tempWARP_TILE;

   for (tempWARP_SPACE = currentMAP->warp_space; tempWARP_SPACE != NULL; tempWARP_SPACE = tempWARP_SPACE->nextWARP_SPACE)
   {

      if (tempWARP_SPACE->from_x == currentCHAR->x_location/32-9)
         if (tempWARP_SPACE->from_y == currentCHAR->y_location/32-7)
         {
            currentCHAR->x_location = (tempWARP_SPACE->to_x+9)*32;
            currentCHAR->y_location = (tempWARP_SPACE->to_y+7)*32;
            initialize_map (tempWARP_SPACE->to_map);
            return (1);
         }
   }

   for (tempWARP_TILE = currentMAP->warp_tile; tempWARP_TILE != NULL; tempWARP_TILE = tempWARP_TILE->nextWARP_TILE)
   {

      if (currentMAP->tile_map[(currentCHAR->y_location/32-7)*currentMAP->x_size+(currentCHAR->x_location/32-9)]  == tempWARP_TILE->from_tile)
      {
         currentCHAR->x_location = (tempWARP_TILE->to_x+9)*32;
         currentCHAR->y_location = (tempWARP_TILE->to_y+7)*32;
         initialize_map (tempWARP_TILE->to_map);
         return (1);
      }
   }

   return (0);

}



int check_passable (int x_loc, int y_loc)
{
   //printf ("checking passability \n");  readkey ();

   if (x_loc >= 0 && x_loc < currentMAP->x_size)
      if (y_loc >= 0 && y_loc < currentMAP->y_size)
         if (tile[currentMAP->tile_map[y_loc*currentMAP->x_size+x_loc]]->passable)
            return 1;

   return 0;

}




void draw_screen (void)
{

   /*int i, x, y;*/

   blit (currentMAP->tiles, camera_buffer, (currentCHAR->x_location-(9*TILE_SIZE)), (currentCHAR->y_location-(7*TILE_SIZE)), 0, 0, (currentMAP->x_size+SCREEN_X_SIZE/TILE_SIZE)*TILE_SIZE, (currentMAP->y_size+SCREEN_Y_SIZE/TILE_SIZE)*TILE_SIZE);
   draw_sprite (camera_buffer, currentCHAR->animation[currentCHAR->current_anim]->frame->frame_bmp, (9)*TILE_SIZE, (7)*TILE_SIZE);
   //draw_sprite (camera_buffer, currentCHAR->bmp[1], 0, 0);
   //ANIMATE!!
   //currentCHAR->animation[5]->frame = currentCHAR->animation[5]->frame->next_frame;

   textprintf_ex (camera_buffer, font, 0, 0, 215, -1, "%s", currentMAP->map_name);
   textprintf_ex (camera_buffer, font, 0, 32, 215, -1, "x loc %d  y loc %d", currentCHAR->x_location, currentCHAR->y_location);
   textprintf_ex (camera_buffer, font, 0, 64, 215, -1, "x map size %d  y map size %d", currentMAP->x_size, currentMAP->y_size);
   textprintf_ex (camera_buffer, font, 0, 96, 215, -1, "tile to check %d ", currentMAP->tile_map[((currentCHAR->y_location/32)+currentCHAR->facing.y-7) *currentMAP->x_size+ ((currentCHAR->x_location/32)+currentCHAR->facing.x-9)]);

   //now finish it up by displaying it all!
   blit (camera_buffer, screen, 0, 0, 0, 0, SCREEN_X_SIZE, SCREEN_Y_SIZE);

   rest (.005*SECONDS);

   //printf ("done with drawing screen \n");  readkey ();

}



void animate (void)
{

   currentCHAR->animation[currentCHAR->current_anim]->frame = currentCHAR->animation[currentCHAR->current_anim]->frame->next_frame;
   //the_animation->frame = the_animation->frame->next_frame;

}




void initialize_map (int map_number)
{
   int x, y;

   if (map[map_number] != currentMAP)
   {
      currentMAP = map[map_number];

      //if (currentMAP->tiles == NULL)
      {

         currentMAP->tiles = create_bitmap ((currentMAP->x_size)*TILE_SIZE+(SCREEN_X_SIZE), (currentMAP->y_size)*TILE_SIZE+(SCREEN_Y_SIZE));
         clear (currentMAP->tiles);

         //printf ("map all clear \n");

         for (y=0; y < currentMAP->y_size+15; y+=1)
            for (x=0; x<currentMAP->x_size+20; x+=1)
            {
               //printf ("about to blit \n"); readkey ();
               blit (tile[currentMAP->tile_map[0]]->bmp, currentMAP->tiles, 0, 0, x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
               //blit (tile[currentMAP->tile_map[0]]->bmp, screen, 0, 0, x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
               //readkey ();
            }
         //printf ("through background loading \n");  readkey ();

         for (y = 0; y < currentMAP->y_size; y+=1)
         {
            for (x = 0; x < currentMAP->x_size; x+=1)
            {
               blit (tile[currentMAP->tile_map[y*currentMAP->x_size+x]]->bmp, currentMAP->tiles, 0, 0, (x+9)*TILE_SIZE, (y+7)*TILE_SIZE, TILE_SIZE, TILE_SIZE);
               //blit (tile[currentMAP->tile_map[y*currentMAP->x_size+x]]->bmp, screen, 0, 0, (x+9)*TILE_SIZE, (y+7)*TILE_SIZE, TILE_SIZE, TILE_SIZE);
               //readkey ();
            }
            //readkey ();
         }
         //printf ("through tile loading \n");  readkey ();
      }
   }
   //for test purposes
   //blit (currentMAP->tiles, screen, 0, 0, 0, 0, SCREEN_X_SIZE, SCREEN_Y_SIZE);
   //readkey ();

}


void initialize_char (int char_number)
{

   int i;

   currentCHAR = character[char_number];

   currentCHAR->x_location = currentMAP->start_x+(9*TILE_SIZE);
   currentCHAR->y_location = currentMAP->start_y+(7*TILE_SIZE);

   //printf ("init.x  %d \ninit.y  %d \n", currentMAP->start_x, currentMAP->start_y);
   //printf ("char.x  %d \nchar.y  %d \n", currentCHAR->x_location, currentCHAR->y_location);
   //readkey ();

   currentCHAR->facing.x=0;
   currentCHAR->facing.y=1;

   for (i=0; i < currentCHAR->num_of_anims; i+=1)
   {
      //printf ("i = %d \n", i);  readkey ();
      if (currentCHAR->animation[i]->key == 0)
         if (currentCHAR->facing.x == currentCHAR->animation[i]->facing.x && currentCHAR->facing.y == currentCHAR->animation[i]->facing.y)
         {
            currentCHAR->current_anim = i;
            //printf ("current char is %d \n", i);  readkey ();
            i = currentCHAR->num_of_anims;
         }
   }

   //printf ("done initializing character \n"); readkey ();

}




int load_character (FILE *di, int characterINT)
{
   BITMAP *whole_bmp;
   ANIMATION *tempANIM;
   /*FRAME *newFRAME, *startFRAME, *currFRAME;*/
   int i, tempINT, animINT=0;
   /*int x, y;*/
   char bmp_file_name[20];
   char tempSTRING[30];
   /*int tempINT2;*/


   whole_bmp = create_bitmap (500*TILE_SIZE, TILE_SIZE);
   clear (whole_bmp);

   //printf ("bmp created \n"); readkey ();


   do
   {
      fscanf (di, "%s", tempSTRING);

      if (strcmp ("file_name", tempSTRING) == 0)
      {
         fscanf (di, "%s", bmp_file_name);
         //printf ("file name loaded \n"); readkey ();

      }
         //character[characterINV]->bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
         //clear (character[characterINT]->bmp);
         //character[characterINT]->place_number = 0;

         //fscanf (di, "%s", tempSTRING);
      if (strcmp ("frames", tempSTRING) == 0)
      {
         //printf ("I am in here first \n");  readkey ();

         fscanf (di, "%d", &(character[characterINT]->frames));
         printf ("character frames  %d \n", character[characterINT]->frames);
         //printf ("frames loaded %d \n", character[characterINT]->frames);  readkey ();

         whole_bmp = load_bitmap (bmp_file_name, game_pallete);
         for (i=0; i<character[characterINT]->frames; i+=1)
         {
            character[characterINT]->bmp[i] = create_bitmap (TILE_SIZE, TILE_SIZE);
            clear (character[characterINT]->bmp[i]);

            //printf ("I am here. \n");  readkey ();
            blit (whole_bmp, character[characterINT]->bmp[i], i*TILE_SIZE, 0, 0, 0, TILE_SIZE, TILE_SIZE);
            //blit (character[characterINT]->bmp[i], screen, 0, 0, 0, 0, TILE_SIZE, TILE_SIZE);
            //readkey ();
            //printf ("done loading individual bmp \n"); readkey ();
         }

      }

      //character[characterINT]->bmp = (BITMAP *) calloc (character[characterINT]->frames, sizeof (BITMAP));


      if (strcmp ("animation", tempSTRING) == 0)
      {
         tempANIM = (ANIMATION *) malloc (sizeof (ANIMATION));
         tempANIM->frame = (FRAME *) malloc (sizeof (FRAME));

         fscanf (di, "%s", tempANIM->name);

         fscanf (di, "%s", tempSTRING);
         fscanf (di, "%d", &(tempANIM->key));

         fscanf (di, "%s", tempSTRING);
         fscanf (di, "%d", &(tempANIM->facing.x));
         fscanf (di, "%d", &(tempANIM->facing.y));

         fscanf (di, "%d", &(tempANIM->frames));

         character[characterINT]->animation[animINT] = tempANIM;
         fscanf (di, "%s", tempSTRING);

         //puts (character[characterINT]->animation[animINT]->name);
         //printf ("key %d \n", tempANIM->key);
         //printf ("look x,y  %d %d \n", character[characterINT]->animation[animINT]->facing.x, character[characterINT]->animation[animINT]->facing.y);
         //printf ("anim frames %d \n", character[characterINT]->animation[animINT]->frames);  readkey();

         //startFRAME = character[characterINT]->animation[animINT]->frame;
         //readkey ();


         //fscanf (di, "%d", &tempINT);
         //printf ("-frame num load... \n"); readkey ();

         /*
         character[characterINT]->animation[animINT]->frame = (FRAME *) malloc (sizeof (FRAME));

         character[characterINT]->animation[animINT]->frame->frame_bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
         clear (character[characterINT]->animation[animINT]->frame->frame_bmp);

         character[characterINT]->animation[animINT]->frame->frame_bmp = character[characterINT]->bmp[tempINT];
         character[characterINT]->animation[animINT]->frame = startFRAME;
         currFRAME = startFRAME;

         //blit (startFRAME->frame_bmp, screen, 0, 0, 0, 0, TILE_SIZE, TILE_SIZE);
         //readkey ();

         for (i=1; i < character[characterINT]->animation[animINT]->frames; i+=1)
         {
            fscanf (di, "%d", &tempINT);
            //printf ("-frame num load... \n"); readkey ();

            newFRAME = (FRAME *) malloc (sizeof (FRAME));

            character[characterINT]->animation[animINT]->frame->frame_bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
            clear (character[characterINT]->animation[animINT]->frame->frame_bmp);

            newFRAME->frame_bmp = character[characterINT]->bmp[tempINT];
            newFRAME->next_frame = startFRAME;
            currFRAME = newFRAME;

         }
         */

         for (i=0; i < character[characterINT]->animation[animINT]->frames; i+=1)
         {
            fscanf (di, "%d", &tempINT);
            tempINT -= 1;
            //printf ("on pic %d \n", tempINT);
            add_frame (character[characterINT]->animation[animINT]->frame, character[characterINT]->bmp[tempINT], i+1);
            //blit (character[characterINT]->animation[animINT]->frame->frame_bmp, screen, 0, 0, 0, 0, 32, 32);
            //printf ("loaded a frame \n");
            //readkey ();
         }
         /*
         if (animINT == 1)
         {
            printf ("prepare for test \n"); readkey ();
            for (i=0; i < 3*character[characterINT]->animation[animINT]->frames; i+=1)
            {
               blit (character[characterINT]->animation[animINT]->frame->frame_bmp, screen, 0, 0, 0, 0, 32, 32);
               readkey ();
               character[characterINT]->animation[animINT]->frame = character[characterINT]->animation[animINT]->frame->next_frame;
            }
         }
         */
         animINT += 1;
         //printf ("done loading animation \n");  readkey ();

      }

      character[characterINT]->num_of_anims = animINT;

   }
   while (strcmp ("end_character", tempSTRING) != 0);


   destroy_bitmap (whole_bmp);

   return (1);

}


int load_data (char *file_name)
{
   FILE *di;
   int tempINT;
   char tempSTRING[30];

   if ((di = fopen (file_name, "r")) == NULL)
      return (0);


   while ((fscanf (di, "%s", tempSTRING)) != EOF)
   {

      if (strcmp ("map_number", tempSTRING) == 0)
      {
         fscanf (di, "%d", &tempINT);
         printf ("map number %d being loaded \n", tempINT);
         map[tempINT] = (MAP *) malloc (sizeof (MAP));
         load_map (di, tempINT);

         //printf ("a map is done. \n");  readkey();
      }

      if (strcmp ("tile_number", tempSTRING) == 0)
      {
         fscanf (di, "%d", &tempINT);
         tile[tempINT] = (TILE *) malloc (sizeof (TILE));
         fscanf (di, "%s", tempSTRING);
         load_tile (di, tempINT);

         //printf ("a tile is done. \n");  readkey();

      }

      if (strcmp ("character_number", tempSTRING) == 0)
      {
         fscanf (di, "%d", &tempINT);
         character[tempINT] = (CHARACTER *) malloc (sizeof (CHARACTER));
         load_character (di, tempINT);

         printf ("a char is done. \n");  //readkey();


      }
   }

   fclose (di);

   return (1);
}





void configure_map_name (char *map_name)
{

   char tempSTRING[150];
   int i, j;

   for (i=0; map_name[i] != '['; i+=1)
      ;
   i+=1;
   for (j=0; map_name[i] != ']'; i+=1, j+=1)
      tempSTRING[j] = map_name[i];

   for (i=0; i<j; i+=1)
      map_name[i] = tempSTRING[i];
   map_name[i] = '\0';

}


int load_map (FILE *di, int mapINT)
{

   int x, y;
   WARP_SPACE *tempWARP_SPACE;
   WARP_TILE *tempWARP_TILE;
   char tempSTRING[30];

   map[mapINT]->warp_space = NULL;
   map[mapINT]->warp_tile = NULL;
   map[mapINT]->map_name[0] = '\0';

   do
   {
      fscanf (di, "%s", tempSTRING);

      if (strcmp ("map_name", tempSTRING) == 0)
      {
         fgets (map[mapINT]->map_name, 150, di);
         configure_map_name (map[mapINT]->map_name);
         //printf ("map name:  %s \n", map[mapINT]->map_name);
      }

      if (strcmp ("start_x", tempSTRING) == 0)
      {
         fscanf (di, "%d", &(map[mapINT]->start_x));
         map[mapINT]->start_x *= TILE_SIZE;
         //printf ("start x = %d \n", map[mapINT]->start_x);
      }
      if (strcmp ("start_y", tempSTRING) == 0)
      {
         fscanf (di, "%d", &(map[mapINT]->start_y));
         map[mapINT]->start_y *= TILE_SIZE;
         //printf ("start y = %d \n", map[mapINT]->start_y);
      }

      if (strcmp ("x_size", tempSTRING) == 0)
      {
         fscanf (di, "%d", &(map[mapINT]->x_size));
         //printf ("x size = %d \n", map[mapINT]->x_size);
      }
      if (strcmp ("y_size", tempSTRING) == 0)
      {
         fscanf (di, "%d", &(map[mapINT]->y_size));
         //printf ("x size = %d \n", map[mapINT]->x_size);
      }

      if (strcmp ("tile_map", tempSTRING) == 0)
      {
         map[mapINT]->tile_map = (int *) calloc (map[mapINT]->x_size*map[mapINT]->y_size, sizeof (int));
         for (y=0; y < map[mapINT]->y_size; y+=1)
            for (x=0; x < map[mapINT]->x_size; x+=1)
               fscanf (di, "%d", &(map[mapINT]->tile_map[y*map[mapINT]->x_size+x]));
      }

      if (strcmp ("warp_space", tempSTRING) == 0)
      {

         tempWARP_SPACE = (WARP_SPACE *) malloc (sizeof (WARP_SPACE));
         fscanf (di, "%d", &(tempWARP_SPACE->to_map));
         fscanf (di, "%d", &(tempWARP_SPACE->from_x));
         fscanf (di, "%d", &(tempWARP_SPACE->from_y));
         fscanf (di, "%d", &(tempWARP_SPACE->to_x));
         fscanf (di, "%d", &(tempWARP_SPACE->to_y));

         tempWARP_SPACE->nextWARP_SPACE = map[mapINT]->warp_space;
         map[mapINT]->warp_space = tempWARP_SPACE;

         //warp_space  3  14 9  1 1
      }

      if (strcmp ("warp_tile", tempSTRING) == 0)
      {
         tempWARP_TILE = (WARP_TILE *) malloc (sizeof (WARP_TILE));
         fscanf (di, "%d", &(tempWARP_TILE->to_map));
         fscanf (di, "%d", &(tempWARP_TILE->from_tile));
         fscanf (di, "%d", &(tempWARP_TILE->to_x));
         fscanf (di, "%d", &(tempWARP_TILE->to_y));

         tempWARP_TILE->nextWARP_TILE = map[mapINT]->warp_tile;
         map[mapINT]->warp_tile = tempWARP_TILE;

      }


   }
   while ((strcmp ("end_map", tempSTRING) != 0));



   return (1);

}


int load_tile (FILE *di, int tileINT)
{
   BITMAP *whole_bmp;
   /*int x, y;*/
   char bmp_file_name[20];
   char tempSTRING[30];

   whole_bmp = create_bitmap (100*TILE_SIZE, TILE_SIZE);
   clear (whole_bmp);


   do
   {
      fscanf (di, "%s", tempSTRING);

      if (strcmp ("passable", tempSTRING) == 0)
         fscanf (di, "%d", &(tile[tileINT]->passable));

      if (strcmp ("file_name", tempSTRING) == 0)
      {
         fscanf (di, "%s", bmp_file_name);
         tile[tileINT]->bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
         clear (tile[tileINT]->bmp);
         tile[tileINT]->place_number = 0;

         fscanf (di, "%s", tempSTRING);
         if (strcmp ("place_number", tempSTRING) == 0)
            fscanf (di, "%d", &(tile[tileINT]->place_number));

         whole_bmp = load_bitmap (bmp_file_name, game_pallete);
         blit (whole_bmp, tile[tileINT]->bmp, tile[tileINT]->place_number*TILE_SIZE, 0, 0, 0, TILE_SIZE, TILE_SIZE);

      }


   }
   while (strcmp ("end_tile", tempSTRING) != 0);

   destroy_bitmap (whole_bmp);

   return (1);

}


int initialize_game (void)
{

   allegro_init ();
   install_keyboard ();
   //set_color_depth (24);
   //set_gfx_mode (GFX_AUTODETECT, SCREEN_X_SIZE, SCREEN_Y_SIZE, 0, 0);
   set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_X_SIZE, SCREEN_Y_SIZE, 0, 0);
   //text_mode (-1);
   install_timer ();

   //load external file
   if (!load_data ("data.txt"))
      return 0;

   return 1;

}


void initialize_graphics (void)
{

   camera_buffer = create_bitmap (SCREEN_X_SIZE, SCREEN_Y_SIZE);
   clear (camera_buffer);

   //set to window's pallette
   set_pallete (game_pallete);
   printf ("pallete set \n");

}


void add_frame (FRAME *frame_set, BITMAP *bmp_to_add, int frame_number)
{

   //FRAME *new_frame;
   FRAME *temp_frame;

   /*
   //blit (bmp_to_add, screen, 0, 0, 0, 0, 32, 32);
   new_frame = (FRAME *) malloc (sizeof (FRAME));
   new_frame->frame_bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
   clear (new_frame->frame_bmp);
   blit (bmp_to_add, new_frame->frame_bmp, 0, 0, 0, 0, TILE_SIZE, TILE_SIZE);
   //blit (new_frame->frame_bmp, screen, 0, 0, 0, 0, 32, 32);  readkey ();
   */

   //printf ("new frame ready \n"); readkey ();

   if (frame_number == 1)
   {
      //printf ("I am in 1 \n");  readkey ();
      frame_set->frame_bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
      clear (frame_set->frame_bmp);
      blit (bmp_to_add, frame_set->frame_bmp, 0, 0, 0, 0, TILE_SIZE, TILE_SIZE);
      frame_set->next_frame = frame_set;
      //frame_set->frame_bmp = new_frame->frame_bmp;
      //frame_set->next_frame = new_frame;

      //blit (frame_set->frame_bmp, screen, 0, 0, 0, 0, 32, 32);  readkey ();
      //printf ("initial frame set. \n");  readkey ();
   }

   else
   {
      //printf ("I am in 2 \n");  //readkey ();
      temp_frame = frame_set->next_frame;

      frame_set->next_frame = (FRAME *) malloc (sizeof (FRAME));
      frame_set->next_frame->frame_bmp = create_bitmap (TILE_SIZE, TILE_SIZE);
      clear (frame_set->next_frame->frame_bmp);
      blit (bmp_to_add, frame_set->next_frame->frame_bmp, 0, 0, 0, 0, TILE_SIZE, TILE_SIZE);

      frame_set->next_frame->next_frame = temp_frame;
      frame_set = frame_set->next_frame;

      //blit (frame_set->frame_bmp, screen, 0, 0, 0, 0, 32, 32);  readkey ();
      //printf ("next frame loaded \n");
      //readkey ();
   }

}





