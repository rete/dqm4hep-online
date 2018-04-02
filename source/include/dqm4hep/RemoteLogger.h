/// \file RemoteLogger.h
/*
 *
 * RemoteLogger.h header template automatically generated by a class generator
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


#ifndef DQM4HEP_REMOTE_LOGGER_H
#define DQM4HEP_REMOTE_LOGGER_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/Logger.h"
#include "dqm4hep/json.h"
#include "dqm4hep/Client.h"
#include "spdlog/sinks/sink.h"

namespace dqm4hep {

  namespace online {
    
    /**
     *  @brief  RemoteLogger class
     */
    class RemoteLogger : public spdlog::sinks::sink {
    public:
      /**
       *  @brief  Constructor
       */
      RemoteLogger();
      
      /**
       *  @brief  Create a shared pointer of RemoteLogger
       */
      static std::shared_ptr<RemoteLogger> make_shared();
      
      /**
       *  @brief  Log a message
       *  
       *  @param  msg the spdlog log message 
       */
      void log(const spdlog::details::log_msg& msg) override;
      
      /**
       *  @brief  Flush the message
       */
      void flush() override;
      
    private:
      core::json     m_message;     ///< The log message to send as json
      std::string    m_hostname;    ///< The current host name
      net::Client    m_client;      ///< The net client interface
    };

  }

} 

#endif  //  DQM4HEP_REMOTE_LOGGER_H
