
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

   /* my functions */

void gravity();

   //indoors
void buildIndoors();
void blankSlate();
void buildRooms();
void buildWallX(int x, int z, int zlen, int xlen, int colour);
void buildWallZ(int x, int z, int zlen, int xlen, int colour);
void buildRoof(int x, int z, int zlen, int xlen, int colour);
void createRoom();
void doorT(int top, int i, int *doorX, int *doorZ);
void doorR(int right, int i, int *doorX, int *doorZ);
void doorB(int bottom, int i, int *doorX, int *doorZ);
void doorL(int left, int i, int *doorX, int *doorZ);
void findDoorLocation();
void createDoors(int x, int y, int z, int colour);
void buildHalls();
void buildHallX(int x, int z, int xlen, int colour);	
void buildHallZ(int x, int z, int zlen, int colour);
void buildRight(int i);
void buildTop(int i);
void buildHalls();
void randomBloxs();
void createFloor();
void stairsUp();
void rebuildIndoors();

   //outdoors
void buildOutside();
void generateClouds();
void moveClouds();
void cloudTimer();
void buildMountain();
void stairsDown();
void rebuildIndoors();

   /* my variables */
int roomX[9];
int roomZ[9];
int roomLenX[9];
int roomLenZ[9];
int doorLocX[9][4];
int doorLocZ[9][4];
int maxBloxs = 30;
int minBloxs = 10;
int bloxLoc[40][2];
int stairDownLocX;
int stairDownLocZ;
int stairDownLocY;
int stairUpLocX;
int stairUpLocY;
int stairUpLocZ;
int flagOut = 0;
int stairFlag = 0;
int outside[100][50][100];
int inside[100][50][100];

