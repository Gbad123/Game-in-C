
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


#include "perlin.h"
#include "graphics.h"
#include "test.h"

extern GLubyte  world[WORLDX][WORLDY][WORLDZ];

	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void setOldViewPosition(float, float, float);
extern void setViewOrientation(float, float, float);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern void addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* tube controls */
extern void createTube(int, float, float, float, float, float, float, int);
extern void hideTube(int);
extern void showTube(int);

	/* 2D drawing functions */
extern void  draw2Dline(int, int, int, int, int);
extern void  draw2Dbox(int, int, int, int);
extern void  draw2Dtriangle(int, int, int, int, int, int);
extern void  set2Dcolour(float []);

	/* texture functions */
extern int setAssignedTexture(int, int);
extern void unsetAssignedTexture(int);
extern int getAssignedTexture(int);
extern void setTextureOffset(int, float, float);


	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate the space bar has been pressed */
extern int space;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 
	/* size of the window in pixels */
extern int screenWidth, screenHeight;
	/* flag indicates if map is to be printed */
extern int displayMap;
	/* flag indicates use of a fixed viewpoint */
extern int fixedVP;

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

	/* allows users to define colours */
extern int setUserColour(int, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
    GLfloat, GLfloat, GLfloat);
void unsetUserColour(int);
extern void getUserColour(int, GLfloat *, GLfloat *, GLfloat *, GLfloat *,
    GLfloat *, GLfloat *, GLfloat *, GLfloat *); 

	/* mesh creation, translatio, rotation functions */
extern void setMeshID(int, int, float, float, float);
extern void unsetMeshID(int);
extern void setTranslateMesh(int, float, float, float);
extern void setRotateMesh(int, float, float, float);
extern void setScaleMesh(int, float);
extern void drawMesh(int);
extern void hideMesh(int);

/********* end of extern variable declarations **************/

   /* my variables */
extern int stairDownLocX;
extern int stairDownLocZ;
extern int stairDownLocY;
extern int stairUpLocX;
extern int stairUpLocY;
extern int stairUpLocZ;
extern int flagOut;
extern int stairFlag;
extern float meshData[10][10];
extern int roomLenX[9];
extern int roomLenZ[9];
extern int roomX[9];
extern int roomZ[9];

int turn = 0;
float playerPostionX = 0,playerPostionZ = 0; 
int GPP[10][2];
int count = 0;

/********* end of my external variable declarations **************/

	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
   /* your code for collisions goes here */
   //variable declaration
   float oldX, oldY, oldZ;
   float curX,curY,curZ;
   getOldViewPosition(&oldX, &oldY, &oldZ);
   getViewPosition(&curX, &curY, &curZ);

   int x = (int)curX*-1;
   int y = (int)curY*-1;
   int z = (int)curZ*-1;

   for(int i = 0; i < 9; i++){
      //get mod location
      int mobX = (int)meshData[i][0];
      int mobZ = (int)meshData[i][1];
      
      //check the 9 squares around the mob
      for (int j = mobX-1; j <= mobX+1; j++){
         for (int k = mobZ-1; k <= mobZ+1; k++){
            //if player found, mesh is in the attack state, and mesh is alive
            if (j == x && k == z && meshData[i][5] == 1 && meshData[i][6] == 0){
               //hit chances
               int hit =  rand() % 2;
               if(hit == 1){
                  printf("Mob #%d attacked\n",i);
               }else{
                  printf("Mob #%d's attack missed\n",i);
               }
               //change state
               meshData[i][5] = 0;
            }
         }
      }
      //player moves to attack the mob
      if (x == mobX && z == mobZ && meshData[i][6] == 0){
         hit chance
         int hit =  rand() % 2;
         if(hit == 1 && meshData[i][5] == 0){
            printf("Mob #%d eliminated\n",i);
            unsetMeshID(i);
            //killed flag
            meshData[i][6] = 1;
            //mob can attack
            meshData[i][5] = 1;
         }else{
            printf("Mob #%d dodged\n",i);
            curX = (int)oldX;
            curY = (int)oldY;
            curY--;
            curZ = (int)oldZ;
            //mob can attack
            meshData[i][5] = 0;
         }
      }
   }

   //something is infront of the view point
   if (world[x][y][z] != 0) {
      if (world[x][y+1][z] == 0) {
         y += 2; //climb if the spot above the view point is empty
         curY = y*-1;
      }else{
         curX = (int)oldX;
         curY = (int)oldY;
         curY--;
         curZ = (int)oldZ;
         //set back to old locations
      }
   //out of bounds stay in bound
   }else if((x > WORLDX) || (x < 0)|| (z > WORLDZ) || (z < 0)) {
      curX = (int)oldX;
      curY = (int)oldY;
      curZ = (int)oldZ;
   }
   //set new positions
   setViewPosition(curX,curY,curZ);
}


	/******* draw2D() *******/
	/* draws 2D shapes on screen */
	/* use the following functions: 			*/
	/*	draw2Dline(int, int, int, int, int);		*/
	/*	draw2Dbox(int, int, int, int);			*/
	/*	draw2Dtriangle(int, int, int, int, int, int);	*/
	/*	set2Dcolour(float []); 				*/
	/* colour must be set before other functions are called	*/
