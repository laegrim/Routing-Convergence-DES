#include "router.h"

#ifndef PACKET_H
#define PACKET_H

class packet {
private:
	const int * senderCosts;
	int originalSender;
	int sender;
	int receiver;
	int packetType;
	double timeUntilArrival;

public:
	packet(const int * costs,int originalSender, int sender, int receiver, int packetType, double propDelay); //packetType 0, periodic, 1, triggered, 2, data
	int getPacketType();
	int getOriginalSender();
	int getSender();
	int getReciever();
	const int * getCosts();
	const double getTimeUntilArrival() const;
	bool operator>(const packet & rhs);
	bool operator<(const packet & rhs);
};
#endif