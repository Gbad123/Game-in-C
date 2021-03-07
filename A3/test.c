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

extern int PointInFrustum( float x, float y, float z );

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

int roomX[9];
int roomZ[9];
int roomLenX[9];
int roomLenZ[9];
int doorLocX[9][4];
int doorLocZ[9][4];
int maxBloxs = 20;
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
int mapRange = 12;
int outside[100][50][100];
int inside[100][50][100];
int FOWmapWorld[100][10][100];
int levelFlag = 0;

float meshData[10][10]; //storing mesh data

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
   createMesh();
   levelFlag = 2;
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

void timer() {
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
         moveMesh();
         updateMap();
         //updatating the fow map
         float playerX,playerY,playerZ;
         getViewPosition(&playerX, &playerY, &playerZ);
         int x = abs(((int)playerX*-1) - mapRange);
         int z = abs(((int)playerZ*-1) - mapRange);
         for (int i = x; i < (x + (mapRange*2)); i++){
            for (int k = 24; k < 30; k++){
               for(int j = z; j < (z+(mapRange*2)); j++){
                  FOWmapWorld[i][k][j] = world[i][k][j];
               }
            }
         }
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
   //resetting fow
   memset(FOWmapWorld,0,sizeof(FOWmapWorld));
   levelFlag = 2;
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
   for(int x = 0; x < 10; x++){
      meshData[x][3] == 0;
   }
   //making sure the stairs are different colour
   world[stairDownLocX][stairDownLocY][stairDownLocZ] = 12;
   //reseting fow
   memset(FOWmapWorld,0,sizeof(FOWmapWorld));
   levelFlag = 1;
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
      stairFlag = 1;
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
      buildRoof(x,z,zlen,xlen,13);
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
   blankSlate();
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
   levelFlag = 1;
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

void createMesh(){
   int x, z;
   int i = 0;
   while (i < 9){
      //getting random mesh positions
      x = rand() % roomLenX[i];
      if(x==0){x++;}
      x += roomX[i];
      z = rand() % roomLenZ[i];
      if(z==0){z++;}
      z += roomZ[i];
      //making sure nothing exists on that position
      if(world[x][27][z] == 0 && world[x+1][27][z] == 0 && world[x-1][27][z] == 0){
         setMeshID(i, 2, x, 27.0, z);
         setScaleMesh(i, 0.2);
         meshData[i][0] = x;
         meshData[i][1] = z;
         //hide by default
         hideMesh(i);
         i++;
      }
   }
}

void moveMesh(){
   float x, z;
   float y = 27;
   for (int i = 0; i < 9; i++){
      x = meshData[i][0];
      z = meshData[i][1];
      //checking to make sure the mesh is visible
      if(meshData[i][3] == 0){
         //setMobPosition(i,x+0.2,y,z,0);
         x+=0.15;
         meshData[i][0] = x;
         setTranslateMesh(i, x, y,z);
      }
   }
}

void meshObjs(){
   float usrX, usrY, usrZ;
   int x, z;
   //get user position
   for (int i = 0; i < 9; i++){
      getViewPosition(&usrX, &usrY, &usrZ);
      x = meshData[i][0];
      z = meshData[i][1];
      usrX = abs(usrX);
      usrZ = abs(usrZ);
      //if the mesh is out of range hide it else show it
      if(abs(usrX - x) <= 15 && abs(usrZ - z) <= 15 && PointInFrustum(x,27,z)){
         if(meshData[i][3] == 1 && levelFlag == 2){
            drawMesh(i);
            printf("Mesh #%d is visible\n",i);
            meshData[i][3] = 0;
         }
      }

      if(abs(usrX - x) > 15 || abs(usrZ - z) > 15 || !PointInFrustum(x,27,z)){
         //hidden flag
         if(meshData[i][3] == 0){
            hideMesh(i);
            printf("Mesh #%d is hidden\n",i);
            meshData[i][3] = 1;
         }
      }
   }
}

void updateMap(){
   GLfloat blue[] = {0.0, 0.0, 0.5, 0.75};
   GLfloat red[] = {0.5, 0.0, 0.0, 0.75};
   GLfloat green[] = {0.0, 0.5, 0.0, 0.5};
   GLfloat white[]  = {1.0, 1.0, 1.0, 1.75};
   GLfloat yellow[] = {0.5, 0.5, 0.0, 0.75};
   GLfloat black[] = {0.0, 0.0, 0.0, 0.5};

   int x1,y1,x2,y2;
   int size = 3;
   int loc = 310;

   if(displayMap == 1){
      for(int i=0; i<WORLDX; i++){
         for(int k=0; k<WORLDZ; k++){
            // draw player
            float x,y,z;
            set2Dcolour(yellow);
            getViewPosition(&x, &y, &z);
            x1 = screenWidth-loc+((int)z*-size);
            y1 = screenHeight-loc+((int)x*-size);
            x2 = screenWidth-loc+((int)z*-size)+5;
            y2 = screenHeight-loc+((int)x*-size)+5;
            draw2Dbox(x1,y1,x2,y2);

            //printf("%d\n",stairFlag);
            //stairs for outdoor level
            if(levelFlag == 1){
               set2Dcolour(white);
               x1 = screenWidth-loc+(stairDownLocZ*size);
               y1 = screenHeight-loc+(stairDownLocX*size);
               x2 = screenWidth-loc+(stairDownLocZ*size)+5;
               y2 = screenHeight-loc+(stairDownLocX*size)+5;
               draw2Dbox(x1,y1,x2,y2);
            }
            if(levelFlag == 2){
               //stair for down stairs / mesh
               set2Dcolour(white);
               x1 = screenWidth-loc+(stairUpLocZ*size);
               y1 = screenHeight-loc+(stairUpLocX*size);
               x2 = screenWidth-loc+(stairUpLocZ*size)+5;
               y2 = screenHeight-loc+(stairUpLocX*size)+5;
               draw2Dbox(x1,y1,x2,y2);
                for(int j = 0; j < 10; j++){
                  set2Dcolour(black);
                  x1 = screenWidth-loc+((int)meshData[j][1]*size);
                  y1 = screenHeight-loc+((int)meshData[j][0]*size);
                  x2 = screenWidth-loc+((int)meshData[j][1]*size)+5;
                  y2 = screenHeight-loc+((int)meshData[j][0]*size)+5;
                  draw2Dbox(x1,y1,x2,y2);
               } 
            }

            //draw the background and random blocs
            if(world[i][25][k] == 0){
               set2Dcolour(green);
            }else if(world[i][25][k] == 3){
               set2Dcolour(red);
            }
            if(world[i][25][k] == 12){
               set2Dcolour(blue);
            }

            //draw the box
            x1 = screenWidth-loc+(k*size);
            y1 = screenHeight-loc+(i*size);
            x2 = screenWidth-loc+(k*size)+5;
            y2 = screenHeight-loc+(i*size)+5;
            draw2Dbox(x1,y1,x2,y2);
         }
      }
   }else if(displayMap == 2){
      for(int i=1; i<WORLDX-1; i++){
         for(int k=1; k<WORLDZ-1; k++){
            // draw player
            float x,y,z;
            set2Dcolour(yellow);
            getViewPosition(&x, &y, &z);
            x1 = screenWidth-loc+((int)z*-size);
            y1 = screenHeight-loc+((int)x*-size);
            x2 = screenWidth-loc+((int)z*-size)+5;
            y2 = screenHeight-loc+((int)x*-size)+5;
            draw2Dbox(x1,y1,x2,y2);

            //stairs for outdoor level
            if(levelFlag == 1){
               set2Dcolour(white);
               x1 = screenWidth-loc+(stairDownLocZ*size);
               y1 = screenHeight-loc+(stairDownLocX*size);
               x2 = screenWidth-loc+(stairDownLocZ*size)+5;
               y2 = screenHeight-loc+(stairDownLocX*size)+5;
               draw2Dbox(x1,y1,x2,y2);
            }
            if(levelFlag == 2){
               //stair for down stairs
               set2Dcolour(white);
               x1 = screenWidth-loc+(stairUpLocZ*size);
               y1 = screenHeight-loc+(stairUpLocX*size);
               x2 = screenWidth-loc+(stairUpLocZ*size)+5;
               y2 = screenHeight-loc+(stairUpLocX*size)+5;
               draw2Dbox(x1,y1,x2,y2);
               for(int j = 0; j < 10; j++){
                  if(meshData[j][3] == 0){
                     set2Dcolour(black);
                     x1 = screenWidth-loc+((int)meshData[j][1]*size);
                     y1 = screenHeight-loc+((int)meshData[j][0]*size);
                     x2 = screenWidth-loc+((int)meshData[j][1]*size)+5;
                     y2 = screenHeight-loc+((int)meshData[j][0]*size)+5;
                     draw2Dbox(x1,y1,x2,y2);
                  }
               } 
            }

            //draw the background and random blocs
            if(FOWmapWorld[i][25][k] == 0){
               set2Dcolour(green);
            }else if(FOWmapWorld[i][25][k] == 3){
               set2Dcolour(red);
            }
            if(FOWmapWorld[i][25][k] == 12){
               set2Dcolour(blue);
            }

            //draw the box
            x1 = screenWidth-loc+(k*size);
            y1 = screenHeight-loc+(i*size);
            x2 = screenWidth-loc+(k*size)+5;
            y2 = screenHeight-loc+(i*size)+5;
            draw2Dbox(x1,y1,x2,y2);
         }
      }
   }
}