#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include "../tools/Types.hpp"
#include "../OS/Task.h"
#include "../OS/Scheduler.h"
#include <enet/enet.h>
#include <string>
//#include <iostream>
#include "../tools/Log.hpp"

class Client : public Task
{
	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;
	ENetHost* client;
public:
	Client():peer(NULL)
	{
		if (enet_initialize() == 0)
		{
			if (setupPeer() == STATUS_OK)
			{
				pSched = Scheduler::getInstance();
				this->sleepable = false;
				pSched->registerTask(this, 10);
			}
		}
		else
		{
			Log::error("Error initializing ENET");
		}
	}

	status setupPeer()
	{

		client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			mUpstreamCh0 /* 56K modem with 56 Kbps downstream bandwidth */,
			mUpstreamCh1 /* 56K modem with 14 Kbps upstream bandwidth */);

		if (client == NULL)
		{
			Log::error("An error occurred while trying to create an ENet client host.");
			
			return STATUS_KO;
		}

		/* Connect to some.server.net:1234. */
		enet_address_set_host(&address, mServerAddress.c_str());
		address.port = mServerPort;

		/* Initiate the connection, allocating the two channels 0 and 1. */
		peer = enet_host_connect(client, &address, 2, 0);

		if (peer == NULL)
		{
			Log::error("No available peers for initiating an ENet connection.");
			
			return STATUS_KO;
		}

		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(client, &event, 20000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			Log::info("Connection to some.server.net:1234 succeeded.");
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(peer);

			Log::error("Connection to some.server.net:1234 failed.");

			return STATUS_KO;
		}

		return STATUS_OK;
	}

	void taskRun()
	{
		ENetPacket* packet;

		/* Create a reliable packet of size 7 containing "packet\0" */
		packet = enet_packet_create("backet",
			strlen("backet") + 1,
			ENET_PACKET_FLAG_RELIABLE);

		/* Extend the packet so and append the string "foo", so it now */
		/* contains "packetfoo\0"                                      */
		enet_packet_resize(packet, strlen("backetfoo") + 1);
		strcpy((char*)& packet->data[strlen("backet")], "foo");

		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(event.peer, 0, packet);
		/* One could just use enet_host_service() instead. */
		enet_host_flush(client);

		while (true)
		{
			/* Wait up to 1000 milliseconds for an event. */
			while (enet_host_service(client, &event, 1000) > 0)
			{
				ENetPacket * packet;

				switch (event.type)
				{
				case ENET_EVENT_TYPE_RECEIVE:
					Log::info("A packet of length %u containing %s was received from %s on channel %u.",
						event.packet->dataLength,
						event.packet->data,
						event.peer->data,
						event.channelID);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);

					break;
				}
			}
		}
	}

	~Client()
	{
		atexit(enet_deinitialize);
	}
private:
	Scheduler* pSched;
	std::string mServerAddress = "192.168.2.13";
	int mServerPort = 1721;
	int mUpstreamCh0 = 57600 / 8; /* 56K modem with 56 Kbps downstream bandwidth */
	int mUpstreamCh1 = 14400 / 8; /* 56K modem with 14 Kbps upstream bandwidth */
};


class NetworkManager
{
public:
	NetworkManager() {}
	~NetworkManager() {}

	status initialize();

private:

};

#endif