void draw2D() {

   if (testWorld) {
		/* draw some sample 2d shapes */
      if (displayMap == 1) {
         GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
         set2Dcolour(green);
         draw2Dline(0, 0, 500, 500, 15);
         draw2Dtriangle(0, 0, 200, 200, 0, 200);

         GLfloat black[] = {0.0, 0.0, 0.0, 0.5};
         set2Dcolour(black);
         draw2Dbox(500, 380, 524, 388);
      }
   } else {
   	/* your code goes here */
      updateMap();
   }

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;
float x, y, z;

	/* sample animation for the testworld, don't remove this code */
	/* demo of animating mobs */
   if (testWorld) {

	/* update old position so it contains the correct value */
	/* -otherwise view position is only correct after a key is */
	/*  pressed and keyboard() executes. */

      getViewPosition(&x, &y, &z);
      setOldViewPosition(x,y,z);

	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;
	/* counter for user defined colour changes */
      static int colourCount = 0;
      static GLfloat offset = 0.0;

	/* offset counter for animated texture */
      static float textureOffset = 0.0;

	/* scaling values for fish mesh */
      static float fishScale = 1.0;
      static int scaleCount = 0;
      static GLfloat scaleOffset = 0.0;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;

	/* change user defined colour over time */
      if (colourCount == 1) offset += 0.05;
      else offset -= 0.01;
      if (offset >= 0.5) colourCount = 0;
      if (offset <= 0.0) colourCount = 1;
      setUserColour(9, 0.7, 0.3 + offset, 0.7, 1.0, 0.3, 0.15 + offset, 0.3, 1.0);

	/* sample tube creation  */
	/* draws a purple tube above the other sample objects */
       createTube(1, 45.0, 30.0, 45.0, 50.0, 30.0, 50.0, 6);

	/* move texture for lava effect */
      textureOffset -= 0.01;
      setTextureOffset(18, 0.0, textureOffset);

	/* make fish grow and shrink (scaling) */
      if (scaleCount == 1) scaleOffset += 0.01;
      else scaleOffset -= 0.01;
      if (scaleOffset >= 0.5) scaleCount = 0;
      if (scaleOffset <= 0.0) scaleCount = 1;
      setScaleMesh(1, 0.5 + scaleOffset);

	/* make cow with id == 2 appear and disappear */
	/* use scaleCount as switch to flip draw/hide */
	/* rotate cow while it is visible */
      if (scaleCount == 0) {
         drawMesh(2);
         setRotateMesh(2, 0.0, 180.0 + scaleOffset * 100.0, 0.0);
      } else {
         hideMesh(2);
      }

    /* end testworld animation */


   } else {
      //my colours
      setUserColour(9, 0.5, 0.5, 0.5, 1.0, 0.2, 0.2, 0.2, 1.0);
      setUserColour(10, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(11, 0.4, 0.7, 0.2, 1.0, 0.8, 0.15, 0.3, 1.0);
      setUserColour(12, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(13, 0.85, 0.85, 0.85, 1.0, 0.5, 0.5, 0.5, 1.0);
      setAssignedTexture(1, 27);
      setAssignedTexture(4, 40);
      setAssignedTexture(9, 40);
      setAssignedTexture(10, 38);      
      setAssignedTexture(13, 18);

	/* your code goes here */
      if(flycontrol == 0){
         gravity();
      }
      //flag ensures stairs exist
      if(stairFlag == 0){
         //set near the stairs
         setViewPosition((stairDownLocX+2)*-1,(stairDownLocY+3)*-1,stairDownLocZ*-1);
         stairFlag = 1;
      }
      //view point for indoor level
      if(flagOut == 1){
         setViewPosition((stairUpLocX+2)*-1,(stairUpLocY+1)*-1,stairUpLocZ*-1);
         flagOut = 3;
      }
      //move the clouds
      timer();
      meshObjs();

      float playerX,playerY,playerZ;
      getOldViewPosition(&playerX, &playerY, &playerZ);
      int x = abs(((int)playerX*-1));
      int z = abs(((int)playerZ*-1));

      turn = 0;//player turn
      int distFlag = 0;//distance flag

      //seeing if the player has moved to a new spot
      int a = (int)(playerX*-1 - playerPostionX);
      int b = (int)(playerZ*-1 - playerPostionZ);
      if(a != 0 || b != 0){
         playerPostionX = playerX*-1;
         playerPostionZ = playerZ*-1;
         for(int i = 0; i < 9; i++){
            //ai's turn
            meshData[i][5] = 1;
         }

         //for all mesh
         for (int i = 0; i < 9; i++){
            if (x != GPP[i][1] || z != GPP[i][2]){
               //Global player positioning
               GPP[i][1] = playerPostionX;
               GPP[i][2] = playerPostionZ;
               turn = 1;//ai turn
            }
            if(turn = 1){
               //cactus
               if(meshData[i][4] == 3 && meshData[i][6] == 0){
                  int mobX = (int)meshData[i][0];
                  int mobZ = (int)meshData[i][1];
               }
               count++;
               //look for fish
               if(meshData[i][4] == 1 && meshData[i][6] == 0 && meshData[i][8] == 1){
                  int xT, zT;
                  if(meshData[i][7] == 1){
                     //if the fish has been seen start chasing the player
                     xT = GPP[i][1];
                     zT = GPP[i][2];
                  }else{
                     //move around in the room
                     xT = (rand () % roomLenX[i]) + roomX[i];
                     zT = (rand () % roomLenZ[i]) + roomZ[i];
                  }
                  int meshX, meshZ; //mesh position
                  int newMeshX, newMeshZ;
                  int mX, mZ; //final mesh postion
                  meshX = (int)meshData[i][0];
                  meshZ = (int)meshData[i][1];
                  double g = 0.0, h = 0.0, f = 0.0;
                  double oldG = 1000.0, oldH = 1000.0, oldF = 1000.0;

                  for (int j = meshX-1; j <= meshX+1; j++){
                     for (int k = meshZ-1; k <= meshZ+1; k++){
                        if(world[j][26][k] == 0){
                           //calculation g, h, f for square around the mob position
                           g = sqrt((j*2) + (k*2));
                           h = abs(j-xT)+abs(k-zT);
                           distFlag = h;
                           f = g+h;
                           //go for the smalles f
                           if(oldF >= f){
                              oldF = f;
                              oldG = g;
                              oldG = h;
                              mX = j;
                              mZ = k;
                              distFlag = h;
                           }
                        }
                     }
                  }
                  //save new mob positions
                  meshData[i][0] = mX;
                  meshData[i][1] = mZ;
                  if(distFlag > 2){
                     //stopping the ai 2 units away from the player
                     //to allow the player to trigger combact
                     setTranslateMesh(i,(float)mX,26.0,(float)mZ);
                  }
               }
               //looking for bat
               if(meshData[i][4] == 2 && meshData[i][6] == 0 && meshData[i][8] == 1){
                  int xT, zT;
                  if(meshData[i][7] == 1){
                     //if the bat has been seen start chasing the player
                     xT = GPP[i][1];
                     zT = GPP[i][2];
                  }else{
                     //for mob 0-7 move to the room +1
                     //mob8 go to room 0
                     if(i != 8){
                        xT = (rand () % roomLenX[i+1]) + roomX[i+1];
                        zT = (rand () % roomLenZ[i+1]) + roomZ[i+1];
                     }else{
                        xT = (rand () % roomLenX[0]) + roomX[0];
                        zT = (rand () % roomLenZ[0]) + roomZ[0];
                     }
                  }
                  int meshX, meshZ; //mesh position
                  int newMeshX, newMeshZ;
                  int mX, mZ; //final mesh postion
                  meshX = (int)meshData[i][0];
                  meshZ = (int)meshData[i][1];
                  double g = 0.0, h = 0.0, f = 0.0;
                  double oldG = 1000.0, oldH = 1000.0, oldF = 1000.0;

                  for (int j = meshX-1; j <= meshX+1; j++){
                     for (int k = meshZ-1; k <= meshZ+1; k++){
                        if(world[j][26][k] == 0){
                           //calculation g, h, f for square around the mob position
                           g = sqrt((j*2) + (k*2));
                           h = abs(j-xT)+abs(k-zT);
                           distFlag = h;
                           f = g+h;
                           //go for the smalles f
                           if(oldF >= f){
                              oldF = f;
                              oldG = g;
                              oldG = h;
                              mX = j;
                              mZ = k;
                              distFlag = h;
                           }
                        }
                     }
                  }
                  //saving new mesh locations
                  meshData[i][0] = mX;
                  meshData[i][1] = mZ;
                  if(distFlag > 2){
                     //stopping the ai 2 units away from the player
                     //to allow the player to trigger combact
                     setTranslateMesh(i,(float)mX,25.0,(float)mZ);
                  }
               }
               turn = 0;
            }
         }
      }
   }
}


	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {

   if (button == GLUT_LEFT_BUTTON)
      printf("left button - ");
   else if (button == GLUT_MIDDLE_BUTTON)
      printf("middle button - ");
   else
      printf("right button - ");

   if (state == GLUT_UP)
      printf("up - ");
   else
      printf("down - ");

   printf("%d %d\n", x, y);
}



int main(int argc, char** argv){
   int seed =time(0);
   srand(seed);
   printf("seed: %d\n", seed);
   int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);


	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* create user defined colour and draw cube */
      setUserColour(9, 0.7, 0.3, 0.7, 1.0, 0.3, 0.15, 0.3, 1.0);
      world[54][25][50] = 9;


	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);

	/* texture examples */

	/* create textured cube */
	/* create user defined colour with an id number of 11 */
      setUserColour(11, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
	/* attach texture 22 to colour id 11 */
      setAssignedTexture(11, 22);
	/* place a cube in the world using colour id 11 which is texture 22 */
      world[59][25][50] = 11;

	/* create textured cube */
      setUserColour(12, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(12, 27);
      world[61][25][50] = 12;

	/* create textured cube */
      setUserColour(10, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(10, 26);
      world[63][25][50] = 10;

	/* create textured floor */
      setUserColour(13, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(13, 8);
      for (i=57; i<67; i++)
         for (j=45; j<55; j++)
            world[i][24][j] = 13;

	/* create textured wall */
      setUserColour(14, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(14, 18);
      for (i=57; i<67; i++)
         for (j=0; j<4; j++)
            world[i][24+j][45] = 14;

	/* create textured wall */
      setUserColour(15, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(15, 42);
      for (i=45; i<55; i++)
         for (j=0; j<4; j++)
            world[57][24+j][i] = 15;

		// two cubes using the same texture but one is offset
		// cube with offset texture 33
      setUserColour(16, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(16, 33);
      world[65][25][50] = 16;
      setTextureOffset(16, 0.5, 0.5);
		// cube with non-offset texture 33
      setUserColour(17, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(17, 33);
      world[66][25][50] = 17;

		// create some lava textures that will be animated
      setUserColour(18, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
      setAssignedTexture(18, 24);
      world[62][24][55] = 18;
      world[63][24][55] = 18;
      world[64][24][55] = 18;
      world[62][24][56] = 18;
      world[63][24][56] = 18;
      world[64][24][56] = 18;

		// draw cow mesh and rotate 45 degrees around the y axis
		// game id = 0, cow mesh id == 0
      setMeshID(0, 0, 48.0, 26.0, 50.0);
      setRotateMesh(0, 0.0, 45.0, 0.0);

		// draw fish mesh and scale to half size (0.5)
		// game id = 1, fish mesh id == 1
      setMeshID(1, 1, 51.0, 28.0, 50.0);
      setScaleMesh(1, 0.5);

		// draw cow mesh and rotate 45 degrees around the y axis
		// game id = 2, cow mesh id == 0
      setMeshID(2, 0, 59.0, 26.0, 47.0);

		// draw bat
		// game id = 3, bat mesh id == 2
      setMeshID(3, 2, 61.0, 26.0, 47.0);
      setScaleMesh(3, 0.5);
		// draw cactus
		// game id = 4, cactus mesh id == 3
      setMeshID(4, 3, 63.0, 26.0, 47.0);
      setScaleMesh(4, 0.5);


   } else {
      /* your code to build the world goes here */
      /* create user defined colour and draw cube */
      setUserColour(9, 0.5, 0.5, 0.5, 1.0, 0.2, 0.2, 0.2, 1.0);
      setUserColour(10, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(11, 0.4, 0.7, 0.2, 1.0, 0.8, 0.15, 0.3, 1.0);
      setUserColour(12, 0.75, 0.74, 0.74, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(13, 0.85, 0.85, 0.85, 1.0, 0.5, 0.5, 0.5, 1.0);
      setAssignedTexture(1, 27);
      setAssignedTexture(4, 40);
      setAssignedTexture(9, 40);
      setAssignedTexture(10, 38);      
      setAssignedTexture(13, 18);
      buildOutside();
   }


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}