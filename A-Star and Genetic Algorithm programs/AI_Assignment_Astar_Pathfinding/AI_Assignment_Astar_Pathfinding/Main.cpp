//AI Assignment 2 - A* Pathfinding algorithm - Making use of the A* code provided and intergrating it with reading from a text file to find the path based off of the binary map described in the file

#include <iostream>
#include <fstream>
#include <istream>
#include <iomanip>
#include <queue>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

//Variables that store the width and height of the map
int gridWidth;
int gridHeight;
//Array that stores the map information
vector<vector<int>> grid;
//Setting the below array values to 100 to hopefully accomodate different maze sizes
//Array that stores information related to the closed nodes within the map (nodes that the pathfinding agent has already travelled through)
static int closedNodesGrid[100][100];
//Array that stores information related to the open nodes within the map (nodes that the pathfinding agent can travel through)
static int openNodesGrid[100][100];
//Array that stores the directions in which the pathfinding agent can travel
static int dirGrid[100][100];
//Number of total directions the pathfinding agent can travel (8 means the agent can move into all 8 nodes around it if not obstructed)
const int dir = 8;
//Array storing information related to the directions the agent can move along the x axis of the map
static int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
//Array storing information related to the directions the agent can move along the y axis of the map
static int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

//Class for nodes
class node
{
	//Variable that stores node's current x-axis position
	int posX;
	//Variable that stores node's current y-axis position
	int posY;
	//Variable that stores the total distance the node has already travelled
	int distance;
	//Variable that stores the priority of a node, with the calculation being priority = distance + remaining distance estimate, the smaller the calculated number, the higher the priority that node has
	int priority;

public:
	//Function to set up the node object, setting all the neccessary variables to there required values for the created node 
	node(int xp, int yp, int d, int p)
	{
		posX = xp; posY = yp; distance = d; priority = p;
	}
	//Function for retrieving the x position of the node
	int getPosX() const 
	{ 
		return posX; 
	}
	//Function for retrieving the y position of the node
	int getPosY() const 
	{ 
		return posY; 
	}
	//Function for retrieving the total distance the node has already travelled
	int getDistance() const 
	{ 
		return distance; 
	}
	//Function for retrieving the calculated priority from the node
	int getPriority() const 
	{ 
		return priority; 
	}

	//Function that updates priority based off of the priority calculation
	void updatePriority(const int& destX, const int& destY)
	{
		priority = distance + estimate(destX, destY) * 10;
	}
	
