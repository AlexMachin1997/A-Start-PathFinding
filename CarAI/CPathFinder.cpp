//#include "stdafx.h"
#include "CPathFinder.h"


CPathFinder::CPathFinder(int* m, int nsx, int nsy, int ngx, int ngy)
{	
	map = m;
	
	//debug map pass
	for (int row = 0; row < mapsize; row++)
	{
		for (int col = 0; col < mapsize; col++)
		{
			tmap[row*mapsize+col] = map[row*mapsize + col];
		}
	}

	mapSizeX = mapsize;
	mapSizeY = mapsize;
	sx = nsx; 
	sy = nsy;
	gx = ngx; 
	gy = ngy;
	pathfound = false;
}


CPathFinder::~CPathFinder()
{
}

STACK CPathFinder::doIt()
{
	//create node map from source map
	for (int row = 0; row < mapSizeY; row++)
	{
		for (int col = 0; col < mapSizeX; col++)
		{
			//int a = map[row*mapsize + col];

			if (map[row*mapsize+col] == 0)
			{
				nodeMap[row][col].open = false;
				nodeMap[row][col].visited = false;
				nodeMap[row][col].cellValue = '0';
			}			   
			else		   
			{			   
				nodeMap[row][col].open = true;
				nodeMap[row][col].cellValue = '1';
			}			   
		}
	}

	insertStart(sx, sy);
	insertGoal(gx, gy);
	
	//find path
	openList.push(nodeMap[sy][sx]);

	while (!(openList.empty()))
	{
		nx = openList.top().x;
		ny = openList.top().y;
		openList.pop();

		//check for goal
		if (nodeMap[ny][nx].cellValue == 'G')
		{
			nodeMap[ny][nx].dest_found = true;//???
			pathfound = true;
			break;
		}
		else
		{
			//search adjacent nodes
			if (checkAdj(nx, ny - 1, nx, ny) == true)break;//check north
			if (checkAdj(nx + 1, ny, nx, ny) == true)break;//check east
			if (checkAdj(nx, ny + 1, nx, ny) == true)break;//check south
			if (checkAdj(nx - 1, ny, nx, ny) == true)break;//check west
		}
	}

	//End of pathfind
	

	//should be directions!!
	return dirList;
}

void CPathFinder::plotPath(int x, int y)
{
	int TX, TY; //temp x, y		
	for (;;)
	{
		if (nodeMap[y][x].cellValue == 'S')break;//if at start node
		else
		{
			dirList.push_front(nodeMap[y][x]);
			directionList.push(nodeMap[y][x]);
			TX = nodeMap[y][x].px;			//get next cNode
			TY = nodeMap[y][x].py;
			x = TX; y = TY;
		}
	}
}

bool CPathFinder::checkAdj(int x, int y, int px, int py)
{

	if (validNode(y, x))
	{

		if (nodeMap[y][x].x == gx && nodeMap[y][x].y == gy)//check for goal
		{

			nodeMap[y][x].x = x;
			nodeMap[y][x].y = y;
			nodeMap[y][x].px = px;	//set parent coords
			nodeMap[y][x].py = py;
			//performance.lengthOfPath = map.plotpath(px, py);		//plot path
			
			plotPath(x, y);

			pathfound = true;

			openList.push(nodeMap[y][x]);

			return true;
		}
		else	//if not at goal
		{

			if (nodeMap[y][x].visited == false)//if not previously visited
			{
				nodeMap[y][x].visited = true;
				nodeMap[y][x].g = nodeMap[py][px].g + 10;	//calculate costs
				//Heuristic calculations
				//Comment out chosen heuristic

				//Manhattan
				MX = gx - x;
				if (MX < 0)MX = -1 * MX;
				MY = gy - y;
				if (MY < 0)MY = -1 * MY;
	
				nodeMap[y][x].h = 10 * (MX + MY);

				//Straight (direct) Line... pythag
				//map.nodeMap[y][x].h=10*( ((x-gx)*(x-gx)) + ((y-gy)*(y-gy)) );

				//End of Heuristic

				nodeMap[y][x].f = nodeMap[y][x].g + nodeMap[y][x].h;

				nodeMap[y][x].cellValue = '-';

				nodeMap[y][x].x = x;
				nodeMap[y][x].y = y;
				nodeMap[y][x].px = px;	//set parent coords
				nodeMap[y][x].py = py;

				openList.push(nodeMap[y][x]);
			}
			else	//if allready visited, compare G scores
			{		//if new g is less change parents

				if (nodeMap[y][x].g > nodeMap[y][x].g + 10)
				{
					nodeMap[y][x].f = nodeMap[y][x].g + nodeMap[y][x].h;
					nodeMap[y][x].px = px;	//set parent coords
					nodeMap[y][x].py = py;

					openList.push(nodeMap[y][x]);
				}
			}
		}
		return false;
	}

}
void CPathFinder::displayList(LIST)
{

}



void CPathFinder::insertStart(int x, int y)
{
	//used as a constructer
	nodeMap[y][x].cellValue = 'S';

	nodeMap[y][x].x = x;
	nodeMap[y][x].y = y;
	nodeMap[y][x].px = x;	//make parent same???
	nodeMap[y][x].py = y;	//comparison is made when plotting path

	nodeMap[y][x].f = 0;
	nodeMap[y][x].g = 0;
	nodeMap[y][x].h = 0;
	nodeMap[y][x].open = true;
	nodeMap[y][x].visited = true;
	nodeMap[y][x].dest_found = false;
}

void CPathFinder::insertGoal(int x, int y)
{
	nodeMap[y][x].cellValue = 'G';

	nodeMap[y][x].x = x;
	nodeMap[y][x].y = y;

	nodeMap[y][x].f = 0;
	nodeMap[y][x].g = 0;
	nodeMap[y][x].h = 0;
}


bool CPathFinder::validNode(int y, int x)//validate cNode when pathfinding
{
	//check with scale of map array
	if (x > mapsize|| x<0 || y>=mapsize|| y < 0) { return false; }

	//check for walls
	if (nodeMap[y][x].cellValue == '1') { return false; }
	else 
		return true;
}

//---------
//CNode functions 
//---------

cNode::cNode()
{
	x = 0;
	y = 0;
	px = 0;
	py = 0;
	f = 0;
	g = 0;
	h = 0;
	open = false;
	visited = false;
	dest_found = false;
	cellValue = '0';
}
