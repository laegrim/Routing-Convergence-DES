#include "router.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

router::router(int id, int numNodes)
	: id(id), numberNodes(numNodes), destination(new int[numNodes]), cost(new int[numNodes]), nextHop(new int[numNodes]), delay(new double[numNodes]), lastPeriodicUpdateRecieved(new double[numNodes]), timeLastLinkDropped(0)
{
	for (int i=0; i<numNodes; i++)
	{
		if (i == id)
		{
			destination[i] = i;
			cost[i] = 0;
			nextHop[i] = id;
			delay[i] = 0;
			lastPeriodicUpdateRecieved[i] = 0.0;
		}
		else
		{
			destination[i] = i;
			cost[i] = -1;
			nextHop[i] = -1;
			delay[i] = 0;
			lastPeriodicUpdateRecieved[i] = 0.0;
		}
	}
}

router::router(const router & original)
	: id(original.id), numberNodes(original.numberNodes), destination(new int[original.numberNodes]), cost(new int[original.numberNodes]), nextHop(new int[original.numberNodes]), delay(new double[original.numberNodes])
{
	for (int i=0; i<numberNodes; i++)
	{
		destination[i] = original.destination[i];
		cost[i] = original.cost[i];
		nextHop[i] = original.nextHop[i];
		delay[i] = original.delay[i];
		lastPeriodicUpdateRecieved[i] = original.lastPeriodicUpdateRecieved[i];
	}
}

const router & router::operator=(const router & rhs)
{
	if (this != &rhs)
	{
		if (numberNodes != rhs.numberNodes)
		{
			delete [] cost;
			delete [] destination; 
			delete [] nextHop;
			delete [] delay;
			delete [] lastPeriodicUpdateRecieved;

			numberNodes = rhs.numberNodes;

			cost = new int[numberNodes];
			destination = new int[numberNodes];
			nextHop = new int[numberNodes];
			delay = new double[numberNodes];
			lastPeriodicUpdateRecieved = new double[numberNodes];
		}

		for (int i = 0; i < numberNodes; i++)
		{
			cost[i] = rhs.cost[i];
			destination[i] = rhs.destination[i];
			nextHop[i] = rhs.nextHop[i];
			delay[i] = rhs.delay[i];
			lastPeriodicUpdateRecieved[i] = rhs.lastPeriodicUpdateRecieved[i];
		}

		id = rhs.id;
	}

	return * this;
}

router::~router()
{
	delete[] destination;
	delete[] cost;
	delete[] nextHop;
	delete[] delay;
	delete[] lastPeriodicUpdateRecieved;
}

int router::getCost(int destination)
{
	return cost[destination];
}

int router::getNextHop(int destination)
{
	return nextHop[destination];
}

double router::getDelay(int destination)
{
	return delay[destination];
}

void router::addVector(int destination, int costAt, int nextHopAt, double delayAt)
{
	cost[destination] = costAt;
	nextHop[destination] = nextHopAt;
	delay[destination] = delayAt;
}

void router::printRoutingTable(ofstream & oFile)
{
	oFile << "Routing Table for Router ID: " << id << endl;
	oFile << left << setw(15) << "Destination" << setw(15) << left << "Cost" << setw(15) << "Next Hop" << setw(15) << "Delay" << endl;
	oFile << "*********************************************************" << endl;
	for ( int i = 0; i < numberNodes; i++)
		oFile << left << setw(15) << destination[i] << setw(15) << cost[i] << setw(15) << nextHop[i] << setw(15) << delay[i] << endl;
	oFile << "*********************************************************" << endl;
	oFile << endl;
}

void router::displayRoutingTable()
{
	cout << "Routing Table for Router ID: " << id << endl;
	cout << left << setw(15) << "Destination" << setw(15) << left << "Cost" << setw(15) << "Next Hop" << setw(15) << "Delay" << endl;
	cout << "*********************************************************" << endl;
	for ( int i = 0; i < numberNodes; i++)
		cout << left << setw(15) << destination[i] << setw(15) << cost[i] << setw(15) << nextHop[i] << setw(15) << delay[i] << endl;
	cout << "*********************************************************" << endl;
	cout << endl;
}

int * router::getCostVector()
{
	return cost;
}

double router::getLastPeriodicUpdateRecieved(int destination)
{
	return lastPeriodicUpdateRecieved[destination];
}

void router::setLastPeriodicUpdateRecieved(int destination, double updateTime)
{
	lastPeriodicUpdateRecieved[destination] = updateTime;
}

double router::getTimeLastLinkDropped()
{
	return timeLastLinkDropped;
}

void router::setTimeLastLinkDropped(double time)
{
	timeLastLinkDropped = time;
}