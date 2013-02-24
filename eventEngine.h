#include <queue>
#include "compareGreater.h"
#include <vector>
#include "packet.h"
#include <fstream>

#ifndef EVENTENGINE_H
#define EVENTENGINE_H

using namespace std;

class eventEngine
{
private:
	priority_queue<packet *, vector<packet *>, compareGreater<packet *> > eventPriorityQueue;
	double currentTime;
	double timeToRun;
	double periodicUpdateTime; 
	double * firstLink;
	double * secondLink;


public:

	eventEngine(const vector<router*> & listOfRouters, double timeToRunSimulation, double periodicUpdateTime);
	void startSimulation(vector<router*> listOfRouters, ofstream & oFile);
	void perodicUpdateScheduler(vector<router*> listOfRouters, bool initial);
	void injectPacket(const vector<router*> listOfRouters, int * costs, int packetType, int originalSender, int sender, int receiver, double timeSent);
	void scheduleLinkDown();
	void logEvents(const vector<router*> listOfRouters, ofstream & oFile, packet * eventToBeLogged, int counter, bool deadEnd);
	void printStatistics(ofstream & oFile);
	bool merge(vector<router*> listOfRouters, packet * update);
	void scheduleLinkDown(double timeToGoDown, int firstRouter, int secondRouter);
	bool testLink(packet * update);
};


#endif 