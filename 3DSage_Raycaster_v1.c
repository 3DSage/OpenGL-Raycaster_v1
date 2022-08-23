#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

//-----------------------------MAP----------------------------------------------

//mapWidth and mapHeight give us an 8 X 8 map with each square of that map being 64 units in size
//meaning we have in total there 4096 individual parts of the map
#define mapWidth  8      //map width
#define mapHeight  8      //map height
#define mapCubeSize 64      //map cube size

//the map array. Edit to change level but keep the outer walls
int map[]= {
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};

// This function draws the map based on the array above
void drawMap2D()
{
 
 //loops through the map height
 for( int loopThroughMapHeight = 0; loopThroughMapHeight < mapHeight; loopThroughMapHeight++)
 {
 	
  //loops through the map width
  for(int loopThroughMapWidth = 0; loopThroughMapWidth < mapWidth; loopThroughMapWidth++)
  {
  	
  	//worth noting this code only colors the walls on the minimap
  	
   //checks if current spot has a wall
   if( map[loopThroughMapHeight * mapWidth + loopThroughMapWidth] == 1 ){
    
   		//makes walls white
		glColor3f(1,1,1);
		
   //if theres no wall
   }else{ 
   
        //makes cubes black
        glColor3f(0,0,0);
        
   }
   
   //these offsets are calculated using the mapCubeSize which is 64 and one cube is 64 units in size
   int xOffset = loopThroughMapWidth * mapCubeSize, yOffset = loopThroughMapHeight * mapCubeSize;
   
   glBegin(GL_QUADS); 
   
   //this lays out the indivdual lines that make up the cubes on the minimap
   //each vertex command makes a point and the 1 pushes each vertex to give 
   //it an outline when drawn on the minimap
   glVertex2i( 0 + xOffset + 1, 0 + yOffset + 1); 
   glVertex2i( 0 + xOffset + 1, mapCubeSize + yOffset - 1); 
   glVertex2i( mapCubeSize + xOffset - 1, mapCubeSize + yOffset - 1);  
   glVertex2i( mapCubeSize + xOffset - 1, 0 + yOffset + 1); 
   
   glEnd();
  } 
 } 
}//-----------------------------------------------------------------------------


//------------------------PLAYER------------------------------------------------

//converts radians in terms of pi
float degToRad(int angle){ 

		return angle * M_PI / 180.0;
		
	}

int FixAng(int angle){
	
	 if(angle > 359){
	 	
	 	 angle -= 360;
	 	 
	 } 
	 
	 if(angle < 0){
	 	
	     angle += 360;
	     
	 } 
	
	return angle;
}
	
//floats that store the players position here
float PlayerXcordinate, PlayerYcordinate, playerDeltaX, playerDeltaY, playerAngle;

//this function draws the player
void drawPlayer2D()
{
 //makes the player on 2D map yellow as R & G are both set
 glColor3f(1,1,0);  
 
 //sets player size on 2D map
 glPointSize(8);

 //sets the width of the line from player
 glLineWidth(4);
 
 glBegin(GL_POINTS);
 
 //sets point of player origin
 glVertex2i(PlayerXcordinate, PlayerYcordinate); 
 
 glEnd();
 
 glBegin(GL_LINES);  
 
 glVertex2i(PlayerXcordinate ,PlayerYcordinate); 
 
 glVertex2i( PlayerXcordinate + playerDeltaX * 20, PlayerYcordinate + playerDeltaY * 20 ); 
 
 glEnd();
}

//----------------------------------------------------------------------------------

//-----------------------------BUTTONS----------------------------------------------

