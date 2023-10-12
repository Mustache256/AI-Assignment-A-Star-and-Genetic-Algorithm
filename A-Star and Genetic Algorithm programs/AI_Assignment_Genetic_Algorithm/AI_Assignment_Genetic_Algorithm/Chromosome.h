#pragma once
#include <iostream>
#include <math.h>
#include <vector>
#include <random>

//Chromosome class used in the creation of new chromosomes
class chromosome
{
	//Variable that stores node's current x-axis position
	int posX;
	//Variable that stores node's current y-axis position
	int posY;
	//Variable that stores the lengths of the chromosome
	int chromosomeLength;
	//Vector that stores the binary numbers that make up the genes of the chromosome
	std::vector<int> chromosomeNum;
	//Float varibles that store the fitness of the chromosome, the rate at whci gene crossover occurs when breeding and the rate at which mutation within a chromosome occurs
	float fitness, crossOverRate, mutationRate;

public:
	//Constructor
	chromosome() {};
	//Set Values function to facilitate the creation of the population array
	void setChromosomeValues(int x, int y, int cLength, float coRate, float mRate)
	{
		posX = x; posY = y; chromosomeLength = cLength; crossOverRate = coRate; mutationRate = mRate;
		//For loop that sets chromosome number
		for (int i = 0; i < chromosomeLength; ++i)
		{
			chromosomeNum.push_back(generateGene());
		}
	}

	//Set chromosome's x position
	void setPosX(int x)
	{
		posX = x;
	}

	//Set chromosome's y position
	void setPosY(int y)
	{
		posY = y;
	}

	//Set chromosome number
	void setChromosomeNum(std::vector<int> newNum)
	{
		//For loop that sets chromosome number
		for (int i = 0; i < chromosomeLength; ++i)
		{
			chromosomeNum[i] = newNum[i];
		}
	}

	//Return chromosome's current x position
	int getPosX()
	{
		return posX;
	}

	//Return chromosome's current y position
	int getPosY()
	{
		return posY;
	}

	//Return chromosome's length
	int getChromosomeLength()
	{
		return chromosomeLength;
	}

	//Return 
	float getFitness()
	{
		return fitness;
	}

	//Return chromosome's binary number vector
	std::vector<int> getChromosomeNum()
	{
		return chromosomeNum;
	}

	//Print chromosome number
	void printChromosome()
	{
		//For loop that prints chromosome number
		for (int i = 0; i < chromosomeLength; ++i)
		{
			std::cout << chromosomeNum.at(i);
		}
		"\n";
	}

	//Function that calculates the estimates Euclidian distance of the agent from the maze finish
	const int& distanceFromFinish(const int& destX, const int& destY) const
	{
		static int distanceX, distanceY, totalDistance;
		distanceX = destX - posX;
		distanceY = destY - posY;

		//Euclidian distance calculation
		totalDistance = static_cast<int>(sqrt(distanceX * distanceX + distanceY * distanceY));
		//Returns calculated distance
		return totalDistance;
	}

	//Function that calculates the fitness of a chromosome
	float calculateFitness(const int& destX, const int& destY)
	{
		float distanceX, distanceY;
		distanceX = destX - posX;
		distanceY = destY - posY;

		//Fitness function
		fitness = 1.f / (distanceX + distanceY + 1.f);
		//Return calculated fitness
		return fitness;
	}

	//Function used to generate genes
	int generateGene()
	{
		//Seeding random number generator and assigning distribution variable
		std::random_device randDev;
		std::mt19937 generator(randDev());
		std::uniform_real_distribution<double> distribution(0.0, 1.0);

		int geneNum;
		float rand;
		//Using random distribution to determine whether gene will be a 0 or 1
		rand = distribution(generator);
		//Determine what gene to assign based off of random number
		if (rand <= 0.5)
		{
			geneNum = 0;
		}
		else
		{
			geneNum = 1;
		}
		return geneNum;
	}
};