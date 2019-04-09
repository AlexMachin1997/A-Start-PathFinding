#pragma once

//STL (Standard Template Library)
#include <list>			
#include <queue>
using namespace std;
#define mapsize 10


class cNode
{
public:
	int x, y;
	int px, py;
	int f, g, h;
	bool open;
	bool visited;
	bool dest_found;
	char cellValue;

	//overload operator for sort function to operate
	bool operator < (const cNode &n1)const
	{
		return n1.f < f;//return lower value node (cost)
	}

	cNode();
};

typedef priority_queue<cNode>  LIST; //STL definition, requires <queue> and namespace standard
										//priority queue removes the need to sort - improves effieciency #define mapsize 10

typedef list<cNode>  STACK; //STL definition, requires <queue> and namespace standard
										//priority queue removes the need to sort - improves effieciency #define mapsize 10



class CPathFinder
{
public:

	//declare in pathfnder function after recieving mapsize
	//cNode nodeMap[][];//=new cNode[map.length][map[0].length];

	int sx, sy;
	int gx, gy;
	bool pathFound;
	int nx, ny; //current node
	int type;	//which algorithm to use?
	int MX, MY;		//Manhattan Variables for the heuristic

	int mapSizeX;
	int mapSizeY;
	int* map;
	int tmap[mapsize * mapsize];

	cNode nodeMap[mapsize][mapsize];

	bool pathfound;

	LIST openList;
	LIST directionList;
	STACK dirList;

	CPathFinder();
	~CPathFinder();


	CPathFinder(int* m, int, int, int, int );
	STACK doIt( );
	bool checkAdj(int, int, int, int);
	void displayList(LIST);
	bool validNode(int, int);
	void insertStart(int, int);
	void insertGoal(int, int);
	void plotPath(int, int);


};