void Buttons(unsigned char key,int x,int y)
{

 //these if statements all control the input from keyboard
 
 //when we use a or d we have to add or subtract a small amount from the player angle first
 // using a (looking right) we add a small amount and calculate our deltas using sin & cos
 
 
 if(key=='a'){ 
 	 playerAngle += 5; 
	 playerAngle = FixAng(playerAngle); 
	 playerDeltaX = cos(degToRad(playerAngle));
	 playerDeltaY =- sin(degToRad(playerAngle));
 } 	
 
 if(key=='d'){ 
	 playerAngle -= 5; 
	 playerAngle = FixAng(playerAngle); 
	 playerDeltaX = cos(degToRad(playerAngle)); 
	 playerDeltaY =- sin(degToRad(playerAngle));
 } 
 
 //because of the calculations we have to do in regards to angles we have to
 //update both the x and y cordinates when we move on either axis
 if(key=='w'){ 
	 PlayerXcordinate += playerDeltaX * 5; 
	 PlayerYcordinate += playerDeltaY * 5;
 }
 
 if(key=='s'){ 
	 PlayerXcordinate -= playerDeltaX * 5; 
	 PlayerYcordinate -= playerDeltaY * 5;
 }
 
 //refreshes the display
 glutPostRedisplay();
 
}

//------------------------------------------------------------------------------

//---------------------------Draw Rays and Walls--------------------------------

void drawRays2D()
{
 glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();	
 glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();	 	
	
 int mx, my, mapPosition, depthOfField, side; 
 float verticalX, verticalY, horizontalIntersectX, horizontalIntersectY, raysAngle, xOffset, yOffset, distanceVertical, distanceHorizontal; 
 
 //ray set back 30 degrees to align field of view center point with the center point of player perspective & movement
 raysAngle = FixAng(playerAngle + 30);
 
 //loop which casts 60 rays from left to right
 for(int rayCount = 0; rayCount < 60; rayCount++) {
 	
  //---Vertical--- 
  depthOfField = 0; 
  side = 0; 
  distanceVertical = 100000;
  float Tan = tan(degToRad(raysAngle));
  
  //all of these pieces of code are checking which way our ray is facing
  if(cos(degToRad(raysAngle)) > 0.001){ 
  
  		horizontalIntersectX = (((int)PlayerXcordinate >> 6) << 6) + 64;      
		horizontalIntersectY = (PlayerXcordinate - horizontalIntersectX) * Tan + PlayerYcordinate; 
		
		xOffset = 64; 
		yOffset =- xOffset * Tan;
		
  }//looking left
  else if(cos(degToRad(raysAngle)) <- 0.001){ 
  
  		horizontalIntersectX = (((int)PlayerXcordinate >> 6) << 6) - 0.0001; 
		horizontalIntersectY = (PlayerXcordinate - horizontalIntersectX) * Tan + PlayerYcordinate;
		 
		xOffset =- 64; 
		yOffset =- xOffset * Tan;
		
  }//looking right
  else { 
  
  		horizontalIntersectX = PlayerXcordinate; 
		horizontalIntersectY = PlayerYcordinate; 
		
		depthOfField = 8;
  }                                                  //looking up or down. no hit  

  while(depthOfField < 8) { 
  
   mx = (int)(horizontalIntersectX) >> 6; 
   my = (int)(horizontalIntersectY) >> 6;
    
   mapPosition = my * mapWidth + mx;    
                    
   if(mapPosition > 0 && mapPosition < mapWidth * mapHeight && map[mapPosition] == 1){ 
   		depthOfField = 8; 
		distanceVertical = cos(degToRad(raysAngle)) * (horizontalIntersectX - PlayerXcordinate) - sin(degToRad(raysAngle)) * (horizontalIntersectY - PlayerYcordinate);
   }
   
   //hit         
   else{ 
   		horizontalIntersectX += xOffset; 
		horizontalIntersectY += yOffset; 
		depthOfField += 1;
	}  
	//check next horizontal
  } 
  
  verticalX = horizontalIntersectX; 
  verticalY = horizontalIntersectY;

  //---Horizontal---
  
  depthOfField = 0;
  distanceHorizontal = 100000;
  Tan = 1.0 / Tan; 
  
  if(sin(degToRad(raysAngle)) > 0.001){ 
  
  		horizontalIntersectY = (((int)PlayerYcordinate >> 6) << 6) - 0.0001; 
		horizontalIntersectX = (PlayerYcordinate - horizontalIntersectY) * Tan + PlayerXcordinate; 
		
		yOffset =- 64; 
		xOffset =- yOffset * Tan;
		
  }//looking up 
  
  else if (sin(degToRad(raysAngle)) <- 0.001){ 
  
  		horizontalIntersectY = (((int)PlayerYcordinate >> 6) << 6) + 64;      
		horizontalIntersectX = (PlayerYcordinate - horizontalIntersectY) * Tan + PlayerXcordinate; 
		
		yOffset = 64; 
		xOffset =- yOffset * Tan;
		
  }
  
  //looking down
  else{ 
  
  		horizontalIntersectX = PlayerXcordinate; 
		horizontalIntersectY = PlayerYcordinate;
		 
		depthOfField = 8;
  }                                                   //looking straight left or right
 
  while(depthOfField < 8) { 
  
   mx = (int)(horizontalIntersectX) >> 6; 
   my = (int)(horizontalIntersectY) >> 6;
    
   mapPosition = my * mapWidth + mx; 
                            
   if( mapPosition > 0 && mapPosition < mapWidth * mapHeight && map[mapPosition] == 1){ 
   
   			depthOfField = 8; 
			distanceHorizontal = cos(degToRad(raysAngle)) * (horizontalIntersectX - PlayerXcordinate) - sin(degToRad(raysAngle)) * (horizontalIntersectY - PlayerYcordinate);
			
   }
   
   //hit         
   else{ 
            horizontalIntersectX += xOffset;
			horizontalIntersectY += yOffset;
			 
			depthOfField += 1;
   }   
                                               //check next horizontal
  } 
  
  glColor3f(0,0.8,0);
  
  if( distanceVertical < distanceHorizontal ){ 
  
  		horizontalIntersectX = verticalX; 
		horizontalIntersectY = verticalY; 
		
		distanceHorizontal = distanceVertical;
		 
		glColor3f(0,0.6,0);
  }            
  //horizontal hit first
  
  glLineWidth(2); glBegin(GL_LINES); glVertex2i(PlayerXcordinate, PlayerYcordinate); glVertex2i(horizontalIntersectX, horizontalIntersectY); glEnd();//draw 2D ray
    
  int ca = FixAng(playerAngle - raysAngle); distanceHorizontal = distanceHorizontal * cos(degToRad(ca));                            //fix fisheye 
  int lineH = (mapCubeSize * 320) / distanceHorizontal;
   
  if(lineH > 320){ 
  		lineH = 320;
  }                                            																				//line height and limit
  int lineOff = 160 - (lineH >> 1);                                                                                           //line offset
  
  glLineWidth(8); glBegin(GL_LINES); glVertex2i(rayCount * 8 + 530, lineOff); glVertex2i(rayCount * 8 + 530, lineOff + lineH); glEnd(); //draw vertical wall  

  raysAngle = FixAng(raysAngle - 1);                                                              //go to next ray
 }
}//---------------------------------------------------------------------------------------------

