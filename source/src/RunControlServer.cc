  /// \file RunControlServer.cc
/*
 *
 * RunControlServer.cc source template automatically generated by a class generator
 * Creation date : ven. oct. 27 2017
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
 * @copyright
 */

// -- dqm4hep headers
#include <dqm4hep/RunControlServer.h>
#include <dqm4hep/RunControlInterface.h>
#include <dqm4hep/OnlineRoutes.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/Logging.h>

using namespace dqm4hep::core;
using namespace dqm4hep::net;

namespace dqm4hep {

  namespace online {

    void RunControlServer::setName(const std::string &name) {
      m_runControl.setName(name);
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::setPassword(const std::string &pwd) {
      m_runControl.setPassword(pwd);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setInterface(const std::string &name) {
      m_interfaceName = name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setUserParameters(const dqm4hep::core::StringMap &parameters) {
      m_userParameters = parameters;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    RunControl &RunControlServer::runControl() {
      return m_runControl;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::run() {
      m_stopFlag = false;
            
      // create user external interface (plugin)
      m_interface = PluginManager::instance()->create<RunControlInterface>(m_interfaceName);
      
      if( not m_interface ) {
        dqm_error( "Couldn't find run control interface '{0}' in plugin manager", m_interfaceName );
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
      }
  
      // create network interface
      m_server = std::make_shared<dqm4hep::net::Server>(m_runControl.name());

      m_pSorService = m_server->createService(OnlineRoutes::RunControl::sor(m_runControl.name()));
      m_runControl.onStartOfRun().connect(this, &RunControlServer::sor);

      m_pEorService = m_server->createService(OnlineRoutes::RunControl::eor(m_runControl.name()));
      m_runControl.onEndOfRun().connect(this, &RunControlServer::eor);

      m_server->createRequestHandler(OnlineRoutes::RunControl::status(m_runControl.name()), this, &RunControlServer::sendCurrentRun);
      
      m_interface->setServer(this);      
      m_interface->readSettings(m_userParameters);
      
      // start the server
      m_server->start();
      
      // run the user plugin
      if(m_interface->runBlocking())
      {
        // block here until stop called from outside
        m_interface->run();
      }
      else
      {
        // call run and sleep
        m_interface->run();
        
        while( ! m_stopFlag )
          dqm4hep::core::sleep(dqm4hep::core::TimeDuration(1));
      }
      
      m_server->stop();
      m_pSorService = nullptr;
      m_pEorService = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::stop()
    {
      if(m_interface) 
        m_interface->stop();
        
      m_stopFlag = true;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sor(dqm4hep::core::Run &r) {
      core::json jsonRun;
      r.toJson(jsonRun);
      m_pSorService->send(jsonRun.dump());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::eor(const dqm4hep::core::Run &r) {
      core::json jsonRun;
      r.toJson(jsonRun);
      m_pEorService->send(jsonRun.dump());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sendCurrentRun(const Buffer &/*request*/, Buffer &response) {
      core::json jsonStatus, jsonRun;
      m_runControl.currentRun().toJson(jsonRun);
      jsonStatus = {
        {"running", m_runControl.isRunning()},
        {"run", jsonRun}
      };
      auto model = response.createModel<std::string>();
      std::string jsonDump(jsonStatus.dump());
      model->move(std::move(jsonDump));
      response.setModel(model);
    }

  }

}