	//Function that adjusts the distance variable in accordance with the node having travelled further from its start point 
	void nextDistance(const int& i)
	{
		distance += (dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	//Function that calculates the estimates Euclidian distance the node still has to travel before it reaches the map finish
	const int& estimate(const int& destX, const int& destY) const
	{
		static int xd, yd, d;
		xd = destX - posX;
		yd = destY - posY;

		//Euclidian distance calculation
		d = static_cast<int>(sqrt(xd * xd + yd * yd));
		//Returns calculated distance
		return d;
	}
};
//Function that determines node priority within the priority queue
bool operator<(const node& a, const node& b)
{
	return a.getPriority() > b.getPriority();
}

//Primary A* pathfinding function - This function figures out the path through the map
string pathFind(const int& xStart, const int& yStart, const int& xFinish, const int& yFinish)
{
	//Array that stores a list of open nodes (Nodes that the pathfinding agent hasnt tried yet)
	static priority_queue<node> priorityQueue[2];
	//Variable that stores the number used to index the priority queue  
	static int priorityQueueIndex;
	//Pointer to the parent node
	static node* n0;
	//Pointer to the child node
	static node* n1;
	static int i, j, x, y, xdx, ydy;
	static char c;
	//Setting index to 0 on function call
	priorityQueueIndex = 0;
	//Set all the elements of the open and closed node maps to 0 to reset the map
	for (y = 0; y < gridHeight; y++)
	{
		for (x = 0; x < gridWidth; x++)
		{
			closedNodesGrid[x][y] = 0;
			openNodesGrid[x][y] = 0;
		}
	}

	//Create the start node and push into list of open nodes
	n0 = new node(xStart, yStart, 0, 0);
	n0->updatePriority(xFinish, yFinish);
	priorityQueue[priorityQueueIndex].push(*n0);
	//Mark it on the open nodes map
	openNodesGrid[x][y] = n0->getPriority();

	// A* search
	while (!priorityQueue[priorityQueueIndex].empty())
	{
		//Get the current node w/ the highest priority from the list of open nodes
		n0 = new node(priorityQueue[priorityQueueIndex].top().getPosX(), priorityQueue[priorityQueueIndex].top().getPosY(), priorityQueue[priorityQueueIndex].top().getDistance(), priorityQueue[priorityQueueIndex].top().getPriority());

		x = n0->getPosX(); y = n0->getPosY();

		//Remove the node from the open list
		priorityQueue[priorityQueueIndex].pop(); 
		openNodesGrid[x][y] = 0;
		//Mark it on the closed nodes map
		closedNodesGrid[x][y] = 1;

		//Quit searching when the goal state is reached
		if (x == xFinish && y == yFinish)
		{
			//Generate the path from finish to start by following the directions
			string path = "";
			while (!(x == xStart && y == yStart))
			{
				j = dirGrid[x][y];
				c = '0' + (j + dir / 2) % dir;
				path = c + path;
				x += dx[j];
				y += dy[j];
			}

			//Cleanup nodes
			delete n0;
			//Empty the leftover nodes
			while (!priorityQueue[priorityQueueIndex].empty())
			{
				priorityQueue[priorityQueueIndex].pop();
			}
			//Return the found path
			return path;
		}

		//Generate moves (child nodes) in all possible directions
		for (i = 0; i < dir; i++)
		{
			xdx = x + dx[i]; ydy = y + dy[i];

			if (!(xdx<0 || xdx>gridWidth - 1 || ydy<0 || ydy>gridHeight - 1 || grid[xdx][ydy] == 1 || closedNodesGrid[xdx][ydy] == 1))
			{
				//Generate a child node
				n1 = new node(xdx, ydy, n0->getDistance(), n0->getPriority());
				n1->nextDistance(i);
				n1->updatePriority(xFinish, yFinish);

				//If it is not in the open list then add into that
				if (openNodesGrid[xdx][ydy] == 0)
				{
					openNodesGrid[xdx][ydy] = n1->getPriority();
					priorityQueue[priorityQueueIndex].push(*n1);
					//Mark its parent node direction
					dirGrid[xdx][ydy] = (i + dir / 2) % dir;
				}
				else if (openNodesGrid[xdx][ydy] > n1->getPriority())
				{
					//Update the priority info
					openNodesGrid[xdx][ydy] = n1->getPriority();
					//Update the parent direction info
					dirGrid[xdx][ydy] = (i + dir / 2) % dir;

					//Replace the node by emptying one pq to the other one except the node to be replaced will be ignored and the new node will be pushed in instead
					while (!(priorityQueue[priorityQueueIndex].top().getPosX() == xdx && priorityQueue[priorityQueueIndex].top().getPosY() == ydy))
					{
						priorityQueue[1 - priorityQueueIndex].push(priorityQueue[priorityQueueIndex].top());
						priorityQueue[priorityQueueIndex].pop();
					}
					//Remove the wanted node
					priorityQueue[priorityQueueIndex].pop(); 

					//Empty the larger size pq to the smaller one
					if (priorityQueue[priorityQueueIndex].size() > priorityQueue[1 - priorityQueueIndex].size()) priorityQueueIndex = 1 - priorityQueueIndex;
					while (!priorityQueue[priorityQueueIndex].empty())
					{
						priorityQueue[1 - priorityQueueIndex].push(priorityQueue[priorityQueueIndex].top());
						priorityQueue[priorityQueueIndex].pop();
					}
					priorityQueueIndex = 1 - priorityQueueIndex;
					//Add the better node instead
					priorityQueue[priorityQueueIndex].push(*n1); 
				}
				else
				{
					//Cleanup nodes
					delete n1; 
				}
			}
		}
		//Cleanup nodes
		delete n0; 
	}
	//Return empty if no route is found
	return ""; 
}

//Function used to remove the spaces from the text input
vector <string> splitArgs(string args, char delim)
{
	//String vector used to store and return the new string without spaces
	vector <string> argVec;
	//Get start of string
	auto start = begin(args);
	//Find end of string
	auto end = find(start, std::end(args), delim);
	//Loop to push characters into the string vector
	while (end != std::end(args))
	{
		argVec.emplace_back(start, end);
		start = end + 1;
		end = find(start, std::end(args), delim);
	}
	argVec.emplace_back(start, std::end(args));
	//Return string vector
	return argVec;
}

//Function that reads from input file
void readFromFile(std::vector <int>& gridVector)
{
	//String that stores the name of the file the application should read from, the file name being input by the user
	std::string gridFileName;
	//Sting that stores the text from the input file
	std::string gridText;
	std::cout << "Please Enter the exact name of the maze file you wish to read from, including file designation (e.g: FileNameHere.txt): ";
	std::cin >> gridFileName;
	std::cout << std::endl;
	//Reading map file
	std::ifstream gridFileRead(gridFileName);
	//Error checking for ensuring that the file has been opened and read successfully
	if (gridFileRead.is_open())
	{
		//Storing file contents into a vector whilst removing spaces
		while (getline(gridFileRead, gridText))
		{
			auto x = splitArgs(gridText, ' ');
			for (int j = 0; j < x.size(); j++)
			{
				int yNum = stoi(x[j]);
				gridVector.push_back(yNum);
			}
		}
	}
	//Error catching
	else
	{
		std::cout << "File Unable To Open" << std::endl;
	}
	//Closing the file
	gridFileRead.close();
}

int main()
{
	//Temp variable used to prevent the application from immediately closing when the route has been found
	string temp;
	//Count variable used to determine which character within the grid vector the function should look at
	int count = 2;
	//Variables that store the x and y positions of the start and finish positions within the map
	int xS, yS, xF, yF;
	//Vector that stores input from file
	vector <int> gridVector;
	
	//Reading map file
	readFromFile(gridVector);

	//Printing the read in from the file for debugging
	cout << "Read from File: " << endl;
	for(int i = 0; i < gridVector.size(); i++)
	{
		cout << gridVector.at(i) << " ";
	}
	cout << "\n";
	
	//Storing the valuyes for both the width and height of the grid in seperate ints
	gridWidth = gridVector.at(0);
	gridHeight = gridVector.at(1);

	//Calculate total grid size and output it
	int gridSize = gridWidth * gridHeight;
	std::cout << "Area of the maze: " << gridSize << "\n";
	
	//For loop to set the 2D vector to the correct size for the current maze
	for (int i = 0; i < gridWidth; i++)
	{
		grid.emplace_back(vector<int>(gridHeight, 0));
	}

	//For loop that assigns the values of the maze into their correct position in the 2D vector
	for (int j = 0; j < gridHeight; j++)
	{
		for (int i = 0; i < gridWidth; i++)
		{
			grid[i][j] = gridVector.at(count);
			if (gridVector.at(count) == 2)
			{
				//Storing start position in the maze for later use
				xS = i;
				yS = j;
			}
			else if (gridVector.at(count) == 3)
			{
				//Storing finish position in the maze for later use
				xF = i;
				yF = j;
			}
			count++;
		}
	}
	count = 0;

	//Outputs the size of the map, the start position within the map, and the end position within the map
	cout << "Map Size(X,Y): " << gridWidth << "," << gridHeight << endl;
	cout << "Start(X,Y): " << xS << "," << yS << endl;
	cout << "Finish(X,Y): " << xF << "," << yF << endl;
	cout << "\n";
	//Starting the timer used to show how long the application took to calculate the path through the maze
	clock_t start = clock();
	//Call pathfinding function to calcualte path and store the return from the function in the route string
	string route = pathFind(xS, yS, xF, yF);
	//Cahnge output if no route is generated
	if (route == "")
	{
		cout << "An Empty Route Generated!" << endl;
	}
	//End the timer
	clock_t end = clock();
	//Calculate total time taken for the application to calculate the route 
	double timeElapsed = double(end - start);
	//Output map, time taken to calculate route and the numerical route taken by the pathfinding agent
	cout << "Grid with generated route (0 = Empty, 1 = Wall, 2 = Start, R = Route): " << endl;
	cout << "Time to calculate the route (ms): " << timeElapsed << endl;
	cout << "Route: " << endl;
	cout << route << endl;

	//Follow the route on the map and display it
	if (route.length() > 0)
	{
		int j; char c;
		int x = xS;
		int y = yS;
		grid[x][y] = 2;
		for (int i = 0; i < route.length(); i++)
		{
			c = route.at(i);
			j = atoi(&c);
			x = x + dx[j];
			y = y + dy[j];
			grid[x][y] = 3;
		}
		//For loop and if statments used to display the map with the route
		for (int y = 0; y < gridHeight; y++)
		{
			for (int x = 0; x < gridWidth; x++)
			{
				if (grid[x][y] == 0)
				{
					//Empty spot
					cout << " 0 ";
				}
				else if (grid[x][y] == 1)
				{
					//Wall
					cout << " 1 ";
				}
				else if (grid[x][y] == 2)
				{
					//Start
					cout << " 2 ";
				}
				else if (grid[x][y] == 3)
				{
					//Route taken
					cout << " R ";
				}
				else if (grid[x][y] == 4)
				{
					//Finish
					cout << " 3 ";
				}
			}
			cout << endl;
		}
	}
	//Catching the end of the application to ensure it doesn't immediately close after the route has been calculated
	cout << "Press Enter in any character to close the program: ";
	cin >> temp;
	return(0);
}