//-----------------------------INITILISATION CLASS----------------------------------------------

void init()
{
 //this sets the background to a dark gray
 glClearColor(0.3,0.3,0.3,0);

 //this sets the new window dimensions
 gluOrtho2D(0,1024,510,0);
 
 //intialise players position here
 PlayerXcordinate = 150; PlayerYcordinate = 400; playerAngle = 90;
 
 //player delta X & Y have to be calculated on program start or else they dont exist
 playerDeltaX = cos(degToRad(playerAngle)); 
 playerDeltaY =- sin(degToRad(playerAngle)); 
}

void display()
{   
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 
 //we have to draw our map before our player as our player lives in the map
 drawMap2D();
 
 //draw player
 drawPlayer2D();
 
 //then draw rays as the player object has to be present first
 drawRays2D();
 
 glutSwapBuffers();  
}
//-------------------------------------------------------------------------------------

//-----------------------------MAIN CLASS----------------------------------------------

int main(int argc, char* argv[])
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 
 //sets window dimensions
 glutInitWindowSize(1024,510);
 
 //sets window name
 glutCreateWindow("Cold Blood - Alpha");
 
 //calls the initilisation function
 init();
 
 
 glutDisplayFunc(display);
 
 //this takes our keyboard input and passes it to the buttons function
 glutKeyboardFunc(Buttons);
 
 glutMainLoop();
}


