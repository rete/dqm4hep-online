/// \file dqm4hep-start-event-collector.cc
/*
 *
 * dqm4hep-start-event-collector.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
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

// -- dqm4hep headers
#include "dqm4hep/EventCollector.h"
#include "dqm4hep/Logging.h"

std::shared_ptr<dqm4hep::online::EventCollector> application;

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
  dqm_warning( "*** SIGN INT ***" );
  dqm_warning( "Caught signal {0}. Stopping the application.", signal );

  if(application) application->exit(0);
}

//-------------------------------------------------------------------------------------------------

// segmentation violation signal handling
void seg_viol_signal_handling(int signal)
{
  dqm_warning( "*** SIGN VIOL ***" );
  dqm_warning( "Caught signal {0}. Stopping the application.", signal );

  if(application) application->exit(signal);
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  dqm4hep::core::screenSplash();
  
  // install signal handlers
  dqm_info( "Installing signal handlers ..." );
  signal(SIGINT,  int_key_signal_handler);
  // signal(SIGSEGV, seg_viol_signal_handling);
  
  // initialize and run the application
  application = std::make_shared<dqm4hep::online::EventCollector>();
  application->init(argc, argv);
  
  return application->exec();
}
