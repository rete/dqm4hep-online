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
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/Logging.h>

using namespace dqm4hep::core;
using namespace dqm4hep::net;

namespace dqm4hep {

  namespace online {

    RunControlServer::RunControlServer()
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    RunControlServer::~RunControlServer()
    {
      if( m_pServer )
        delete m_pServer;
      
      if( m_pInterface )
        delete m_pInterface;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::setName(const std::string &name)
    {
      m_runControl.setName(name);
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::setPassword(const std::string &pwd)
    {
      m_runControl.setPassword(pwd);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setInterface(const std::string &name)
    {
      m_interfaceName = name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setUserParameters(const dqm4hep::core::StringMap &parameters)
    {
      m_userParameters = parameters;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    RunControl &RunControlServer::runControl()
    {
      return m_runControl;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::run()
    {
      m_stopFlag = false;
            
      // create user external interface (plugin)
      m_pInterface = PluginManager::instance()->create<RunControlInterface>(m_interfaceName);
      
      if( ! m_pInterface )
      {
        delete m_pServer;
        dqm_error( "Couldn't find run control interface '{0}' in plugin manager", m_interfaceName );
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
      }
      
      m_pInterface->setServer(this);      
      m_pInterface->readSettings(m_userParameters);
      
      // create network interface
      std::string baseName = "/dqm4hep/RunControl/" + m_runControl.name() + "/";

      m_pServer = new dqm4hep::net::Server(m_runControl.name());

      m_pSorService = m_pServer->createService(baseName + "SOR");
      m_runControl.onStartOfRun().connect(this, &RunControlServer::sor);

      m_pEorService = m_pServer->createService(baseName + "EOR");
      m_runControl.onEndOfRun().connect(this, &RunControlServer::eor);

      m_pServer->createRequestHandler(baseName + "Status", this, &RunControlServer::sendCurrentRun);
      
      // start the server
      m_pServer->start();
      
      // run the user plugin
      if(m_pInterface->runBlocking())
      {
        // block here until stop called from outside
        m_pInterface->run();
      }
      else
      {
        // call run and sleep
        m_pInterface->run();
        
        while( ! m_stopFlag )
          dqm4hep::core::sleep(dqm4hep::core::TimeDuration(1));
      }
      
      // exit the server : stop and release memory
      m_pServer->stop();
      
      delete m_pServer; m_pServer = nullptr;
      delete m_pInterface; m_pInterface = nullptr;      
      m_pSorService = nullptr;
      m_pEorService = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::stop()
    {
      m_pInterface->stop();
      m_stopFlag = true;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sor(dqm4hep::core::Run &run)
    {
      Json::Value jsonRun;
      run.toJson(jsonRun);

      Json::StreamWriterBuilder builder;
      builder["indentation"] = "  ";
      std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
      std::ostringstream jsonRunStr;
      writer->write(jsonRun, &jsonRunStr);

      m_pSorService->send(jsonRunStr.str());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::eor(const dqm4hep::core::Run &run)
    {
      Json::Value jsonRun;
      run.toJson(jsonRun);

      Json::StreamWriterBuilder builder;
      builder["indentation"] = "  ";
      std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
      std::ostringstream jsonRunStr;
      writer->write(jsonRun, &jsonRunStr);

      m_pEorService->send(jsonRunStr.str());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sendCurrentRun(const Buffer &request, Buffer &response)
    {
      Json::Value jsonStatus, jsonRun;
      m_runControl.currentRun().toJson(jsonRun);
      
      jsonStatus["running"] = m_runControl.isRunning();
      jsonStatus["run"] = jsonRun;

      Json::StreamWriterBuilder builder;
      builder["indentation"] = "  ";
      std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
      std::ostringstream jsonStatusStr;
      writer->write(jsonStatus, &jsonStatusStr);

      auto model = response.createModel<std::string>();
      std::string jsonStatusStr2(jsonStatusStr.str());
      model->move(std::move(jsonStatusStr2));
      response.setModel(model);
    }

  }

}