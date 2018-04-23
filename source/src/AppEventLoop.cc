/// \file AppEventLoop.cc
/*
 *
 * AppEventLoop.cc source template automatically generated by a class generator
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
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include "dqm4hep/AppEventLoop.h"
#include "dqm4hep/Logging.h"

// -- std headers
#include <memory>

namespace dqm4hep {

  namespace online {
    
    AppEventLoop::Timer::Timer(const std::string &n, AppEventLoop &loop) :
      DimTimer(),
      m_name(n),
      m_appEventLoop(loop)
    {
      /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string &AppEventLoop::Timer::name() const {
      return m_name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::Timer::setTimeout(int nSeconds) {
      m_timeout = nSeconds;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int AppEventLoop::Timer::timeout() const {
      return m_timeout;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::Timer::startTimer() {
      DimTimer::start(m_timeout);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::Timer::stopTimer() {
      DimTimer::stop();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::Timer::setSingleShot(bool single) {
      m_singleShot = single;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool AppEventLoop::Timer::singleShot() const {
      return m_singleShot;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::Signal<void> &AppEventLoop::Timer::onTimeout() {
      return m_timeoutSignal;
    }
    
    //-------------------------------------------------------------------------------------------------

    void AppEventLoop::Timer::timerHandler() {
      // process the timeout in the event loop
      m_appEventLoop.processFunction([this](){
        this->m_timeoutSignal.process();
      });
      
      // restart in not single shot
      if(!singleShot()) {
        startTimer();
      }
      else {
        m_appEventLoop.removeTimer(this);
      }
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::postEvent(AppEvent *pAppEvent)
    {
      if(nullptr == pAppEvent)
        return;
        
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      
      // push event in the queue
      m_eventQueue.push_front(std::shared_ptr<AppEvent>(pAppEvent));
      
      // sort the event queue by priority
      std::sort(m_eventQueue.begin(), m_eventQueue.end(), [](AppEventPtr lhs, AppEventPtr rhs){
        return lhs->priority() > rhs->priority();
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::sendEvent(AppEvent *pAppEvent)
    {
      if(nullptr == pAppEvent)
        return;
        
      // take ownership of event ptr
      std::unique_ptr<AppEvent> ptr(pAppEvent);
      
      // process an event
      this->processEvent(pAppEvent);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::clear()
    {
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      m_eventQueue.clear();
    }
    
    //-------------------------------------------------------------------------------------------------

    int AppEventLoop::exec()
    {
      m_running = true;
      m_quitFlag = false;
      
      {
        // start timers
        std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
        for(auto timer : m_timers) {
          timer.second->startTimer();
        }
      }
      
      while(1)
      {
        if(m_quitFlag)
          break;
        
        // safely get the app event pointer 
        AppEventPtr event;
        
        {
          std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
          
          if(!m_eventQueue.empty())
          {
            event = m_eventQueue.back();
            m_eventQueue.pop_back();            
          }
        }
        
        // if no event, save cpu ressources ...
        if(!event)
        {
          usleep(100);
          continue;
        }
        
        // process an event
        AppEvent *pAppEvent = event.get();
        
        try
        {
          this->processEvent(pAppEvent);          
        }
        catch(...)
        {
          m_returnCode = 1;
          break;
        }
        
        {
          std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
          for(auto timer : m_timerRemovals) {
            // just in case ...
            timer.second->stopTimer();
            delete timer.second;
          }
          m_timerRemovals.clear();
        }
      }
      
      m_running = false;
      
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      for(auto timer : m_timerRemovals) {
        timer.second->stopTimer();
        delete timer.second;
      }
      for(auto timer : m_timers) {
        timer.second->stopTimer();
        delete timer.second;
      }
      m_timers.clear();
      m_timerRemovals.clear();
      
      return m_returnCode.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool AppEventLoop::running() const
    {
      return m_running.load();
    }

    //-------------------------------------------------------------------------------------------------    
    
    void AppEventLoop::exit(int returnCode)
    {
      this->sendEvent(new QuitEvent(returnCode));
    }

    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::quit()
    {
      this->exit(0);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::processEvent(AppEvent *pAppEvent)
    {
      // if(!this->running())
      // {
      //   dqm_error( "AppEventLoop::processEvent(): can not process event, the event loop has not been started yet !" );
      //   throw core::StatusCodeException(core::STATUS_CODE_NOT_INITIALIZED);
      // }
      
      try
      {
        std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
        m_onEventSignal.process(pAppEvent);
      }
      catch(core::StatusCodeException &except)
      {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection())
        {
          dqm_error( "EventLoop::exec(): Caught exception: {0}" , except.what() );
          throw except;          
        }
        else
        {
          m_onExceptionSignal.process(pAppEvent);
        }
      }
      catch(std::exception &except)
      {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection())
        {
          dqm_error( "EventLoop::exec(): Caught exception: {0}" , except.what() );
          throw except;          
        }
        else
        {
          m_onExceptionSignal.process(pAppEvent);
        }
      }
      catch(...)
      {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection())
        {
          dqm_error( "EventLoop::exec(): Caught unknown exception !" );
          throw core::StatusCodeException(core::STATUS_CODE_FAILURE);
        }
        else
        {
          m_onExceptionSignal.process(pAppEvent);
        }
      }
      
      if(pAppEvent->type() == AppEvent::QUIT)
      {
        m_quitFlag = true;
        m_returnCode = 1;
        QuitEvent *pQuitEvent = dynamic_cast<QuitEvent*>(pAppEvent);
        
        if(pQuitEvent)
          m_returnCode = pQuitEvent->returnCode();
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::removeTimer(Timer *timer) {
      removeTimer(timer->name());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::removeTimer(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      auto findIter = m_timers.find(name);
      if(m_timers.end() != findIter) {
        findIter->second->stopTimer();
        m_timerRemovals[findIter->first] = findIter->second;
        m_timers.erase(findIter);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int AppEventLoop::count(int eventType) {
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      return std::count_if(m_eventQueue.begin(), m_eventQueue.end(), [&eventType](AppEventPtr ptr){
        return (ptr->type() == eventType);
      });
    }
    
  }

}

