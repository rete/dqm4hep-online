/// \file dqm4hep-convert-configuration.cc
/*
 *
 * dqm4hep-convert-configuration.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2016
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
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/ConfigurationManager.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  dqm4hep::core::DQM4HEP::screenSplash();

  std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);
  std::string log4cxx_file = std::string(DQMCore_DIR) + "/conf/defaultLoggerConfig.xml";

  TCLAP::ValueArg<std::string> inputArg(
      "i"
      , "input"
      , "The input configuration"
      , true
      , ""
      , "string");
  pCommandLine->add(inputArg);

  TCLAP::ValueArg<std::string> outputArg(
      "o"
      , "output"
      , "The output configuration"
      , true
      , ""
      , "string");
  pCommandLine->add(outputArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  log4cxx::xml::DOMConfigurator::configure(log4cxx_file);
  dqm4hep::core::dqmMainLogger->setLevel( log4cxx::Level::toLevel( "INFO" ) );

  std::string input( inputArg.getValue() );
  std::string output( outputArg.getValue() );

  dqm4hep::core::ConfigurationManager cfgManager;
  std::string inputIoType(cfgManager.guessIOTypeFromInput(input));
  std::string outputIoType(cfgManager.guessIOTypeFromInput(output));

  THROW_RESULT_IF(dqm4hep::core::STATUS_CODE_SUCCESS, !=, cfgManager.readSettings(inputIoType, input));
  THROW_RESULT_IF(dqm4hep::core::STATUS_CODE_SUCCESS, !=, cfgManager.writeSettings(outputIoType, output));

  return 0;
}
