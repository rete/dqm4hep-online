/// \file RemoteLogger.cc
/*
 *
 * RemoteLogger.cc source template automatically generated by a class generator
 * Creation date : ven. sept. 5 2014
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
 * @copyright 2018
 */

// -- dqm4hep headers
#include "dqm4hep/RemoteLogger.h"
#include "dqm4hep/OnlineRoutes.h"

namespace dqm4hep {

  namespace online {

    RemoteLogger::RemoteLogger() {
      char hname[256];
      gethostname(hname, 256);
      m_hostname = hname;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::Logger::AppenderPtr RemoteLogger::make_shared() {
      return std::make_shared<RemoteLogger>();
    }
        
    //-------------------------------------------------------------------------------------------------
    
    void RemoteLogger::log(const spdlog::details::log_msg& msg) {
      m_message = {
        {"logger",   *msg.logger_name},
        {"level",   msg.level},
        {"message", msg.raw.str()},
        {"host",    m_hostname},
        {"pid",     dqm4hep::core::pid()}
      };
      m_client.sendCommand(OnlineRoutes::OnlineManager::collectLog(), m_message.dump());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RemoteLogger::flush() {      
    }
    
  }

}

