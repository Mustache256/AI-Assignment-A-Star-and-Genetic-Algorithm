#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <math.h>
#include <istream>
#include <fstream>
#include <random>
#include "Chromosome.h"

//Seeding random number generator and creating distribution variable 
std::random_device randDev;
std::mt19937 generator(randDev());
std::uniform_real_distribution<double> distribution(0.0, 1.0);

//Array that stores chromosome population
chromosome pop[10];
//Vector that stores map vector
std::vector<std::vector<int>> grid;

//Function that displays the initial maze before the route is found
void gridDisplay(int gridWidth, int gridHeight)
{
	std::cout << "Grid with no route (0 = Empty, 1 = Wall, 2 = Start, 3 = Finish): " << std::endl;
	int count = 0;
	//Loop used to display maze from 2D array
	for (int x = 0; x < gridWidth; x++)
	{
		for (int y = 0; y < gridHeight; y++)
		{
			std::cout << " " << grid[x][y] << " ";

			if (++count >= gridWidth)
			{
				count = 0;
				std::cout << "\n";
			}
		}
	}
}

//Function used to remove the spaces from the text input
std::vector <std::string> splitArgs(std::string arguments, char space)
{
	//String vector used to store and return the new string without spaces
	std::vector <std::string> argumentVector;
	//Get start of string
	auto start = begin(arguments);
	//Find end of string
	auto end = find(start, std::end(arguments), space);
	//Loop to push characters into the string vector
	while (end != std::end(arguments))
	{
		argumentVector.emplace_back(start, end);
		start = end + 1;
		end = find(start, std::end(arguments), space);
	}
	argumentVector.emplace_back(start, std::end(arguments));
	//Return string vector
	return argumentVector;
}

//Function that generates a population of 10 chromosomes
void generatePopulation(int xStart, int yStart, int gridSize, int cRate, int mRate)
{
	//For loops that generates population
	for (int i = 0; i < 10; ++i)
	{
		//Create new chromosome
		pop[i].setChromosomeValues(xStart, yStart, gridSize, cRate, mRate);
	}
}

//Function that reads from text file
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

//Function that manages chromosome mutation
void mutation()
{
	float rand;
	int randInt;
	std::vector<int> temp;
	//Looping through entire population
	for (int i = 0; i < 10; ++i)
	{
		//Distribution for picking gene to mutate defined here
		std::uniform_real_distribution <double> element(0.0, (double)pop[i].getChromosomeLength() - 1);
		rand = distribution(generator);
		//Only mutate if mutation rate is met
		if (rand <= 0.001)
		{
			//Store current chromosome num in temp vector
			temp = pop[i].getChromosomeNum();
			//Determine which element to chane
			randInt = (int)round(element(generator));
			//Change element based off of its current value
			if (temp[randInt] == 0)
			{
				temp[randInt] = 1;
			}
			else
			{
				temp[randInt] = 0;
			}
			//Set chromosome num to new num after mutation
			pop[i].setChromosomeNum(temp);
		}
		//Skip if mutation rate isnt met
		else
		{
			continue;
		}
	}
	return;
}

//Fucntion that manages population breeding
void crossOver(std::vector<chromosome> pairs)
{
	float rand;
	float crossOverRate = 0.7;

	//Loop to ensure breeding can occur in all pairs
	for (int i = 0; i < 10; i += 2)
	{
		rand = distribution(generator);
		//Perform breeding if crossover rate is met
		if (rand <= crossOverRate)
		{
			std::vector<int> childOne, childTwo;

			int cOneNum, cTwoNum;

			//Set children to be equal to the parents temporarily
			childOne = pairs[i].getChromosomeNum();
			childTwo = pairs[i + 1].getChromosomeNum();

			//Get half of the chromosomes from each and swap them over to produce the 2 children
			for (int j = 0; j < (childOne.size() / 2); j++)
			{
				cOneNum = childOne[j];
				cTwoNum = childTwo[j];

				childOne[j] = cTwoNum;
				childTwo[j] = cOneNum;
			}
			//Set children into population array to create a new population from the offspring
			pop[i].setChromosomeNum(childOne);
			pop[i + 1].setChromosomeNum(childTwo);
		}
		//Skip breeding and keep chromosomes the same
		else
		{
			continue;
		}
	}
	return;
}

