#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "router.h"
#include "eventEngine.h"

int getNumNodes(ifstream &, const char *);
vector<router*> & initializeList(int , ifstream &, const char *, vector<router*> & );

int main(int argc, char* argv[]) {


	//arguments should be inputFileName, timeToRunSimulation

	ofstream oFile;
	ifstream iFile;
	int numNodes;

	if (argc != 3)
	{
		cout << "Arguments Missing: programName inputFileName timeToRun" << endl;
		return 0;
	}
	else
	{
		char * PATHNAME = argv[1];
		char * timeToRun = argv[2];

		double timeToRunSimulation = atoi(timeToRun);
		double periodicUpdateTime = 1;

		//const string PATHNAME = "topology3.txt";


		oFile.open("results.txt");
		if (!oFile.is_open())
			cout << "Couldn't open output file" << endl;

		numNodes = getNumNodes(iFile, PATHNAME);
		vector<router*> listOfRouters(numNodes);
		listOfRouters = initializeList(numNodes, iFile, PATHNAME, listOfRouters);
		eventEngine * engine;
	
	
		//for (int i = 0; i < numNodes; i++)
			//listOfRouters[i]->printRoutingTable(oFile);

		engine = new eventEngine(listOfRouters, timeToRunSimulation, periodicUpdateTime);

		//Optional configurations
		//engine->injectPacket(listOfRouters, listOfRouters[0]->getCostVector(), 2, 0, 0, 3, 10);
		//engine->injectPacket(listOfRouters, listOfRouters[0]->getCostVector(), 2, 0, 0, 7, 20);
		//engine->injectPacket(listOfRouters, listOfRouters[0]->getCostVector(), 2, 0, 0, 23, 30);
		//engine->scheduleLinkDown(10.0, 0, 2);
		//engine->scheduleLinkDown(20.0, 5, 7);
		//engine->scheduleLinkDown(30.0, 9, 17);


		engine->startSimulation(listOfRouters, oFile);
		//engine->printStatistics(oFile);

		for (int i = 0; i < numNodes; i++)
			listOfRouters[i]->printRoutingTable(oFile);

		delete engine;
		oFile.close();
		//system("PAUSE");
		return 0;
	}
}

vector<router*> & initializeList(int numNodes, ifstream & iFile, const char * pathname, vector<router*> & listOfRouters) 
{
	string input;
	int pos, len;

	int source, destination, cost;
	double delay;

	for (int i = 0; i < numNodes; i++)
	{
		listOfRouters[i] = new router(i, numNodes);
	}

	iFile.open(pathname);

	if (iFile.is_open())
	{
		for (int i = 0; !iFile.eof(); i++)
		{
			getline(iFile, input);

			string temp;

			len = input.length();
			pos = input.find_first_of('	', 0);
			temp = input.substr(0, pos);
			input = input.substr(pos + 1, len);

			source = atoi(temp.c_str());

			len = input.length();
			pos = input.find_first_of('	');
			temp = input.substr(0, pos);
			input = input.substr(pos + 1, len);

			destination = atoi(temp.c_str());

			len = input.length();
			pos = input.find_first_of('	');
			temp = input.substr(0, pos);
			input = input.substr(pos + 1, len);

			cost = atoi(temp.c_str());

			len = input.length();
			pos = input.find_first_of('	');
			temp = input.substr(0, pos);

			delay = atof(temp.c_str());

			listOfRouters[source]->addVector(destination, cost, source, delay);
			listOfRouters[destination]->addVector(source, cost, destination, delay);
			
		}
	}
	else
		cout << "couldn't open file" << endl;

	return listOfRouters;
}


int getNumNodes(ifstream & iFile, const char * pathName)
{

	iFile.open(pathName);
	int pos, len, largest = 0;
	int source, destination;
	string input;

	if (iFile.is_open())
	{
		for (int i = 0; !iFile.eof(); i++) 
		{
			getline(iFile, input);

			string temp;

			len = input.length();
			pos = input.find_first_of('	', 0);
			temp = input.substr(0, pos);
			input = input.substr(pos + 1, len);

			source = atoi(temp.c_str());

			len = input.length();
			pos = input.find_first_of('	');
			temp = input.substr(0, pos);

			destination = atoi(temp.c_str());

			if (source > largest)
				largest = source;
			else if (destination > largest)
				largest = destination;
		}
	}
	else 
		cout << "couldn't open file";

	iFile.close();

	return largest + 1;
}