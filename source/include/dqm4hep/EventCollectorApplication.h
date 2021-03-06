/// \file DQMEventCollectorApplication.h
/*
 *
 * DQMEventCollectorApplication.h header template automatically generated by a class generator
 * Creation date : jeu. juin 18 2015
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


#ifndef DQM4HEP_EVENTCOLLECTORAPPLICATION_H
#define DQM4HEP_EVENTCOLLECTORAPPLICATION_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Application.h"

namespace dqm4hep {

  namespace core {

    class EventCollector;
    class EventCollectorImp;

    /** EventCollectorApplication class
     */
    class EventCollectorApplication : public Application
    {
    public:
      /** Constructor with collector name
       */
      EventCollectorApplication();

      /** Destructor
       */
      ~EventCollectorApplication();

      /** Run the application
       */
      StatusCode run();

      /** Stop the application
       */
      StatusCode exit( int returnCode );

      /** Get the application name
       */
      const std::string &getName() const;

      /** Get the application type
       */
      const std::string &getType() const;

      /** Set the event collector name
       */
      StatusCode setCollectorName(const std::string &collectorName);

      /** Set the event collector implementation
       */
      void setEventCollectorImp(EventCollectorImp *pCollectorImp);

      /** Set the event streamer to serialize/deserialize the in/out-coming events
       */
      void setEventStreamer(EventStreamer *pEventStreamer);

      /** Get the event streamer
       */
      EventStreamer *getEventStreamer() const;

    protected:
      /** No settings to read
       */
      StatusCode readSettings(const std::string &settingsFile = "") { return STATUS_CODE_SUCCESS; }

      bool                               m_shouldExit;
      std::string                        m_type;
      std::string                        m_networkName;
      State                              m_applicationState;
      EventCollector                    *m_pEventCollector;
    };

  }

}

#endif  //  DQM4HEP_EVENTCOLLECTORAPPLICATION_H