/********* end of my variable declarations **************/

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
   //something is infront of the view point
   if (world[x][y][z] != 0) {
      if (world[x][y+1][z] == 0) {
         y += 2; //climb if the spot above the view point is empty
         curY = y*-1;
      } else {
         curX = (int)oldX;
         curY = (int)oldY;
         curZ = (int)oldZ;
         //set back to old locations
      }
   //out of bounds stay in bound
   } else if ((x > WORLDX) || (x < 0)|| (z > WORLDZ) || (z < 0)) {
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
      cloudTimer();
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
      /* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;

   	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }

      /* create user defined colour and draw cube */
      setUserColour(9, 0.5, 0.5, 0.5, 1.0, 0.2, 0.2, 0.2, 1.0);
      setUserColour(10, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(11, 0.4, 0.7, 0.2, 1.0, 0.8, 0.15, 0.3, 1.0);
      setUserColour(12, 0.75, 0.74, 0.74, 1.0, 0.5, 0.5, 0.5, 1.0);
      setUserColour(13, 0.85, 0.85, 0.85, 1.0, 0.5, 0.5, 0.5, 1.0);
      /* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         for (int j = 0; j < 2; j++){
            world[i][25+j][0] = 7;
            world[i][25+j][WORLDZ-1] = 7;
         }
      }
      for(i=0; i<WORLDZ-1; i++) {
         for (int j = 0; j < 2; j++){
            world[0][25+j][i] = 7;
            world[WORLDX-1][25+j][i] = 7;
         }
      }
      buildOutside();
   }


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

void blankSlate(){
   //resets the world to be empty and build the board
   int i,j,k;
   for(i=0; i<WORLDX; i++)
      for(j=0; j<WORLDY; j++)
         for(k=0; k<WORLDZ; k++)
            world[i][j][k] = 0;

   /* build a red platform */
   for(i=0; i<WORLDX; i++) {
      for(j=0; j<WORLDZ; j++) {
         world[i][24][j] = 3;
      }
   }
   /* blue box shows xy bounds of the world */
   for(i=0; i<WORLDX-1; i++) {
      for (int j = 0; j < 2; j++){
         world[i][25+j][0] = 7;
         world[i][25+j][WORLDZ-1] = 7;
      }
   }
   for(i=0; i<WORLDZ-1; i++) {
      for (int j = 0; j < 2; j++){
         world[0][25+j][i] = 7;
         world[WORLDX-1][25+j][i] = 7;
      }
   }
}

void buildIndoors(){
   blankSlate();
   buildRooms();
   createRoom();
   findDoorLocation();
   buildHalls();
   randomBloxs();
   createFloor();
   stairsUp();

   //storing the world data
   int x,y,z;
   for(x=0; x<WORLDX; x++) {
      for(y=0; y<WORLDY; y++) {
         for(z=0; z<WORLDZ; z++){
            inside[x][y][z] = world[x][y][z];
         }
      }
   }   
}

void cloudTimer() {
   static clock_t start; 
   clock_t end;
   static int flag = 1;
   int diff;
   //get the start time
   if (flag == 1){
      start = clock();
      flag = 0;
   }else{
      //get the end time
      end = clock();
      //setting speed speed
      diff = (int)((end-start)/5000);
      if(diff >=15){
         flag = 1;
         //Update cloud location
         moveClouds();
      }
   }
}

void moveClouds(){
   int x,y,z;
   for(y = 45; y < WORLDY; y++){
      for (z = WORLDZ - 1; z >= 0; z--) {
            for (x = WORLDX - 1; x >= 0; x--) {
            //get the original cloud location
            if(world[x][y][z]==5){
               //delete the original cloud
               world[x][y][z] = 0;
               if(x >= WORLDX-1){
                  world[10][y][z] = 5;
               }else{
                  //recreate the cloud
                  world[x+1][y][z] = 5;
               }
            }
         }
      }
   }
}

void rebuildIndoors(){
   //recall data from the inside world array
   int i,j,k;
   for(i=0; i<WORLDX; i++) {
      for(j=0; j<WORLDY; j++) {
         for(k=0; k<WORLDZ; k++){
            world[i][j][k] = inside[i][j][k];
         }
      }
   }
}

void rebuildOutdoors(){
   //recall data from ourside world array
   int i,j,k;
   for(i=0; i<WORLDX; i++) {
      for(j=0; j<WORLDY; j++) {
         for(k=0; k<WORLDZ; k++){
            world[i][j][k] = outside[i][j][k];
         }
      }
   }
   //making sure the stairs are different colour
   world[stairDownLocX][stairDownLocY][stairDownLocZ] = 12;
}

void gravity() {
   //varuable delcartions
   float x, y, z;
   float oldX, oldY, oldZ;
   float speed = 0.8;
   int objX, objY, objZ;
   int i,j,k;
   //getting current positions
   getViewPosition(&x, &y, &z);
   objX = (int)(x) * -1;
   objY = (int)(y) * -1;
   objZ = (int)(z) * -1;
   //if above floor level and there is nothing below you, gravity
   if (objY > 25 && world[objX][objY-1][objZ] == 0){
      setViewPosition(x, y + 0.1, z);   
   }
   if(objX == stairDownLocX && objY-1 == stairDownLocY && objZ == stairDownLocZ){
      //first time creating indoor level
      if(flagOut == 0){
         buildIndoors();
      }
      //rebuild indoors
      if (flagOut == 3){
         rebuildIndoors();
      }
      flagOut = 1;
   }
   if(objX == stairUpLocX && objY-1 == stairUpLocY && objZ == stairUpLocZ){
      //rebuild outside
      rebuildOutdoors();
      flagOut = 3;
      stairFlag = 0;
   }
}

void createFloor(){
   //creating the world board
   int x,z;
   for (x = 0; x < WORLDX; x++) {
      for (z = 0; z < WORLDZ; z++) {
         if(x % 2 == 0){
            world[x][24][z] = 9;
         }else{
            world[x][24][z] = 4;
         }
      }
   }
   return;
}

void randomBloxs(){
   int blocksNum = 0;
   int numR =  rand() % maxBloxs + 1;
   if (numR < minBloxs){
      numR+=minBloxs;
   }
   int room;
   while (numR > 0){
      room = rand() % 9;
      int x; //= roomX[room];
      int z; //= roomZ[room];
      
      x = rand() % roomLenX[room];
      x += roomX[room];
      z = rand() % roomLenZ[room];
      z += roomZ[room];

      //placing random bloxs
      if(world[x][26][z] == 0){
         world[x][25][z] = 3;
         numR = numR - 1;
         //storing the x and z location for blocks
         bloxLoc[blocksNum][0] = x;
         bloxLoc[blocksNum][1] = z;
         blocksNum++;
      }
      
   }
}

void buildRooms(){
   int xMax = WORLDX;
   int zMax = WORLDZ;
   int x, z, xlen, zlen;
   int count = 0;
   //dividing the grid leaveing space for corridors
   xMax = (xMax - 8) / 3;
   zMax = (zMax - 8) / 3;
   //creating one square in each section of the grid
   for(int i = 0; i <= 2; i++){
      for(int j = 0; j <= 2; j++){
         xlen = xMax;
         zlen = zMax;
         if (i == 0){
            x = rand() % (xMax-11);
            z = rand() % (zMax-11);
            x = x + (xMax * i);
            z = z + (zMax * j);
         }
          if(i == 1){
            x = rand() % (xMax-11);
            z = rand() % (zMax-11);
            x = x + (xMax * i + 3);
            z = z + (zMax * j + 3);
            xlen = xlen * 2; //moving rooms to the second row
         }
         if(i == 2){
            x = rand() % (xMax-11);
            z = rand() % (zMax-11);
            x = x + (xMax * i + 6);
            z = z + (zMax * j + 6);
            xlen = xlen * 3; //moving rooms to the third row
         }
         if (j == 1){
            z+=3;
            zlen = zlen * 2; //moving rooms to the second column
         }else if (j == 2){
            z+=6;
            zlen = zlen * 3; //moving rooms to the third column
         }
         //stroing the first point x and z value, and how the max amount of space they have
         roomX[count] = x;
         roomZ[count] = z;
         roomLenX[count] = xlen;
         roomLenZ[count] = zlen;
         count++;
      }
   }
   return;
}

void createRoom(){
   for(int count = 0; count < 9; count++){
      
      //using the max lengths to find the actual x and z lengths of the rooms
      int x = roomX[count];
      int z = roomZ[count];
      int xlen = roomLenX[count];
      int zlen = roomLenZ[count];

      xlen = rand() % (xlen - x) + x;
      zlen = rand() % (zlen - z) + z;
      xlen = xlen - x;
      zlen = zlen - z;

      //ensuring the rooms have a max and min size to stop overflow and rooms being too small
      if (xlen < 8) {
         xlen = 8;
      }
      if (xlen > 25) {
         xlen = 25;
      }
      if (zlen < 8){
         zlen = 8;
      }
      if( zlen > 25) {
         zlen = 25;
      }
      //updating the room lengths
      roomLenX[count] = xlen;
      roomLenZ[count] = zlen;

      //building the x walls
      //use x to find location on map
      //use xlen to get a random point on map
      buildWallX(x,z,zlen, xlen,13);
      
      //building the z walls
      buildWallZ(x,z,zlen, xlen,13);

      //building the roofs      
      buildRoof(x,z,zlen,xlen,5);
   }
   return;
}

//building the walls using the lengths of x
void buildWallX(int x, int z, int zlen, int xlen, int colour){
   for (int i = 0; i <= xlen; i++){
      for (int j = 0; j < 5; j++){
         world[x+i][25+j][z] = colour;
         world[x+i][25+j][z+zlen] = colour;
      }
   }
}

//building the walls using the depth of z
void buildWallZ(int x, int z, int zlen, int xlen, int colour){
   for (int i = 0; i <= zlen; i++){
      for (int j = 0; j < 5; j++){
         world[x][25+j][z+i] = colour;
         world[x+xlen][25+j][z+i] = colour;
      }
   }
}

//building the roof using the lengths of x and depth of z
void buildRoof(int x, int z, int zlen, int xlen, int colour){
   for (int i = 0; i <= xlen; i++){
      for (int j = 0; j <= zlen; j++){
         world[x+i][30][z+j] = colour;
      }
   }
}

void findDoorLocation(){
   int bottom, top, right, left, temp;
   for (int i = 0; i < 9; i++){
      //direction ints find the location for the door along its side
      top = roomLenX[i];
      bottom = roomLenX[i];
      right = roomLenZ[i];
      left = roomLenZ[i];
      
      top = rand() % (top-2);
      bottom = rand() % (bottom-2);
      right = rand() % (right-2);
      left = rand() % (left-2);

      switch(i){
         //each case corresponds with the square in the part of the grid, 0 = bottom left
         //the function save the door location in the area provided
         //this data is used to connect the doors and make halls
         case 0:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorR(right, i, &doorLocX[i][1],&doorLocZ[i][1]);
            break;
         case 1:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorR(right,i, &doorLocX[i][1],&doorLocZ[i][1]);
            break;
         case 2:
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorR(right,i, &doorLocX[i][1],&doorLocZ[i][1]);
            break;
         case 3:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorR(right,i, &doorLocX[i][1],&doorLocZ[i][1]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
         case 4:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorR(right,i, &doorLocX[i][1],&doorLocZ[i][1]);
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
         case 5:
            doorR(right,i, &doorLocX[i][1],&doorLocZ[i][1]);
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
         case 6:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
         case 7:
            doorT(top, i, &doorLocX[i][0],&doorLocZ[i][0]);
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
         case 8:
            doorB(bottom,i, &doorLocX[i][2],&doorLocZ[i][2]);
            doorL(left,i, &doorLocX[i][3],&doorLocZ[i][3]);
            break;
      } 
   }
   return;
}

//creating doors along each side of the wall
void doorT(int top, int i, int *doorX, int *doorZ){
   //build door on the top side of the room
   int temp = roomLenZ[i] + roomZ[i];
   int t=top+roomX[i]+1;
   createDoors(t,25,temp,0);
   *doorX = t;
   *doorZ = temp;
}

void doorR(int right, int i, int *doorX, int *doorZ){
   //build door on the right side of the room
   int temp = roomX[i] + roomLenX[i];
   int r=right+roomZ[i]+1;
   createDoors(temp,25,r,0);
   *doorX = temp;
   *doorZ = r;
}

void doorB(int bottom, int i, int *doorX, int *doorZ){
   //build door on the bottom side of the room
   int temp = roomZ[i];
   int b=bottom+roomX[i]+1;
   createDoors(b,25,temp,0);
   *doorX = b;
   *doorZ = temp;
}

void doorL(int left, int i, int *doorX, int *doorZ){
   //build door on the left side of the room
   int temp = roomX[i];
   int l=left+roomZ[i]+1;
   createDoors(temp,25,l,0);
   *doorX = temp;
   *doorZ = l;
}

void createDoors(int x, int y, int z, int colour){
   //removing the bloacks from the door location
   for (int i = 0; i < 5; i++){
      world[x][y+i][z] = colour;
   }
}

void buildHallX(int x, int z, int xlen, int colour){
   //bulding halls in the x direction going up till y=30
   for (int i = 0; i <= xlen; i++){
      for (int j = 0; j < 5; j++){
         if(world[x+i][25+j][z] == 0){
            world[x+i][25+j][z] = colour;
         }
      }
   }
}

void buildHallZ(int x, int z, int zlen, int colour){
   //bulding halls in the Z direction going up till y=30
   for (int i = 0; i <= zlen; i++){
      for (int j = 0; j < 5; j++){
         if(world[x][25+j][z+i] == 0){
            world[x][25+j][z+i] = colour;         
         }
      }
   }
}

void buildRight(int i){
   //variable delcariton and getting start/end coordinates
   int xStart, xEnd, zStart, zEnd, xlen, zlen;
   xStart = doorLocX[i][1];
   zStart = doorLocZ[i][1];
   xEnd = doorLocX[i+3][3];
   zEnd = doorLocZ[i+3][3];
   zlen = zEnd-zStart;
   zStart = zStart-1;
   xlen = (xEnd-xStart)/2;
   int colour = 10;
   //determines if the elbow is going up or down _|¯ or ¯\_
   if(zlen > 0){
      buildHallX(xStart,zStart,xlen,colour); //build halls from starting room to near the midpoint top of the door
      buildHallX(xStart+xlen,zEnd-1,xlen,colour); //build halls from ending room to near the midpoint top of the door
      buildHallZ(xStart+xlen,zStart,zlen-1,colour); //connects the 2 halls
            
      buildHallX(xStart,zStart+2,xlen-2,colour); //build halls from starting room to near the midpoint below door
      buildHallX(xStart+xlen-2,zEnd+1,xlen+2,colour); //build halls from ending room to near the midpoint below the door */
      buildHallZ(xStart+xlen-2,zStart+2,zlen-1,colour); //connects the 2 halls
   } else{
      zlen = abs(zlen); //converts to postive to get the length, now that we know the direction
      buildHallX(xStart,zStart,xlen,colour);
      buildHallX(xStart+xlen,zEnd-1,xlen,colour);
      buildHallZ(xStart+xlen,zEnd-1,zlen-1,colour);

      buildHallX(xStart,zStart+2,xlen+2,colour);
      buildHallX(xStart+xlen+2,zEnd+1,xlen-2,colour);
      buildHallZ(xStart+xlen+2,zEnd+1,zlen-1,colour);
   }
   return;
}

void buildTop(int i){
   //variable delcariton and getting start/end coordinates
   int xStart, xEnd, zStart, zEnd, xlen, zlen;
   xStart = doorLocX[i][0];
   zStart = doorLocZ[i][0];
   xEnd = doorLocX[i+1][2];
   zEnd = doorLocZ[i+1][2];
   xStart = xStart-1;
   zlen = (zEnd-zStart)/2;
   xlen = xEnd-xStart;
   int colour = 10;
   //determines if the elbow is going right or left
   if(xlen > 0){
      world[xEnd-1][25][zEnd] = 8;
      buildHallZ(xStart,zStart,zlen,colour); //build halls from starting room to near the midpoint left of the door
      buildHallZ(xEnd-1,zStart+zlen,zlen,colour); //build halls from ending room to near the midpoint left of  the door
      buildHallX(xStart,zStart+zlen,xlen-1,colour); //connects the 2 halls

      buildHallZ(xStart+2,zStart,zlen-2,colour); //build halls from starting room to near the midpoint right of the door
      buildHallZ(xEnd+1,zStart+zlen-2,zlen+2,colour); //build halls from ending room to near the midpoint right of the door
      buildHallX(xStart+2,zStart+zlen-2,xlen-1,colour); //connects the 2 halls */
   }else{
      xlen = abs(xlen); //converts to postive to get the length, now that we know the direction
      buildHallZ(xStart,zStart,zlen,colour);
      buildHallZ(xEnd-1,zStart+zlen,zlen,colour);
      buildHallX(xEnd-1,zStart+zlen,xlen,colour);

      buildHallZ(xStart+2,zStart,zlen+2,colour);
      buildHallZ(xEnd+1,zStart+zlen+2,zlen-2,colour);
      buildHallX(xEnd+1,zStart+zlen+2,xlen,colour);
   }
}

void buildHalls(){
   int xStart, xEnd, zStart, zEnd, xlen, zlen; //variable delcartion
   //loops through all rooms and connects to the ones on either top/right or both
   for (int i = 0; i < 9; i++){
      switch(i){
         case 0: 
            buildRight(i); //connecting room 0 -> 3
            buildTop(i); //connecting room 0 -> 1
            break;
         case 1:
            buildRight(i); //connecting room 1 -> 4
            buildTop(i); //connecting room 1 -> 2
            break;
         case 2:
            buildRight(i); //connecting room 2 -> 5
            break;
         case 3:
            buildRight(i); //connecting room 3 -> 6
            buildTop(i); //connecting room 3 -> 4
            break;
         case 4:
            buildRight(i); //connecting room 4 -> 7
            buildTop(i); //connecting room 4 -> 5
            break;
         case 5:
            buildRight(i); //connecting room 5 -> 8
            break;
         case 6:
            buildTop(i); //connecting room 6 -> 7
            break;
         case 7:
            buildTop(i); //connecting room 7 -> 8
            break;
      }
   }
}

void stairsUp(){
   int x,z;
   int temp = 0;
   while(temp == 0){
      int room = rand() % 9;
      x = rand() % roomLenX[room];
      x += roomX[room];
      z = rand() % roomLenZ[room];
      z += roomZ[room];
      if(world[x][25][z] == 0 && world[x+1][25][z] == 0 && world[x+2][25][z] == 0){
         world[x][25][z] = 5;
         stairUpLocX = x;
         stairUpLocY = 25;
         stairUpLocZ = z;
         temp = 1;
      }
   }
}

void buildOutside(){
   int x,y,z;
   generateClouds();
   buildMountain();
   stairsDown();
   for(x=0; x<WORLDX; x++) {
        for(y=0; y<WORLDY; y++) {
         for(z=0; z<WORLDZ; z++){
            outside[x][y][z] = world[x][y][z];
         }
      }
   }
}

void generateClouds(){
   for (int i = 0; i < 50; i++){
      int size = rand() % 2;
      size++;   
      int x = rand() % 80;
      x += 10;
      int z = rand() % 80;
      z += 10;
      int y = rand() % 5;
      y += 45;
      world[x][y][z] = 5;
      if(size == 1){
         world[x+1][y][z] = 5;
         world[x][y][z+1] = 5;
         world[x+1][y][z+1] = 5;
      }
      if(size == 2){
         for (int j = 0; j <= y; j++){
            world[x+1][y][z] = 5;
            world[x][y][z+1] = 5;
            world[x+1][y][z+1] = 5;
            world[x+2][y][z] = 5;
            world[x][y][z+2] = 5;
         }
      }
   }
}

void buildMountain(){
   int x, y, z;
   int colour = 11;
   //run though the world along x and z axis
   for(x = WORLDX - 1; x >= 0; x--){
      for(z = WORLDZ - 1; z >= 0; z--){
         //calling perlin
         y = perlin2d(x,z,0.1,4)*15+26;
         //from 26 to up along y 
         for(int i = y; i > 26; i--){
            //create snowy top
            if(y >= 36){
               colour = 5;
            }else{
               colour = 11;
            }
            //build world
            world[x][y][z] = colour;
            if(world[x][y-1][z] == 0){
               world[x][y-1][z] = colour;
            }
         }
      }
   }
}

void stairsDown(){
   int temp = 0;
   int x, y, z;
   //getting x,y,z location for stair
   while(temp == 0){
      x = rand() % 80;
      x += 10;
      z = rand() % 80;
      z += 10;
      y = rand() % 5;
      y += 30;
      //checking the stairs location is valod
      if(world[x][y][z] == 0 && world[x][y+1][z] == 0 && world[x][y-1][z] != 0){
         world[x][y][z] = 12;
         //sotring location
         stairDownLocX = x;
         stairDownLocY = y;
         stairDownLocZ = z;
         //updating flag to go down
         temp = 1;
      }
   }
   stairFlag = 0;
}