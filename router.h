#include <vector>
#include <fstream>

#ifndef ROUTER_H
#define ROUTER_H

using namespace std;

class router {

	private:
		int id;
		int numberNodes;
		int * destination;
		int * cost;
		int * nextHop;
		double * delay;
		double * lastPeriodicUpdateRecieved;
		double timeLastLinkDropped;


	public:
		router(int id, int numNodes);
		~router();
		router(const router & original);
		const router & operator=(const router & rhs);

		double getTimeLastLinkDropped();
		void setTimeLastLinkDropped(double time);
		double getLastPeriodicUpdateRecieved(int destination);
		void setLastPeriodicUpdateRecieved(int destination, double updateTime);
		int getCost(int destination);
		int * getCostVector();
		int getNextHop(int destination);
		double getDelay(int destination);
		void addVector(int destination, int cost, int nextHop, double delay); 
		void printRoutingTable(ofstream & oFile);
		void displayRoutingTable();
};
#endif