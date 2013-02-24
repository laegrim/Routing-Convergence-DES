#include "packet.h"
#include "router.h"


packet::packet(const int * costs, int originalSender, int sender, int receiver, int packetType, double propDelay) 
	: senderCosts(costs), sender(sender), receiver(receiver), packetType(packetType), timeUntilArrival(propDelay), originalSender(originalSender)
{}

int packet::getPacketType()
{
	return packetType;
}

const int * packet::getCosts()
{
	return senderCosts;
}

int packet::getSender()
{
	return sender;
}

int packet::getReciever()
{
	return receiver;
}

const double packet::getTimeUntilArrival() const
{
	return  timeUntilArrival;
}

bool packet::operator>(const packet & rhs)
{
	return (timeUntilArrival > rhs.timeUntilArrival); 
}

bool packet::operator<(const packet & rhs)
{
	return (timeUntilArrival < rhs.timeUntilArrival);
}

int packet::getOriginalSender()
{
	return originalSender;
}
