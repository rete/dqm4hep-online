  /// \file DQMDimRunControlClient.h
/*
 *
 * DQMDimRunControlClient.h header template automatically generated by a class generator
 * Creation date : mer. nov. 12 2014
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


#ifndef DQMDIMRUNCONTROLCLIENT_H
#define DQMDIMRUNCONTROLCLIENT_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMRunControlClient.h"
#include "dqm4hep/DQMDataStream.h"

// -- dim headers
#include "dic.hxx"

namespace dqm4hep
{

/** DQMRunControlClient class
 */ 
class DQMDimRunControlClient : public DimClient, public DQMRunControlClient
{
public:
	/** Constructor
	 */
	DQMDimRunControlClient();

	/** Destructor
	 */
	~DQMDimRunControlClient();

	/** Connect to the service.
	 */
	StatusCode connectToService();

	/** Disconnect from the service
	 */
	StatusCode disconnectFromService();

	/** Whether the client is connected to the service
	 */
	bool isConnectedToService() const;

protected:
	/** Dim infod handler
	 */
	void infoHandler();

	/** Set the run control name only if the service hasn't been started yet
	 */
	StatusCode setName(const std::string &name);

	bool             m_isConnected;          ///< Whether the client is connected to the server instance
	DQMDataStream    m_dataStream;           ///< The data stream to deserialize the run at start of run
	DimInfo          *m_pStartOfRunInfo;     ///< The dim start of run info
	DimInfo          *m_pEndOfRunInfo;       ///< The dim end of run info
}; 

} 

#endif  //  DQMRUNCONTROLCLIENT_H
