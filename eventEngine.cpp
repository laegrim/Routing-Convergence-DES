#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "eventEngine.h"
#include "router.h"
#include "packet.h"
using namespace std;

eventEngine::eventEngine(const vector<router*> & listOfRouters, double timeToRunSimulation = -1, double perodicUpdateTime = 1) 
	: currentTime(0), timeToRun(timeToRunSimulation), periodicUpdateTime(perodicUpdateTime)
{
	int numRouters = listOfRouters.size();

	firstLink = new double[numRouters];
	secondLink = new double[numRouters];

	for(int i = 0; i < numRouters; i++)
	{
		firstLink[i] = -1.0;
		secondLink[i] = -1.0;
	}

	//for each router
	for(int i = 0; i < numRouters; i++)
	{
		//check the direct neighbors
		for (int j = 0; j< numRouters; j++)
		{
			if (listOfRouters[i]->getNextHop(j) != -1 && i != j)
			{
				//send an update to the neighbors
				packet * temp = new packet(listOfRouters[i]->getCostVector(), i, i, j, 1, listOfRouters[i]->getDelay(j));
				eventPriorityQueue.push(temp);
			}
		}
	}
}

//Prints the contents of the queue, but empties it in the process
void eventEngine::printStatistics(ofstream & oFile)
{
	packet * temp;
	int size = eventPriorityQueue.size();
	oFile << "Event Queue: " << size << " Events " << endl;
	oFile << "*********************************************" << endl;
	oFile << endl;

	for (int i = 0; i < size; i++)
	{
		temp = eventPriorityQueue.top();
		oFile << "Packet " << i << ": " << endl;
		oFile << "Packet Type: " << temp->getPacketType() << endl;
		oFile << "Time Until Packet Reaches Destination: " << temp->getTimeUntilArrival() << endl;
		oFile << "Original Sender: " << temp ->getOriginalSender() << endl;
		oFile << "Sender: " << temp->getSender() << endl;
		oFile << "Reciever: " << temp->getReciever() << endl;
		oFile << "------------------------------------------------------" << endl;
		eventPriorityQueue.pop();
	}
}

