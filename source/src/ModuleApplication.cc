/// \file DQMModuleApplication.cc
/*
 *
 * DQMModuleApplication.cc source template automatically generated by a class generator
 * Creation date : dim. sept. 6 2015
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
#include "dqm4hep/ModuleApplication.h"
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/OnlineElement.h"
#include "dqm4hep/OnlineRoutes.h"
#include "dqm4hep/XmlHelper.h"
#include "DQMOnlineConfig.h"

namespace dqm4hep {

  namespace online {
    
    ModuleApplication::ModuleApplication() :
      m_cycle(m_eventLoop) {
      m_monitorElementManager = std::make_shared<core::MonitorElementManager>();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    ModuleApplication::~ModuleApplication() {
      removeTimer(m_standaloneTimer);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::parseCmdLine(int argc, char **argv) {
      std::string cmdLineFooter = "Please report bug to <dqm4hep@desy.de>";
      m_cmdLine = std::make_shared<CmdLine::element_type>(cmdLineFooter, ' ', DQMOnline_VERSION_STR);

      TCLAP::ValueArg<std::string> steeringFileNameArg(
          "f"
          , "steering-file"
          , "The xml steering file for the module application"
          , true
          , ""
          , "string");
      m_cmdLine->add(steeringFileNameArg);

      core::StringVector verbosities(core::Logger::logLevels());
      TCLAP::ValuesConstraint<std::string> verbosityConstraint(verbosities);
      TCLAP::ValueArg<std::string> verbosityArg(
          "v"
          , "verbosity"
          , "The logging verbosity"
          , false
          , "info"
          , &verbosityConstraint);
      m_cmdLine->add(verbosityArg);
      
      TCLAP::ValueArg<std::string> moduleTypeArg(
          "t"
          , "type"
          , "The module type to run (plugin name). Overwrite the module type from steering file"
          , false
          , ""
          , "string");
      m_cmdLine->add(moduleTypeArg);
      
      TCLAP::ValueArg<std::string> moduleNameArg(
          "n"
          , "name"
          , "The module name to run. Overwrite the module name from steering file"
          , false
          , ""
          , "string");
      m_cmdLine->add(moduleNameArg);

      // TCLAP::MultiArg<std::string> parameterArg(
      //     "p"
      //     , "parameter"
      //     , "A parameter to replace in the application (see XmlHelper)"
      //     , false
      //     , "");
      // m_cmdLine->add(parameterArg);
      
      // parse command line
      m_cmdLine->parse(argc, argv);

      m_moduleType = moduleTypeArg.getValue();
      m_moduleName = moduleNameArg.getValue();
      // setType(OnlineRoutes::ModuleApplication::applicationType());
      setType("module");
      setLogLevel(core::Logger::logLevelFromString(verbosityArg.getValue()));

      parseSteeringFile(steeringFileNameArg.getValue());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onInit() {
      dqm_info( "Module application mode set to {0}", m_mode );
      m_standaloneTimer = createTimer();
      m_standaloneTimer->setInterval(m_standaloneSleep);
      m_standaloneTimer->setSingleShot(true);
      m_standaloneTimer->onTimeout().connect(this, &ModuleApplication::postStandaloneProcess);
      m_cycle.setEventPriority(Priorities::END_OF_CYCLE);
      m_runControl.onStartOfRun().connect(this, &ModuleApplication::setElementsRunNumber);
      m_runControl.onStartOfRun().connect(m_module.get(), &Module::startOfRun);      
      m_runControl.onEndOfRun().connect(m_module.get(), &Module::endOfRun);
      queuedSubscribe(
        OnlineRoutes::RunControl::sor(m_runControl.name()),
        Priorities::START_OF_RUN
      );
      queuedSubscribe(
        OnlineRoutes::RunControl::eor(m_runControl.name()),
        Priorities::END_OF_RUN
      );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onEvent(AppEvent * appEvent) {
      if(AppEvent::SERVICE_UPDATE == appEvent->type()) {
        ServiceUpdateEvent* svc = dynamic_cast<ServiceUpdateEvent*>(appEvent);
        // Start of run
        if(svc->serviceName() == OnlineRoutes::RunControl::sor(m_runControl.name())) {
          processStartOfRun(svc);
        }
        // End of run
        if(svc->serviceName() == OnlineRoutes::RunControl::eor(m_runControl.name())) {
          processEndOfRun();
        }
      }
      if(AppEvent::END_OF_RUN == appEvent->type()) {
        auto eorEvent = dynamic_cast<StoreEvent<core::Run>*>(appEvent);
        auto run = eorEvent->data();
        m_runControl.endCurrentRun(run.parameters());
      }
      // process event received from the event collector
      if(AppEvent::PROCESS_EVENT == appEvent->type() and ModuleApplication::ANALYSIS == m_mode) {
        if(m_runControl.isRunning()) {
          auto procEvent = dynamic_cast<StoreEvent<core::EventPtr>*>(appEvent);
          auto anaModule = moduleAs<AnalysisModule>();
          anaModule->process(procEvent->data());
          m_cycle.incrementCounter();
        }
      }
      // generic loop
      if(AppEvent::PROCESS_EVENT == appEvent->type() and ModuleApplication::STANDALONE == m_mode) {
        auto standModule = moduleAs<StandaloneModule>();
        standModule->process();
        m_cycle.incrementCounter();
        m_standaloneTimer->start();
        // const core::TimePoint now = core::now();
        // const float limit = static_cast<float>(m_standaloneSleep);
        // const float ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now-procEvent->data()).count() / 1000.f;
        // if(ellapsed < limit) {
        //   const unsigned int sleepTime = static_cast<unsigned int>((limit-ellapsed)*1000000);
        //   ::usleep(sleepTime);
        // }
        // auto *processEvent = new StoreEvent<core::TimePoint>(AppEvent::PROCESS_EVENT, core::now()); 
        // processEvent->setPriority(ModuleApplication::PROCESS_CALL);
        // m_eventLoop.postEvent(processEvent);
      }
      // end of cycle
      if(AppEvent::END_OF_CYCLE == appEvent->type()) {
        auto eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(appEvent);
        auto condition = eocEvent->data();
        m_module->endOfCycle(condition);
        if(condition.m_counter > 0) {
          try {
            core::QReportStorage reportStorage;
            // process quality tests
            THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_monitorElementManager->runQualityTests(reportStorage));
            // TODO send monitor element to collector
          }
          catch(core::StatusCodeException &exception) {
            dqm_error( "Error caught at end of cycle: {0}", exception.getStatusCode() );
          }
        }
        // always restart a new cycle for standalone modules
        if(ModuleApplication::STANDALONE == m_mode) {
          m_module->startOfCycle();
          m_cycle.startCycle(true);           
        }
        // forced end is a synonym of end of run for analysis modules
        // do not restart a cycle in this case
        if(ModuleApplication::ANALYSIS == m_mode and not condition.m_forcedEnd) {
          m_module->startOfCycle();
          m_cycle.startCycle(true); 
        }
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onStart() {
      // get run control status in case it is already running
      sendRequest(OnlineRoutes::RunControl::status(m_runControl.name()), net::Buffer(), [this](const net::Buffer &response){
        core::json statusJson = core::json::parse(response.begin(), response.end());
        bool rcRunning = statusJson.value<bool>("running", false);
        if(rcRunning) {
          core::json runJson = statusJson.value<core::json>("run", core::json({}));
          core::Run run;
          run.fromJson(runJson);
          m_runControl.startNewRun(run);
        }
      });
      if(ModuleApplication::STANDALONE == m_mode) {
        m_module->startOfCycle();
        m_cycle.startCycle(true);
      }
      else if(ModuleApplication::ANALYSIS == m_mode and m_runControl.isRunning()) {
        m_module->startOfCycle();
        m_cycle.startCycle(true);
      }
      m_standaloneTimer->start();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onStop() {
      m_standaloneTimer->stop();
    }
    
    //-------------------------------------------------------------------------------------------------
  
    ModulePtr ModuleApplication::module() const {
      return m_module;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ModuleApplication::moduleName() const {
      return m_moduleName;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ModuleApplication::moduleType() const {
      return m_moduleType;
    }

    //-------------------------------------------------------------------------------------------------

    ModuleApplication::Mode ModuleApplication::mode() const {
      return m_mode;
    }

    //-------------------------------------------------------------------------------------------------

    const RunControl& ModuleApplication::runControl() const {
      return m_runControl;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool ModuleApplication::allowBooking() const {
      return m_allowBooking;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::shared_ptr<core::MonitorElementManager> ModuleApplication::monitorElementManager() const {
      return m_monitorElementManager;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::parseSteeringFile(const std::string &fname) {
      m_parser.parse(fname);
      
      auto document = m_parser.document();
      const core::TiXmlHandle xmlHandle(document.RootElement());
      
      auto qtestsElement = xmlHandle.FirstChildElement("qtests").Element();
      if(qtestsElement) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_monitorElementManager->createQualityTests(qtestsElement));
      }
      
      auto moduleElement = xmlHandle.FirstChildElement("module").Element();
      configureModule(moduleElement);
      
      auto settingsElement = xmlHandle.FirstChildElement("settings").Element();
      configureCycle(settingsElement);
      configureNetwork(settingsElement);
      
      core::TiXmlHandle settingsHandle(settingsElement);
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND,!=, core::XmlHelper::readParameter(settingsHandle, "StandaloneSleepTime", m_standaloneSleep));
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureModule(core::TiXmlElement *element) {
      if(m_moduleType.empty()) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::getAttribute(element, "type", m_moduleType));
      }
      if(m_moduleName.empty()) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::getAttribute(element, "name", m_moduleName));
      }
      if(m_moduleType.empty() or m_moduleName.empty()) {
        dqm_error( "Undefined module type and/or module name (type: {0}, name: {1}).", m_moduleType, m_moduleName );
        throw core::StatusCodeException(core::STATUS_CODE_FAILURE);
      }
      setName(m_moduleName);
      m_module = core::PluginManager::instance()->create<Module>(m_moduleType);
      if(nullptr == m_module) {
        dqm_error( "Module '{0}' not registered in plugin manager!", m_moduleType );
        dqm_error( "Please check your plugin settings (libraries, environment) and restart the application" );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      if(nullptr != dynamic_cast<AnalysisModule*>(m_module.get())) {
        m_mode = ANALYSIS;
      }
      else if(nullptr != dynamic_cast<StandaloneModule*>(m_module.get())) {
        m_mode = STANDALONE;
      }
      else {
        dqm_error( "Undefined module type, must be AnalysisModule or StandaloneModule" );
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      m_module->setModuleApplication(this);
      const core::TiXmlHandle moduleHandle(element);
      
      m_allowBooking = true;
      m_module->readSettings(moduleHandle);
      m_module->initModule();
      m_allowBooking = false;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureCycle(core::TiXmlElement *element) {
      unsigned int timeout = 10;
      unsigned int period = 30;
      unsigned int counterLimit = 0;
      if(nullptr != element) {
        core::TiXmlHandle handle(element);
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CycleTimeout", timeout));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CyclePeriod", period));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CycleCounter", counterLimit));
      }
      if((timeout >= period) || (0 == period && 0 == counterLimit)) {
        dqm_error( "Invalid cycle settings: period={0}, timeout={1}, counter={2}", period, timeout, counterLimit );
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      dqm_info( "== Cycle settings ==" );
      dqm_info( "=> Period:  {0}", period );
      dqm_info( "=> Timeout: {0}", timeout );
      dqm_info( "=> Counter: {0}", counterLimit );
      dqm_info( "====================" );
      m_cycle.setTimeout(timeout);
      m_cycle.setTimerPeriod(period);
      m_cycle.setCounterLimit(counterLimit);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureNetwork(core::TiXmlElement *element) {
      core::TiXmlHandle handle(element);
      
      std::string runControlName;
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "RunControl", runControlName));
      m_runControl.setName(runControlName);
      
      if(m_mode == ModuleApplication::ANALYSIS) {
        std::string eventCollector;
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventCollector", eventCollector));
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventSource", m_eventSourceName));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "EventQueueSize", m_eventQueueSize));
        
        m_eventCollectorClient = std::make_shared<EventClientPtr::element_type>(eventCollector);
        m_eventCollectorClient->onEventUpdate(m_eventSourceName, this, &ModuleApplication::receiveEvent);        
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::processStartOfRun(ServiceUpdateEvent *svc) {
      core::json runJson = core::json::parse(svc->buffer().begin(), svc->buffer().end());
      core::Run run;
      run.fromJson(runJson);
      dqm_info( "Starting new run {0}", run.runNumber() );
      m_runControl.startNewRun(run);
      if(ModuleApplication::ANALYSIS == m_mode) {
        m_module->startOfCycle();
        m_cycle.startCycle();
      }
      if(ModuleApplication::ANALYSIS == m_mode) {
        m_eventCollectorClient->startEventUpdates(m_eventSourceName);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::processEndOfRun() {
      if(ModuleApplication::ANALYSIS == m_mode) {
        m_eventCollectorClient->stopEventUpdates(m_eventSourceName);
      }
      // end cycle only for analysis module case
      // standalone modules never stops
      if(ModuleApplication::ANALYSIS == m_mode) {
        m_cycle.forceStopCycle(true, true);
      }
      auto eorEvent = new StoreEvent<core::Run>(AppEvent::END_OF_RUN, m_runControl.currentRun());
      eorEvent->setPriority(ModuleApplication::END_OF_RUN);
      m_eventLoop.postEvent(eorEvent);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::receiveEvent(core::EventPtr event) {
      if(m_currentNQueuedEvents.load() >= m_eventQueueSize) {
        return;
      }
      auto appEvent = new StoreEvent<core::EventPtr>(AppEvent::PROCESS_EVENT, event);
      appEvent->setPriority(ModuleApplication::PROCESS_CALL);
      m_eventLoop.postEvent(appEvent);
      m_currentNQueuedEvents++;
    }
    
    
    void ModuleApplication::setElementsRunNumber(core::Run &run) {
      m_monitorElementManager->iterate<OnlineElement>([&](OnlineElementPtr monitorElement){
        monitorElement->setRunNumber(run.runNumber());
        return true;
      });
    }
    
    
    void ModuleApplication::postStandaloneProcess() {
      auto *processEvent = new StoreEvent<core::TimePoint>(AppEvent::PROCESS_EVENT, core::now());
      processEvent->setPriority(ModuleApplication::PROCESS_CALL);
      m_eventLoop.postEvent(processEvent);
    }

  }

}

