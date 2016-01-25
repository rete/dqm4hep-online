  /// \file DQMDimEventClient.h
/*
 *
 * DQMDimEventClient.h header template automatically generated by a class generator
 * Creation date : mar. sept. 8 2015
 *
 * This file is part of DQM4HEP libraries.
 * 
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef DQMDIMEVENTCLIENT_H
#define DQMDIMEVENTCLIENT_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMEventClientImp.h"
#include "dqm4hep/DQMStreamer.h"
#include "dqm4hep/DQMDataStream.h"

#include "dic.hxx"

namespace dqm4hep
{

class DQMDimEventClient;

/** DimEventRpcInfo class
 */
class DimEventRpcInfo : public DimRpcInfo
{
	friend class DQMDimEventClient;

private:
	/** Constructor
	 */
	DimEventRpcInfo(DQMDimEventClient *pEventClient);

	/** Dim handler of rpc info
	 */
	void rpcInfoHandler();

	DQMDimEventClient *m_pEventClient;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

/** DQMDimEventClient class
 */ 
class DQMDimEventClient : public DQMEventClientImp, public DimClient
{
	friend class DimEventRpcInfo;

public:
	/** Constructor
	 */
	DQMDimEventClient();

	/** Destructor
	 */
	~DQMDimEventClient();

	/** Set the collector to connect to.
	 *  Can set the name only if client not yet connected
	 */
	StatusCode setCollectorName(const std::string &collectorName);

	/** Get the collector name
	 */
	const std::string &getCollectorName() const;

	/** Connect to the collector service (server)
	 */
	StatusCode connectToService();

	/** Disconnect the client from the collector (server)
	 */
	StatusCode disconnectFromService();

	/** Whether the client is connected to the collector (server)
	 */
	bool isConnectedToService() const;

	/** Send an event to the collector (server).
	 *  Possible only if a connection has been created (connectToService())
	 *  and an event streamer set.
	 */
	StatusCode sendEvent(const DQMEvent *pEvent);

	/** Set the streamer that will stream in/out the sent/received event(s).
	 *  No default streamer provided. User must provide one before querying/sending
	 *  events from/to collector (server)
	 */
	StatusCode setEventStreamer(DQMEventStreamer *pEventStreamer);

	/** Get the event streamer
	 */
	DQMEventStreamer *getEventStreamer() const;

	/** Set the maximum queue size that stores the received events
	 */
	StatusCode setMaximumQueueSize(unsigned int queueSize);

	/** Clear the event queue
	 */
	StatusCode clearQueue();

	/** Set the sub event identifier.
	 *  This string is sent while querying events.
	 *  The received event will be a sub event matching this identifier
	 */
	void setSubEventIdentifier(const std::string &identifier);

	/** Get the sub event identifier.
	 */
	const std::string &getSubEventIdentifier() const;

	/** Query an event to the collector (server) with a timeout
	 *  and handle it without pushing it into the internal queue
	 */
	StatusCode queryEvent(DQMEvent *&pEvent, int timeout);

	/** Query an event to the collector.
	 *
	 *  A command is send to the collector in order to send back an event.
	 *  This method does not wait for the event reception.
	 *
	 *  To query a single event that is directly handled by the caller,
	 *  use querySingleEvent().
	 *
	 *  The received event is pushed in an internal event queue.
	 */
	StatusCode queryEvent();

	/** Take an event from the event queue (pop front) and return the
	 *  pointer to the caller. The event is removed from the queue,
	 *  meaning that the caller is responsible for the event deletion.
	 */
	StatusCode takeEvent(DQMEvent *&pEvent);

	/** Set the update mode.
	 *  If the update mode is set to true, a command
	 *  is sent to the server in order to update the client
	 *  as soon as an event is received in the collector server.
	 */
	void setUpdateMode(bool updateMode);

	/** Whether the update mode is set
	 */
	bool updateMode() const;

private:

	/** Add an event to the queue.
	 *  Call back method of the event dim rpc info handler
	 */
	StatusCode addEvent(DQMEvent *pEvent);

	/** Dim info handler
	 */
	void infoHandler();


	// collector info
	bool                         m_isConnected;
	std::string                   m_collectorName;

	DimEventRpcInfo              *m_pDimEventRpcInfo;

	// dim infos
	DimUpdatedInfo               *m_pClientIdInfo;       ///< to receive the client id stored on server
	DimUpdatedInfo               *m_pServerStateInfo;   ///< Server state. 0 for stopped : 1 for running
	DimUpdatedInfo               *m_pEventUpdateInfo;   ///< For event reception

	// internal
	DQMEventQueue                 m_eventQueue;
	unsigned int                m_maximumQueueSize;
	DQMEventStreamer             *m_pEventStreamer;
	DQMDataStream                 m_dataStream;

	// client information on server
	int                          m_serverClientId;
	bool                         m_updateMode;
	std::string                   m_subEventIdentifier;

	// thread mutex
	mutable pthread_mutex_t      m_mutex;
}; 

} 

#endif  //  DQMDIMEVENTCLIENT_H
