/// \file OnlineManagerServer.h
/*
 *
 * OnlineManagerServer.h header template automatically generated by a class generator
 * Creation date : mer. juin 17 2015
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


#ifndef DQM4HEP_ONLINEMANAGERSERVER_H
#define DQM4HEP_ONLINEMANAGERSERVER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Server.h>
#include <dqm4hep/Service.h>
#include <dqm4hep/RequestHandler.h>
#include <dqm4hep/Logger.h>

// -- std headers
#include <atomic>

namespace dqm4hep {

  namespace online {

    /**
     *  @brief  OnlineManagerServer class
     */
    class OnlineManagerServer {
    public:
      /**
       *  @brief  Constructor
       */
      OnlineManagerServer() = default;
      
      /**
       *  @brief  Run the server
       */
      void run();
      
      /**
       *  @brief  Stop the server
       */
      void stop();
      
      /**
       *  @brief  Set the rotating log file properties
       *
       *  @param  fileBaseName the log file base name (.log appended) 
       *  @param  maxFileSize the maximum single file size (unit bytes)
       *  @param  maxNFiles the maximum number of rotating files
       */
      void setLogProperties(const std::string &fileBaseName, size_t maxFileSize, size_t maxNFiles);
      
    private:
      /**
       *  @brief  Collect a log message (server command)
       *  
       *  @param  buffer  the log message as json buffer
       */
      void collectLog(const net::Buffer &buffer);
      
      /**
       *  @brief  Collect an app stat (server command)
       *  
       *  @param  buffer  the app stat as json buffer
       */
      void collectAppStat(const net::Buffer &buffer);
      
      /**
       *  @brief  Log message in log file
       * 
       *  @param  message the json object describing the log message
       */
      void logMessage(const core::json &message);
      
      std::string currentTimeToString() const;
            
    private:
      std::shared_ptr<net::Server>            m_server = {nullptr};           ///< The main server
      net::Service                           *m_logsService = {nullptr};      ///< The logs service
      net::Service                           *m_appStatsService = {nullptr};  ///< The application stats service
      std::atomic<bool>                       m_stopFlag = {false};           ///< The main loop stop flag
      std::string                             m_logFileBaseName = {""};
      std::size_t                             m_logFileMaxSize = {2*1024*1024};
      std::size_t                             m_logFileNFiles = {2};
      core::Logger::LoggerPtr                 m_logger = {nullptr};
    };

  }

} 

#endif  //  DQM4HEP_ONLINEMANAGERSERVER_H