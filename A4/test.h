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
void timer();
void buildMountain();
void stairsDown();
void rebuildIndoors();

//A3
void createMesh();
void moveMesh();
void meshObjs();

//map
void updateMap();


