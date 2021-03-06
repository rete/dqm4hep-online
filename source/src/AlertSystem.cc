/// \file DQMAlertSystem.cc
/*
 *
 * DQMAlertSystem.cc source template automatically generated by a class generator
 * Creation date : mar. mars 22 2016
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


#include "dqm4hep/AlertSystem.h"
#include "dqm4hep/MonitorElement.h"
#include "dqm4hep/Logging.h"

#include <algorithm>

namespace dqm4hep {

  namespace core {

    Alert::Alert() :
        m_type(OTHER_ALERT)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Alert::Alert(const std::string &emitter, AlertType type,
        const std::string &message, const MonitorElementPtr &monitorElement) :
        m_emitter(emitter),
        m_type(type),
        m_message(message),
        m_monitorElement(monitorElement)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Alert::~Alert()
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Alert::getEmitter() const
    {
      return m_emitter;
    }

    //-------------------------------------------------------------------------------------------------

    AlertType Alert::getType() const
    {
      return m_type;
    }

    //-------------------------------------------------------------------------------------------------

    void Alert::setType(AlertType type)
    {
      m_type = type;
    }

    //-------------------------------------------------------------------------------------------------

    void Alert::setMessage(const std::string &message)
    {
      m_message = message;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Alert::getMessage() const
    {
      return m_message;
    }

    //-------------------------------------------------------------------------------------------------

    const MonitorElementPtr &Alert::getMonitorElement() const
    {
      return m_monitorElement;
    }

    //-------------------------------------------------------------------------------------------------

    void Alert::setMonitorElement(const MonitorElementPtr &monitorElement)
    {
      m_monitorElement = monitorElement;
    }

    //-------------------------------------------------------------------------------------------------

    xdrstream::Status Alert::stream(xdrstream::StreamingMode mode, xdrstream::IODevice *pDevice,
        xdrstream::xdr_version_t version)
    {
      if( xdrstream::XDR_READ_STREAM == mode )
      {
        XDR_STREAM( pDevice->read( & m_emitter ) )

		    int8_t type;
        XDR_STREAM( pDevice->read<int8_t>( & type ) )
        m_type = static_cast<AlertType>(type);

        XDR_STREAM( pDevice->read( & m_message ) )

        bool hasMonitorElement;
        XDR_STREAM( pDevice->read( & hasMonitorElement ) )

        if( hasMonitorElement )
        {
          std::unique_ptr<MonitorElement> monitorElement( new MonitorElement() );

          XDR_STREAM( monitorElement->stream( mode , pDevice , version ) )

          m_monitorElement = MonitorElementPtr(monitorElement.release());
        }
      }
      else
      {
        XDR_STREAM( pDevice->write( & m_emitter ) )

		    int8_t type = m_type;
        XDR_STREAM( pDevice->write<int8_t>( & type ) )

        XDR_STREAM( pDevice->write( & m_message ) )

        bool hasMonitorElement = NULL != m_monitorElement;
        XDR_STREAM( pDevice->write( & hasMonitorElement ) )

        if( hasMonitorElement )
          XDR_STREAM( m_monitorElement->stream( mode , pDevice , version ) )
      }

      return xdrstream::XDR_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    AlertNotifier::AlertNotifier(const std::string &emitter) :
        m_emitter(emitter),
        m_minAlertInterval(-1),
        m_lastNotificationTime(0)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    AlertNotifier::~AlertNotifier()
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &AlertNotifier::getEmitter() const
    {
      return m_emitter;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AlertNotifier::notify(AlertType type, const std::string &message, const MonitorElementPtr &monitorElement)
    {
      if( ! this->isRunning() )
        return STATUS_CODE_NOT_INITIALIZED;

      time_t currentTime = time(0);

      if( m_minAlertInterval > 0 && currentTime - m_lastNotificationTime < m_minAlertInterval )
        return STATUS_CODE_UNCHANGED;

      m_lastNotificationTime = currentTime;

      return this->notify(type, message, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    void AlertNotifier::setMinAlertInterval(int nSeconds)
    {
      m_minAlertInterval = nSeconds;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    static const char DimAlertNotifier_emptyBuffer[] = "EMPTY";

    DimAlertNotifier::DimAlertNotifier(const std::string &emitter) :
		    AlertNotifier(emitter),
		    m_isRunning(false),
		    m_pNotificationService(NULL),
		    m_pBuffer(NULL)
    {
      m_pBuffer = new xdrstream::BufferDevice(1024);
    }

    //-------------------------------------------------------------------------------------------------

    DimAlertNotifier::~DimAlertNotifier()
    {
      delete m_pBuffer;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertNotifier::startService()
    {
      if( this->isRunning() )
        return STATUS_CODE_SUCCESS;

      std::string serviceName = "DQM4HEP/ALERT/" + this->getEmitter();
      m_pNotificationService = new DimService( serviceName.c_str() , "C" , (void*) DimAlertNotifier_emptyBuffer , 5 );
      m_isRunning = true;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertNotifier::stopService()
    {
      if( ! this->isRunning() )
        return STATUS_CODE_SUCCESS;

      delete m_pNotificationService;
      m_isRunning = false;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    bool DimAlertNotifier::isRunning() const
    {
      return m_isRunning;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertNotifier::userNotify(AlertType type, const std::string &message, const MonitorElementPtr &monitorElement)
    {
      if( this->isRunning() )
        return STATUS_CODE_NOT_INITIALIZED;

      Alert alert(this->getEmitter(), type, message, monitorElement);

      if( ! XDR_TESTBIT( alert.stream(xdrstream::XDR_WRITE_STREAM, m_pBuffer) , xdrstream::XDR_SUCCESS ) )
        return STATUS_CODE_FAILURE;

      m_pNotificationService->updateService( m_pBuffer->getBuffer() , m_pBuffer->getPosition() );

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    AlertHandler::~AlertHandler()
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void AlertHandler::addListener(AlertListener *pListener)
    {
      if(NULL == pListener)
        return;

      m_listeners.insert( pListener );
    }

    //-------------------------------------------------------------------------------------------------

    void AlertHandler::removeListener(AlertListener *pListener)
    {
      if(NULL == pListener)
        return;

      m_listeners.erase( pListener );
    }

    //-------------------------------------------------------------------------------------------------

    void AlertHandler::notifyListeners(AlertPtr alert)
    {
      for(std::set<AlertListener *>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
          endIter != iter ; ++iter)
      {
        (*iter)->onAlert(alert);
      }
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    DimAlertHandler::DimAlertHandler(const std::string &emitter) :
		    m_emitter(emitter),
		    m_isConnected(false),
		    m_pBuffer(NULL)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    DimAlertHandler::~DimAlertHandler()
    {
      if( m_pBuffer )
        delete m_pBuffer;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &DimAlertHandler::getEmitter() const
    {
      return m_emitter;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertHandler::connectToService()
    {
      if( this->isConnected() )
        return STATUS_CODE_SUCCESS;

      std::string infoName = "DQM4HEP/ALERT/" + this->getEmitter();
      m_pAlertInfo = new DimUpdatedInfo( infoName.c_str(), (void*) NULL, 0 , this );
      m_isConnected = true;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertHandler::disconnectFromService()
    {
      if( ! this->isConnected() )
        return STATUS_CODE_SUCCESS;

      delete m_pAlertInfo;
      m_isConnected = false;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    bool DimAlertHandler::isConnected() const
    {
      return m_isConnected;
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertHandler::configureInBuffer(char *pBuffer, xdrstream::xdr_size_t bufferSize)
    {
      if(NULL == m_pBuffer)
        m_pBuffer = new xdrstream::BufferDevice(pBuffer, bufferSize, false);
      else
        m_pBuffer->setBuffer(pBuffer, bufferSize, false);

      m_pBuffer->setOwner(false);
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertHandler::infoHandler()
    {
      char *pBuffer = static_cast<char *>(m_pAlertInfo->getData());
      xdrstream::xdr_size_t bufferSize = m_pAlertInfo->getSize();

      if(NULL == pBuffer || 0 == bufferSize)
        return;

      this->configureInBuffer( pBuffer, bufferSize );

      AlertPtr alert(new Alert());

      if( ! XDR_TESTBIT( alert->stream( xdrstream::XDR_READ_STREAM , m_pBuffer ) , xdrstream::XDR_SUCCESS ) )
        return;

      this->notifyListeners(alert);
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    DimAlertInterface::DimAlertInterface() :
		    m_updateTime(5),
		    m_isConnected(false),
		    m_pBuffer(NULL)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    DimAlertInterface::~DimAlertInterface()
    {
      if( m_pBuffer )
        delete m_pBuffer;

      this->disconnectFromService();
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::newEmitterRegistered(const std::string &emitter)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::emitterUnregistered(const std::string &emitter)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::setUpdateTime(int updateTime)
    {
      if( updateTime <= 0 )
        return;

      m_updateTime = updateTime;

      if( this->isConnected() )
        this->start( m_updateTime );
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertInterface::connectToService()
    {
      if( this->isConnected() )
        return STATUS_CODE_SUCCESS;

      StringVector emitters = this->browse();

      for(StringVector::iterator iter = emitters.begin(), endIter = emitters.end() ;
          endIter != iter ; ++iter)
      {
        std::string infoName = "DQM4HEP/ALERT/" + *iter;

        DimInfo *pAlertInfo = new DimUpdatedInfo( infoName.c_str() , (void *) NULL, 0 , this );

        m_emitterInfoMap[*iter] = pAlertInfo;
      }

      m_isConnected = true;
      this->start(m_updateTime);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimAlertInterface::disconnectFromService()
    {
      if( ! this->isConnected() )
        return STATUS_CODE_SUCCESS;

      this->stop();

      for(EmitterInfoMap::iterator iter = m_emitterInfoMap.begin(), endIter = m_emitterInfoMap.end() ;
          endIter != iter ; ++iter)
      {
        delete iter->second;
      }

      m_emitterInfoMap.clear();
      m_isConnected = false;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    bool DimAlertInterface::isConnected() const
    {
      return m_isConnected;
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::infoHandler()
    {
      DimInfo *pInfo = getInfo();

      if( ! pInfo )
        return;

      char *pBuffer = static_cast<char *>(pInfo->getData());
      xdrstream::xdr_size_t bufferSize = pInfo->getSize();

      if( NULL == pBuffer || 0 == bufferSize )
        return;

      AlertPtr alert(new Alert());

      if( ! XDR_TESTBIT( alert->stream( xdrstream::XDR_READ_STREAM , m_pBuffer ) , xdrstream::XDR_SUCCESS ) )
        return;

      this->notifyListeners(alert);
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::timerHandler()
    {
      StringVector emitters = this->browse();

      StringVector emittersToRemove;

      // find emitters to remove from info map
      for(EmitterInfoMap::iterator iter = m_emitterInfoMap.begin(), endIter = m_emitterInfoMap.end() ;
          endIter != iter ; ++iter)
      {
        if( std::find( emitters.begin() , emitters.end() , iter->first ) == emitters.end() )
          emittersToRemove.push_back(iter->first);
      }

      // remove emitters from map
      for(StringVector::iterator iter = emittersToRemove.begin(), endIter = emittersToRemove.end() ;
          endIter != iter ; ++iter)
      {
        EmitterInfoMap::iterator findIter = m_emitterInfoMap.find( *iter );

        if( findIter == m_emitterInfoMap.end() )
          continue;

        delete findIter->second;
        m_emitterInfoMap.erase( findIter );

        // notify !
        this->emitterUnregistered( *iter );
      }

      // find emitters to add to info map and add them on the fly
      for(StringVector::iterator iter = emitters.begin(), endIter = emitters.end() ;
          endIter != iter ; ++iter)
      {
        EmitterInfoMap::iterator findIter = m_emitterInfoMap.find( *iter );

        if( findIter != m_emitterInfoMap.end() )
          continue;

        std::string infoName = "DQM4HEP/ALERT/" + *iter;

        DimInfo *pAlertInfo = new DimUpdatedInfo( infoName.c_str() , (void *) NULL, 0 , this );

        m_emitterInfoMap[ *iter ] = pAlertInfo;

        // notify !
        this->newEmitterRegistered( *iter );
      }

      // restart timer
      this->start(m_updateTime);
    }

    //-------------------------------------------------------------------------------------------------

    StringVector DimAlertInterface::browse()
    {
      StringVector emitters;

      DimBrowser browser;
      int nServices = browser.getServices("DQM4HEP/ALERT/*");

      if( 0 == nServices )
        return emitters;

      char *pServiceName, *pFormat;
      int serviceType;

      while( 1 )
      {
        serviceType = browser.getNextService(pServiceName, pFormat);

        if( 0 == serviceType )
          break;

        if( DimSERVICE != serviceType )
          continue;

        std::string emitter = std::string(pServiceName).substr( std::string("DQM4HEP/ALERT/").size() );

        LOG4CXX_DEBUG( dqmMainLogger, "Found emitter : " << emitter );

        emitters.push_back(emitter);
      }

      return emitters;
    }

    //-------------------------------------------------------------------------------------------------

    void DimAlertInterface::configureInBuffer(char *pBuffer, xdrstream::xdr_size_t bufferSize)
    {
      if(NULL == m_pBuffer)
        m_pBuffer = new xdrstream::BufferDevice(pBuffer, bufferSize, false);
      else
        m_pBuffer->setBuffer(pBuffer, bufferSize, false);

      m_pBuffer->setOwner(false);
    }

  }

} 

