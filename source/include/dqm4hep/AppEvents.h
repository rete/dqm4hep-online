/// \file AppEvents.h
/*
 *
 * AppEvent.h header template automatically generated by a class generator
 * Creation date : mar. oct. 7 2014
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


#ifndef DQM4HEP_APPEVENTS_H
#define DQM4HEP_APPEVENTS_H

// -- dqm4hep headers
#include "dqm4hep/AppEvent.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/DQMNet.h"

namespace dqm4hep {

  namespace online {

    class QuitEvent : public AppEvent {
    public:
      QuitEvent(int returnCode);
      int returnCode() const;
    private:
      int          m_returnCode;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    class ProcessEventEvent : public AppEvent
    {
    public:
      ProcessEventEvent(std::shared_ptr<core::Event> event);
      std::shared_ptr<core::Event> event() const;
    private:
      std::shared_ptr<core::Event>       m_event;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class StartOfRunEvent : public AppEvent
    {
    public:
      StartOfRunEvent(const core::Run &run);
      const core::Run &run() const;
    private:
      core::Run           m_run;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class EndOfRunEvent : public AppEvent
    {
    public:
      EndOfRunEvent(const core::Run &run);
      const core::Run &run() const;
    private:
      core::Run           m_run;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class ServiceUpdateEvent : public AppEvent
    {
    public:
      ServiceUpdateEvent(const std::string &serviceName, net::BufferModelPtr bufferModel);
      const std::string &serviceName() const;
      const net::Buffer &buffer() const;
    private:
      std::string              m_serviceName;
      net::Buffer              m_buffer;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class RequestEvent : public AppEvent
    {
    public:
      RequestEvent(const std::string &name, net::BufferModelPtr requestBufferModel, net::Buffer &response);
      const std::string &requestName() const;
      const net::Buffer &request() const;
      net::Buffer &response() const;
    private:
      std::string              m_requestName;
      net::Buffer              m_requestBuffer;
      net::Buffer             &m_responseBuffer;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class CommandEvent : public AppEvent
    {
    public:
      CommandEvent(const std::string &commandName, net::BufferModelPtr bufferModel);
      const std::string &commandName() const;
      const net::Buffer &buffer() const;
    private:
      std::string              m_commandName;
      net::Buffer              m_buffer;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class ClientExitEvent : public AppEvent
    {
    public:
      ClientExitEvent(int clientId);
      int clientId() const;
    private:
      int                   m_clientId;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline QuitEvent::QuitEvent(int returnCode) :
      AppEvent(AppEvent::QUIT),
      m_returnCode(returnCode)
    {
      setPriority(100);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline int QuitEvent::returnCode() const
    {
      return m_returnCode;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline ProcessEventEvent::ProcessEventEvent(std::shared_ptr<core::Event> event) :
      AppEvent(AppEvent::PROCESS_EVENT),
      m_event(event)
    {
      setPriority(60);
    }
    
    //-------------------------------------------------------------------------------------------------

    inline std::shared_ptr<core::Event> ProcessEventEvent::event() const
    {
      return m_event;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline StartOfRunEvent::StartOfRunEvent(const core::Run &run) :
      AppEvent(AppEvent::START_OF_RUN)
    {
      setPriority(90);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const core::Run &StartOfRunEvent::run() const
    {
      return m_run;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline EndOfRunEvent::EndOfRunEvent(const core::Run &run) :
      AppEvent(AppEvent::END_OF_RUN)
    {
      setPriority(90);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const core::Run &EndOfRunEvent::run() const
    {
      return m_run;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline ServiceUpdateEvent::ServiceUpdateEvent(const std::string &serviceName, net::BufferModelPtr bufferModel) :
      AppEvent(SERVICE_UPDATE),
      m_serviceName(serviceName),
      m_buffer()
    {
      m_buffer.setModel(bufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------

    inline const std::string &ServiceUpdateEvent::serviceName() const
    {
      return m_serviceName;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const net::Buffer &ServiceUpdateEvent::buffer() const
    {
      return m_buffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline RequestEvent::RequestEvent(const std::string &name, net::BufferModelPtr requestBufferModel, net::Buffer &response) :
      AppEvent(AppEvent::REQUEST_HANDLING),
      m_requestName(name),
      m_requestBuffer(),
      m_responseBuffer(response)
    {
      m_requestBuffer.setModel(requestBufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------

    inline const std::string &RequestEvent::requestName() const
    {
      return m_requestName;
    }

    //-------------------------------------------------------------------------------------------------

    inline const net::Buffer &RequestEvent::request() const
    {
      return m_requestBuffer;
    }
    
    //-------------------------------------------------------------------------------------------------

    inline net::Buffer &RequestEvent::response() const
    {
      return m_responseBuffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline CommandEvent::CommandEvent(const std::string &commandName, net::BufferModelPtr bufferModel) :
      AppEvent(AppEvent::COMMAND_HANDLING),
      m_commandName(commandName)
    {
      m_buffer.setModel(bufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const std::string &CommandEvent::commandName() const
    {
      return m_commandName;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const net::Buffer &CommandEvent::buffer() const
    {
      return m_buffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline ClientExitEvent::ClientExitEvent(int clientId) :
      AppEvent(AppEvent::CLIENT_EXIT),
      m_clientId(clientId)
    {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline int ClientExitEvent::clientId() const
    {
      return m_clientId;
    }

  }

} 

#endif  //  DQM4HEP_APPEVENTS_H