//Function that manages population fitness calculations and determining breeding pairs
std::vector<chromosome> setPopFitness(int xFinish, int yFinish)
{
	float fitnessArray[10];
	float wheelArray[10];
	float totalFitness = 0.0f;
	float rand;
	//Calculate total fitness of population
	for (int i = 0; i < 10; ++i)
	{
		fitnessArray[i] = pop[i].calculateFitness(xFinish, yFinish);
		totalFitness += fitnessArray[i];
	}

	//Set chromosome roulette wheel percentage in array to determione the likelyhood at which each chromosome in the population will be selected
	for (int i = 0; i < 10; i++)
	{
		wheelArray[i] = pop[i].getFitness() / totalFitness;
	}

	std::vector<chromosome> pairs;

	//Run thorugh entire population to determine pairs
	for (int i = 0; i < 10; ++i)
	{
		rand = distribution(generator);
		//Select chromosomefrom roulette and push them in pairs vector, determining the pairs
		if (rand >= 0 && rand < wheelArray[0])
		{
			pairs.push_back(pop[0]);
		}
		else if (rand >= wheelArray[0] && rand < (wheelArray[0] + wheelArray[1]))
		{
			pairs.push_back(pop[1]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2]))
		{
			pairs.push_back(pop[2]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3]))
		{
			pairs.push_back(pop[3]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4]))
		{
			pairs.push_back(pop[4]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5]))
		{
			pairs.push_back(pop[5]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6]))
		{
			pairs.push_back(pop[6]);;
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6] + wheelArray[7]))
		{
			pairs.push_back(pop[7]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6] + wheelArray[7]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6] + wheelArray[7] + wheelArray[8]))
		{
			pairs.push_back(pop[8]);
		}
		else if (rand >= (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6] + wheelArray[7] + wheelArray[8]) && rand < (wheelArray[0] + wheelArray[1] + wheelArray[2] + wheelArray[3] + wheelArray[4] + wheelArray[5] + wheelArray[6] + wheelArray[7] + wheelArray[8] + wheelArray[9]))
		{
			pairs.push_back(pop[9]);
		}
	}
	//Return the pairs vector with pairs now allocated
	return pairs;
}

//Function that manages chromosome navigation through the maze
bool navigateGrid(int gridWidth, int gridHeight, int xStart, int yStart)
{
	int y;
	int x;
	std::vector<int> genes;

	//Set the start positions of all the chromosomes in the population to be the same as the start position of the maze
	for (int i = 0; i < 10; ++i)
	{
		pop[i].setPosX(xStart);
		pop[i].setPosY(yStart);
	}

	//Loop through and perform moves for entire population
	for (int i = 0; i < 10; ++i)
	{
		genes = pop[i].getChromosomeNum();
		//Ensure all moves are taken by current chromosome
		for (int j = 0; j < pop[i].getChromosomeLength(); j += 2)
		{
			//Move up
			if (genes[j] == 0 && genes[j + 1] == 0)
			{
				//Calculate new y position
				y = pop[i].getPosY() - 1;
				//Check if new y position is out of map or going into a wall
				if (y < 0 || grid[pop[i].getPosX()][y] == 1)
				{
					//Skip move
					continue;
				}
				else
				{
					//Set new y position
					pop[i].setPosY(y);
				}
			}

			//Move right
			else if (genes[j] == 0 && genes[j + 1] == 1)
			{
				//Calculate new x position
				x = pop[i].getPosX() + 1;
				//Check if new x position is out of map or going into a wall
				if (x > gridWidth - 1 || grid[x][pop[i].getPosY()] == 1)
				{
					//Skip move
					continue;
				}
				else
				{
					//Set new x position
					pop[i].setPosX(x);
				}
			}

			//Move down
			else if (genes[j] == 1 && genes[j + 1] == 0)
			{
				//Calculate new x position
				y = pop[i].getPosY() + 1;
				//Check if new x position is out of map or going into a wall
				if (y > gridHeight - 1 || grid[pop[i].getPosX()][y] == 1)
				{
					//Skip move
					continue;
				}
				else
				{
					//Set new x position
					pop[i].setPosY(y);
				}
			}

			//Move left
			else if (genes[j] == 1 && genes[j + 1] == 1)
			{
				//Calculate new y position
				x = pop[i].getPosY() - 1;
				//Check if new y position is out of map or going into a wall
				if (x < 0 || grid[x][pop[i].getPosY()] == 1)
				{
					//Skip move
					continue;
				}
				else
				{
					//Set new y position
					pop[i].setPosX(x);
				}
			}
			//Check if chromosomes final position is at the finish of the maze
			if (grid[pop[i].getPosX()][pop[i].getPosY()] == 3)
			{
				std::cout << "\n";
				std::cout << "SOLUTION FOUND! Chromosome " << i << " has navigated to the finish!" << std::endl;
				std::cout << "Chrmomosome Genes: ";
				pop[i].printChromosome();
				std::cout << "\n";
				return true;
			}
		}
	}
	return false;
}

