Ebad Babar
0954164
Cis 4820

To run the program:
  export DISPLAY=localhost:0
  make
  ./A1

/********* Assignment 1 **************/

Algorithum:
+---+---+---+
| 2 | 5 | 8 |
+---+---+---+
| 1 | 4 | 7 |  Z
+---+---+---+
| 0 | 3 | 6 |
+---+---+---+
      X

x = refers to the width
z = refers to the depth
y = refers to the height

Main:
    Create a yellow border around the world
    Set colour 10, light shade of gery similar to the one provided in the other Read me
  Build Rooms:
    Divides the world grid by 3 in both x and z to get zone 0
    Using xlen and zlen navigate to each sector to place one block in each sector
    The location for these blocks are stored in a global x and z array (corresponding w the room number)
  createRooms:
    Using the coordinates and max lengths calls buildWallX and buildWallZ
    buildWallX:
        double for loop making the walls along the x axis 5 units height
    buildWallZ:
        double for loop making the walls along the z axis 5 units height
    buildRoof:
        build the roof for the room
  findDoorLocation:
    loops through all the Rooms
    selects a random location along each wall
    Run through each room and decide how many doors it has
      Room 0 connects to 3 and 1 therefore build door along the Top and right
      doorT, doorR, doorB, doorL:
        uses the data it was provided to make a hole in the wall where the door should be
        made 5 in height to ensure its visible
  buildHalls:
    loops through until all rooms are connected
    following the same logic as door, calling top and right to see where halls need to be build
      Room zero connects to 3 on the right and 1 to the top
      to reduce compliction halls are only build along the top and right
        Meaning halls go 0 -> 3, 3 -> 6 and 0 -> 1, 1 -> 2
        not 3 -> 0, or 1 -> 0
    buildRight:
      Calculates the mid point and determins which direction the bend is
      Does the following twice: once above the door locaiton and once below
        calls buildHallX to make a hall from 0 towards 3
        calls buildHallX to make a halls from 3 towards 0
        calls buildHallZ to connect the 2 halls
    buildTop:
      Calculates the mid point and determins which direction the bend is
      Does the following twice: once to the right of the door locaiton and once to the left
        calls buildHallZ to make a hall from 0 towards 1
        calls buildHallZ to make a halls from 1 towards 0
        calls buildHallX to connect the 2 halls
    buildHallZ,buildHallX:
      2 for loops build the halls depending on the info it was given
      goes up 5 units to stay at the same level as the rooms
  randomBloxs:
    uses the max and min blocks globals to see how many blocks need to be placed
    randomly selects one of the Rooms
    selects a random location along the x axis and z axis to get the coordinates to place the bloc at
  createFloor:
    set colour at 9, uses the example one provided in the ReadMe
    uses colour 9 and 4 to create a prision like feel for the floor

Gravity:
  Called if fly mode is off
  Check current user location and see's if the spot below contains a bloc
  if not lower the user
  stop once reach the bloc or 25 (floor level)

collisionResponse:
  Uses getOldViewPosition and getViewPosition
  If current view position has a block
    Climb it if its only one unit high
    Set new position to old position
  Also ensure the player doesn't go out of bounce is x and z axis


/********* Assignment 2 **************/

Clouds
  Generate
    There can be up to 50 clouds
    There are 2 styles of cloud
    First is a flat cube
    Second has depth in the y direction
    They are randomly placed along the sky in a between y 45 and y 50
  Moving cloud 
    Run 2 loops around the x,z axis and 1 for the y range where clouds can be
    If a cloud bloc is found delete it 
      Then recreate it in x+1
  Cloud Timer
    Gets start and end clock Timer
    Set speed

Terrain
  https://gist.github.com/nowl/828013
  Perlin function sourse
  Get the y values from Perlin
  Build the mountain
  If height is above 36 -> snowy tops

Clouds are well below the terrain max height to ensure no collisions

States
  Indoor 3D world array stores data for indoor level
  Outdoor 3D world array stores data for Outdoor level

Stairs
  Outdoor
    User spawn point is +2y and +1x from the Stairs
    If you're right beside the bloc and snow it might be difficult to see
    If you move away the difference becomes more apparent
  Indoor
    The user is spawned +2x from the stairs going up

Limitations:
  States can not be switched if fly mode is on
  The halls don't have roof to allow you to go outside

/********* Assignment 3 **************/

Timer function
  Gets inital user computer Timer
  Every time the timer is called
    Calculate how much time has passed
    If interval == 1.5ms call movement
      Clouds
      MeshMove
      UpdateMap

Textures
  Added grass textures
  Half the floor in the indoor level
  Walls, and halls Textures

  Didn't add too many to prevent performance issues

Animation
  Mesh data is stored in a 2D array
  One mesh object per room
  Moves along the x asix at the same speed as the clouds
  If a mesh is with a radius of the player it is visible
  Only moves when visible

Map
  Yellow - player
  Red - blocks
  Black - enemies
  White - stairs

  Normal - White border
    Uses world array and floor flag to populate the Map
    run 2 for loops in x and z axis
    Place player
    Place stairs
    If world at x and z != 0 place it on the Map
  FOW
    Same logic as the Normal Map
    But instead of world array it uses its dedicated array
    The array only contains world data of places the user has been
    The FOWarray is updated in the timer function w user location and range
    The array is reset when the player moves between floors


Assumption
  Prof stated in class a Mesh can pop into view when in hall walking towards a room
  MESH will be visible before the user enters a room
  Visibility range takes room size into account

  Ignore mesh collision

Limitations
  Stairs for both level show up, regardless on which level


/********* Assignment 4 **************/

Ai
  9 mesh items are drawn each can be either
  cactus, fish, or bat

cactus -> Light Purple
  Stationary, waits player to enter range then attacks
  turn counter enscures it can only attack if either the player has moved or attacked it

fish -> Pink
  Moving around the room
  If the player sees it or within a range to activate, the ai starts chaseing the player

bat -> Hot Pink
  Moving around the maze
  If the player sees it, the ai starts chasing the player

Algorithum
  Modified A*
  The ai calculates the g,h,f vectors using current location and last saved player location
  Moves the square with the smallest f value
  If multiple squares have the same f value, move to the smallest h
  Else stay where you are
  The Ai stops moving if its 2 units away from the player, to allow the player to trigger combact

Limitations
  As best move is being calculated each new spot
  Sometimes the ai can get stuck as the best is move back and forth between 2 spots
  To counter this, when the player moves again the best path is recalculated