//Loops through event queue processing events
void eventEngine::startSimulation(vector<router*> listOfRouters, ofstream & oFile)
{
	bool endSimulation = false;
	double timeLastTriggered = 0;
	int counter = 0;

	#define SENDING_ROUTER_NEXT_HOP_TO_RECIEVER listOfRouters[temp->getSender()]->getNextHop(temp->getReciever())
	
	//Loop through the queue of events
	while (endSimulation == false)
	{
		packet * temp;
		temp = eventPriorityQueue.top();

		//update current time to reflect new event
		if (currentTime < temp->getTimeUntilArrival())
			currentTime = currentTime + (temp->getTimeUntilArrival() - currentTime);

		//Process the Packet
		switch (temp->getPacketType())
		{
		case 2: 

			//If packet has not yet reached it's destination, forward it
			if (temp->getSender() != SENDING_ROUTER_NEXT_HOP_TO_RECIEVER)
			{
				//If packet the next hop is -1 (indicating that a route doesn't exist) log a dead end
				if ((SENDING_ROUTER_NEXT_HOP_TO_RECIEVER == -1) || !testLink(temp))
				{
					logEvents(listOfRouters, oFile, temp, counter, true);
					eventPriorityQueue.pop();
					break;
				}

				//Packet has successfully reached an interem router, log this
				logEvents(listOfRouters, oFile, temp, counter, false);

				//Send packet to next stop towards it's final destination
				packet * temp2 = new packet(temp->getCosts(), 
					temp->getOriginalSender(), 
					SENDING_ROUTER_NEXT_HOP_TO_RECIEVER, 
					temp->getReciever(), 2, //currentTime + delay from next sender to next senders next hop
					(currentTime + listOfRouters[SENDING_ROUTER_NEXT_HOP_TO_RECIEVER]->getDelay(listOfRouters[(SENDING_ROUTER_NEXT_HOP_TO_RECIEVER)]->getNextHop(temp->getReciever())))); 

				eventPriorityQueue.pop();
				eventPriorityQueue.push(temp2);
			}
			//packet has reached it's destination, 
			else
			{
				//If packet reaches node and finds that the connection been severed during travel time
				if(listOfRouters[temp->getReciever()]->getCost(temp->getSender()) == -1 || !testLink(temp))
				{
					logEvents(listOfRouters, oFile, temp, counter, true);
				}
				else
				{
					//data packet successfully reached it's destination
					logEvents(listOfRouters, oFile, temp, counter, false);
				}
					eventPriorityQueue.pop();
			}
			break;
		default:
			//Since these are triggered updates, they will never need to be forwarded as they are always destined for immediate neighbors
			//If the packet reaches the node (connection wasn't severed while package was in route), merge the tables
			if(listOfRouters[temp->getReciever()]->getCost(temp->getSender()) != -1)
			{
				if (testLink(temp))
				{
					logEvents(listOfRouters, oFile, temp, counter, false);

					if(merge(listOfRouters, temp))
					{
						//If tables were changed, send updates to all neighbor nodes
						int numRouters = listOfRouters.size();
						int currentNode = temp->getReciever();

						for (int j = 0; j< numRouters; j++)
						{
							//If router is not the one who sent the packet, and is not yoursef
							if (j != temp ->getSender() && j != currentNode)
							{
								//If router is a neighbor
								if (listOfRouters[currentNode]->getNextHop(j) == currentNode)
								{
									//send an update to the neighbors
									packet * temp2 = new packet(listOfRouters[currentNode]->getCostVector(), currentNode, currentNode, j, 1, currentTime + listOfRouters[currentNode]->getDelay(j));
									eventPriorityQueue.push(temp2);
								}
							}
						}

					}

					//Each node keeps track of the last periodic update to detect down links
					if(temp->getPacketType() == 0)
						listOfRouters[temp->getReciever()]->setLastPeriodicUpdateRecieved(temp->getSender(), currentTime);

					//eventEngine keeps track of the last triggered update so that it can shut down after convergence if no other time is specified
					if (temp->getPacketType() == 1)
					timeLastTriggered = currentTime;
				}
				else
				{
					logEvents(listOfRouters, oFile, temp, counter, true);
				}
			}
			else
			{
				logEvents(listOfRouters, oFile, temp, counter, true);
			}
			
			eventPriorityQueue.pop();

			break;
		}

		// check conditions for ending the simulation
		if ((currentTime - timeLastTriggered) > (2.0) && timeToRun == -1)
			endSimulation = true;
		else if (currentTime > timeToRun && timeToRun != -1)
			endSimulation = true;
		else if (eventPriorityQueue.empty())
			endSimulation = true;

		int size = listOfRouters.size();

		// see if a route has gone down
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{	// if a router is a neighbor and it's been over twice the periodic update time since that router has sent a periodic update, tell the link that it's down
				if ((listOfRouters[i]->getNextHop(j) == i) && (i != j ) && (currentTime - listOfRouters[i]->getLastPeriodicUpdateRecieved(j) > 2))
				{
					//update the table of the router that detected the downed link to reflect this new status
					listOfRouters[i]->setTimeLastLinkDropped(currentTime);
					listOfRouters[i]->addVector(j, -1, -1, 0);

					for (int k = 0; k < size; k++)
					{
						//update the table of the router that detected the link break to no longer use the disconnected router as a next hop
						if (listOfRouters[i]->getNextHop(k) == j)
							listOfRouters[i]->addVector(k, -1, -1, 0);
					}

					//Now that there's been a change to the table, send out triggered updates
					for (int k = 0; k < size; k++)
					{
						//If router is not the one on the other side of the down link, and is not yoursef
						if ((k != j) && (k != i))
						{
							//If router is a neighbor
							if (listOfRouters[i]->getNextHop(k) == i)
							{
								//send an update to the neighbors
								packet * temp2 = new packet(listOfRouters[i]->getCostVector(), i, i, k, 1, currentTime + listOfRouters[i]->getDelay(k));
								eventPriorityQueue.push(temp2);
							}
						}
					}
				}
			}
		}

		perodicUpdateScheduler(listOfRouters, false);
		counter++;
	}
}