int main()
{
	//Temp variable used to prevent the application from immediately closing when the route has been found
	std::string temp;
	//Variables that store the width and height of the map
	int gridWidth;
	int gridHeight;
	//Variable that stores the distance between teh start and finish of the maze
	int sfDistance;
	//Count variable used to determine which character within the grid vector the function should look at
	int count = 2;
	//Variables that store the x and y positions of the start and finish positions within the map
	int xS, yS, xF, yF;
	//Vector that stores input from file
	std::vector <int> gridVector;
	//Get text from file and print it
	readFromFile(gridVector);
	std::cout << "Read from File: " << std::endl;
	//Push file text into storage vector
	for (int i = 0; i < gridVector.size(); i++)
	{
		std::cout << gridVector.at(i) << " ";
	}
	std::cout << "\n";

	//Get maze width and height from file text storage vector
	gridWidth = gridVector.at(0);
	gridHeight = gridVector.at(1);

	//Calculate maze area and print it
	int gridSize = gridWidth * gridHeight;
	std::cout << "Area of the maze: " << gridSize << "\n";

	//Setting 2D vector to be the correct size for the map
	for (int i = 0; i < gridWidth; i++)
	{
		grid.emplace_back(std::vector<int>(gridHeight, 0));
	}

	//For loop that fills the 2D vector with values
	for (int i = 0; i < gridWidth; i++)
	{
		for (int j = 0; j < gridHeight; j++)
		{
			//Storing values from text file red storage vector
			grid[i][j] = gridVector.at(count);
			//Store location of start point of the maze in 2 variables
			if (gridVector.at(count) == 2)
			{
				xS = i;
				yS = j;
			}
			//Store location of finish point of the maze in 2 variables
			else if (gridVector.at(count) == 3)
			{
				xF = i;
				yF = j;
			}
			count++;
		}
	}
	count = 0;
	//Display the initial grid from the input file before the route has been calculated
	gridDisplay(gridWidth, gridHeight);
	
	//Creating an empty map
	//Outputs the size of the map, the start position within the map, and the end position within the map
	std::cout << "Map Size(X,Y): " << gridWidth << "," << gridHeight << std::endl;
	std::cout << "Start(X,Y): " << xS << "," << yS << std::endl;
	std::cout << "Finish(X,Y): " << xF << "," << yF << std::endl;

	//Generate initial population
	generatePopulation(xS, yS, gridSize, 0.7, 0.001);
	
	bool foundRoute = false;
	std::vector<chromosome> pairs;
	//Start timer
	clock_t start = clock();
	int episodes = 1;
	//Loop until solution is found
	while (!foundRoute)
	{
		//Navigate maze with current population
		foundRoute = navigateGrid(gridWidth, gridHeight, xS, yS);
		//Exit loop if solution found
		if (foundRoute)
		{
			break;
		}
		//Determine breeding pairs
		pairs = setPopFitness(xF, yF);
		//Perform breeding to create new population
		crossOver(pairs);
		//Perform mutation on population
		mutation();

		episodes += 1;
	}
	//End timer
	clock_t end = clock();
	//Calculate total time taken and print it
	double timeElapsed = double(end - start);
	std::cout << "Time to get solution (ms): " << timeElapsed << std::endl;
	std::cout << "Number of iterations: " << episodes << std::endl;
	//Catching the end of the application to ensure it doesn't immediately close after the route has been calculated
	std::cout << "Press Enter in any character to close the program: ";
	std::cin >> temp;
	return(0);
}