//updates the routing tables of routers with new information based on incoming packets
bool eventEngine::merge(vector<router*> listOfRouters, packet * update)
{
	bool changed = false;
	const int * costs;
	costs = update->getCosts();

	int size = listOfRouters.size();
	int dest = update->getReciever();

	for (int i = 0; i < size; i++)
	{   
		if (costs[i] != -1)
		{
			//     If the given router doesn't already have a route  or the adverstised path + cost to advertiser is cheaper than the current path
			if ((listOfRouters[dest]->getCost(i) == -1 && testLink(update)) || ((costs[i] + listOfRouters[dest]->getCost(update->getSender())) < listOfRouters[dest]->getCost(i)))
			{
				//If the time since the link drop was detected hasn't been greater than the periodic update period, ignore any potential changes by periodic updates
				if ((update->getPacketType() > 0) || (update->getTimeUntilArrival() - listOfRouters[dest]->getTimeLastLinkDropped() > 1.0)) 
				{
					//new route is cheaper than current route in router, update to reflect this new information
					listOfRouters[dest]->addVector(i, costs[i] + listOfRouters[dest]->getCost(update->getSender()), update->getSender(), listOfRouters[dest]->getDelay(update->getSender()));
					changed = true;
				}
			}
		}
		else if (costs[i] == -1)
		{
			//if the packet signals that the sending router doesn't have link to a given router
			for (int i = 0; i < size; i++)
			{
				//check to see if the current router utilizes the path now indicated as not existing, if so, update to reflect that this path doesn't work
				if ((listOfRouters[dest]->getNextHop(i) == update->getSender()) && (listOfRouters[dest]->getCost(i) != -1))
					listOfRouters[dest]->addVector(i, -1, -1, 0);

				changed = true;
			}
		}
	}

	return changed;
}

//pushes a data packet into the priority queue
void eventEngine::injectPacket(const vector<router *> listOfRouters, int * costs, int packetType, int originalSender, int sender, int receiver, double timeSent)
{
	packet * temp = new packet(costs, originalSender, sender, receiver, packetType, timeSent);
	eventPriorityQueue.push(temp);
}

//Provides detailed logging of which packets arrive when and where
void eventEngine::logEvents(const vector<router*> listOfRouters, ofstream & oFile, packet * eventToBeLogged, int counter, bool deadEnd)
{
	//Condition to exclude logging of periodic packets, as they form 
	//the bulk of packets logged and can be a pain to sort through for useful information
	if (eventToBeLogged->getPacketType() == 0)
		return;

	oFile << "At time " << currentTime << " a packet (#" << counter << ") originating from " << eventToBeLogged->getOriginalSender()
		<< " and most recently sent from " << eventToBeLogged->getSender() << " has arrived at ";

	// if the sending router's next hop towards the receiver is not the same as the sending router then the packet is not at the final destination
	if (listOfRouters[eventToBeLogged->getSender()]->getNextHop(eventToBeLogged->getReciever()) != eventToBeLogged->getSender())
		oFile << listOfRouters[eventToBeLogged->getSender()]->getNextHop(eventToBeLogged->getReciever());
	else
		oFile << eventToBeLogged->getReciever();

	oFile << ".  This packet is of type " << eventToBeLogged->getPacketType() << " and had a propagation delay of " 
		<< listOfRouters[eventToBeLogged->getSender()]->getDelay(eventToBeLogged->getReciever())<< endl;
	if (deadEnd == true)
		oFile << "This route has turned into a dead end or the packet was lost" << endl;
}

//inputs the links to be broken, and the time at which they will break;
void eventEngine::scheduleLinkDown(double timeToGoDown, int firstRouter, int secondRouter)
{
	firstLink[firstRouter] = timeToGoDown;
	secondLink[secondRouter] = timeToGoDown;
}

//checks to make sure that packets are not being sent along a broken link
bool eventEngine::testLink(packet * update)
{	
	//if the sender and reciever are routers that have a broken link
	if ((firstLink[update->getSender()] == secondLink[update->getReciever()]) && (firstLink[update->getSender()] != -1))
	{
		//and it is at or past the time at which the link is broken
		if (currentTime >= firstLink[update->getSender()])
		{
			//do not allow the packet to proceed
			return false;
		}
	}
	else if ((secondLink[update->getSender()] == firstLink[update->getReciever()]) && (firstLink[update->getReciever()] != -1))
	{
		if (currentTime >= firstLink[update->getReciever()])
		{
			return false;
		}
	}
	else 
	{
		return true;
	}
}

//Queues the periodic updates at the appropriate times
void eventEngine::perodicUpdateScheduler(vector<router*> listOfRouters, bool initial)
{
	int numRouters = listOfRouters.size();

	//if it's been at least a second since the last periodic update
	if (periodicUpdateTime - currentTime <= 1.0 || initial == true)
	{
		//for each router
		for(int i = 0; i < numRouters; i++)
		{
			//check the direct neighbors
			for (int j = 0; j< numRouters; j++)
			{
				if (listOfRouters[i]->getNextHop(j) != -1 && i != j && listOfRouters[i]->getNextHop(j) == i)
				{
					//send an update to the neighbors
					packet * temp = new packet(listOfRouters[i]->getCostVector(), i, i, j, 0, periodicUpdateTime + listOfRouters[i]->getDelay(j));
					eventPriorityQueue.push(temp);
				}
			}
		}

		periodicUpdateTime++;
	}
}

