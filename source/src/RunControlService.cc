// /// \file DQMRunControlService.cc
// /*
//  *
//  * DQMRunControlService.cc source template automatically generated by a class generator
//  * Creation date : mer. nov. 12 2014
//  *
//  * This file is part of DQM4HEP libraries.
//  *
//  * DQM4HEP is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  * based upon these libraries are permitted. Any copy of these libraries
//  * must include this copyright notice.
//  *
//  * DQM4HEP is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
//  *
//  * @author Remi Ete
//  * @copyright CNRS , IPNL
//  */
//
// // -- dqm4hep headers
// #include "dqm4hep/RunControlService.h"
// #include "dqm4hep/RunControl.h"
// #include "dqm4hep/Run.h"
// #include "dqm4hep/Logging.h"
// #include "dqm4hep/CoreTool.h"
//
// #include <fstream>
//
// namespace dqm4hep {
//
//   namespace core {
//
//     static const char RunControlService_emptyBuffer [] = "EMPTY";
//
//     CurrentRunRpc::CurrentRunRpc(char *rpcName, RunControlService *pService) :
//         DimRpc(rpcName, "I", "C"),
//         m_pService(pService)
//     {
//       /* nop */
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void CurrentRunRpc::rpcHandler()
//     {
//       m_pService->handleCurrentRunRpc(this);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//     //-------------------------------------------------------------------------------------------------
//
//     StartNewRunRpc::StartNewRunRpc(char *rpcName, RunControlService *pService) :
//         DimRpc(rpcName, "C", "I:1;C"),
//         m_pService(pService)
//     {
//       /* nop */
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void StartNewRunRpc::rpcHandler()
//     {
//       m_pService->handleStartNewRunRpc(this);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//     //-------------------------------------------------------------------------------------------------
//
//     EndCurrentRunRpc::EndCurrentRunRpc(char *rpcName, RunControlService *pService) :
//         DimRpc(rpcName, "C", "I:1;C"),
//         m_pService(pService)
//     {
//       /* nop */
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void EndCurrentRunRpc::rpcHandler()
//     {
//       m_pService->handleEndCurrentRunRpc(this);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//     //-------------------------------------------------------------------------------------------------
//
//     RunControlService::RunControlService() :
//         m_currentRunNumber(-1),
//         m_serviceState(STOPPED_STATE),
//         m_pStartOfRunService(NULL),
//         m_pEndOfRunService(NULL),
//         m_pRunControl(NULL),
//         m_pCurrentRunRpc(NULL),
//         m_pStartNewRunRpc(NULL),
//         m_pEndCurrentRunRpc(NULL),
//         m_pOutBuffer(0),
//         m_pInBuffer(0)
//     {
//       m_pRunControl = new RunControl();
//       m_pOutBuffer = new xdrstream::BufferDevice(1024);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     RunControlService::~RunControlService()
//     {
//       if(this->isRunning())
//       {
//         if(RUNNING_STATE == m_pRunControl->getRunState())
//           this->endCurrentRun();
//
//         this->stop();
//       }
//
//       delete m_pRunControl;
//       delete m_pOutBuffer;
//
//       if( m_pInBuffer )
//         delete m_pInBuffer;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::start()
//     {
//       if(isRunning())
//         return STATUS_CODE_ALREADY_INITIALIZED;
//
//       std::string sorServiceName = "DQM4HEP/RunControl/" + m_pRunControl->getRunControlName() + "/START_OF_RUN";
//       std::string eorServiceName = "DQM4HEP/RunControl/" + m_pRunControl->getRunControlName() + "/END_OF_RUN";
//       std::string currentRunRpcName = "DQM4HEP/RunControl/" + m_pRunControl->getRunControlName() + "/CURRENT_RUN";
//       std::string startNewRunRpcName = "DQM4HEP/RunControl/" + m_pRunControl->getRunControlName() + "/START_NEW_RUN";
//       std::string endCurrentRunRpcName = "DQM4HEP/RunControl/" + m_pRunControl->getRunControlName() + "/END_CURRENT_RUN";
//
//       m_pStartOfRunService = new DimService(sorServiceName.c_str(), "C", (void*) RunControlService_emptyBuffer, 5);
//       m_pEndOfRunService   = new DimService(eorServiceName.c_str(), "C", (void*) RunControlService_emptyBuffer, 5);
//       m_pCurrentRunRpc = new CurrentRunRpc( (char *) currentRunRpcName.c_str(), this);
//       m_pStartNewRunRpc = new StartNewRunRpc( (char *) startNewRunRpcName.c_str(), this);
//       m_pEndCurrentRunRpc = new EndCurrentRunRpc( (char *) endCurrentRunRpcName.c_str(), this);
//
//       m_serviceState = RUNNING_STATE;
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::stop()
//     {
//       if(!isRunning())
//         return STATUS_CODE_NOT_INITIALIZED;
//
//       delete m_pStartOfRunService;     m_pStartOfRunService = NULL;
//       delete m_pEndOfRunService; 	     m_pEndOfRunService = NULL;
//       delete m_pCurrentRunRpc;         m_pCurrentRunRpc = NULL;
//       delete m_pStartNewRunRpc;        m_pStartNewRunRpc = NULL;
//       delete m_pEndCurrentRunRpc;      m_pEndCurrentRunRpc = NULL;
//
//       m_serviceState = STOPPED_STATE;
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::pause()
//     {
//       return stop();
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::restart()
//     {
//       if(isRunning())
//       {
//         RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, stop());
//         RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, start());
//
//         return STATUS_CODE_SUCCESS;
//       }
//       else
//       {
//         RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, start());
//
//         return STATUS_CODE_SUCCESS;
//       }
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::startNewRun(unsigned int runNumber, const std::string &description, const std::string &detectorName, const std::string &password)
//     {
//       if(!isRunning())
//         return STATUS_CODE_NOT_INITIALIZED;
//
//       if(RUNNING_STATE == getRunState())
//         RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, endCurrentRun(password));
//
//       m_currentRunNumber = runNumber;
//       RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControl->startNewRun(m_currentRunNumber, description, detectorName, password));
//
//       m_pOutBuffer->reset();
//
//       if( xdrstream::XDR_SUCCESS != m_pRunControl->getCurrentRun()->stream( xdrstream::XDR_WRITE_STREAM , m_pOutBuffer ) )
//         return STATUS_CODE_FAILURE;
//
//       m_pStartOfRunService->updateService( (void *) m_pOutBuffer->getBuffer(), m_pOutBuffer->getPosition());
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::startNewRun(Run *pRun, const std::string &password)
//     {
//       if(!isRunning())
//         return STATUS_CODE_NOT_INITIALIZED;
//
//       if(RUNNING_STATE == getRunState())
//         RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, endCurrentRun(password));
//
//       m_currentRunNumber = pRun->getRunNumber();
//       RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControl->startNewRun(pRun, password));
//
//       m_pOutBuffer->reset();
//       if( xdrstream::XDR_SUCCESS != m_pRunControl->getCurrentRun()->stream( xdrstream::XDR_WRITE_STREAM , m_pOutBuffer ) )
//         return STATUS_CODE_FAILURE;
//
//       m_pStartOfRunService->updateService( (void *) m_pOutBuffer->getBuffer(), m_pOutBuffer->getPosition());
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::endCurrentRun(const std::string &password)
//     {
//       if(!isRunning())
//         return STATUS_CODE_NOT_INITIALIZED;
//
//       if(RUNNING_STATE != getRunState())
//         return STATUS_CODE_SUCCESS;
//
//       m_currentRunNumber = -1;
//       m_pRunControl->getCurrentRun()->setEndTime(CoreTool::now());
//       m_pOutBuffer->reset();
//
//       if( xdrstream::XDR_SUCCESS != m_pRunControl->getCurrentRun()->stream( xdrstream::XDR_WRITE_STREAM , m_pOutBuffer ) )
//         return STATUS_CODE_FAILURE;
//
//       RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControl->endCurrentRun(password));
//
//       m_pEndOfRunService->updateService((void *) m_pOutBuffer->getBuffer(), m_pOutBuffer->getPosition());
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     int RunControlService::getCurrentRunNumber() const
//     {
//       return m_currentRunNumber;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     State RunControlService::getRunState() const
//     {
//       return m_pRunControl->getRunState();
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     bool RunControlService::isRunning() const
//     {
//       return (m_serviceState == RUNNING_STATE);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     State RunControlService::getState() const
//     {
//       return m_serviceState;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::setRunControlName(const std::string &runControlName)
//     {
//       if(isRunning())
//         return STATUS_CODE_NOT_ALLOWED;
//
//       m_pRunControl->setRunControlName(runControlName);
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     const std::string &RunControlService::getRunControlName() const
//     {
//       return m_pRunControl->getRunControlName();
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     Run *RunControlService::getCurrentRun() const
//     {
//       return m_pRunControl->getCurrentRun();
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     StatusCode RunControlService::setPassword( const std::string &password )
//     {
//       if( this->isRunning() )
//         return STATUS_CODE_NOT_ALLOWED;
//
//       m_pRunControl->setPassword(password);
//
//       return STATUS_CODE_SUCCESS;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
// #ifdef DQM4HEP_USE_MONGOOSE
//     void RunControlService::setup()
//     {
//       addRoute( "GET"  , "/" + m_pRunControl->getRunControlName() + "/SOR" ,  RunControlService , mongooseStartOfRunGetForm );
//       addRoute( "POST" , "/" + m_pRunControl->getRunControlName() + "/SOR" ,  RunControlService , mongooseStartOfRun );
//
//       addRoute( "GET"  , "/" + m_pRunControl->getRunControlName() + "/EOR" ,  RunControlService , mongooseEndCurrentRunGetForm );
//       addRoute( "POST" , "/" + m_pRunControl->getRunControlName() + "/EOR" ,  RunControlService , mongooseEndOfRun );
//
//       addRoute( "GET"  , "/" + m_pRunControl->getRunControlName() + "/STATUS" ,  RunControlService , mongooseRunStatus );
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::mongooseStartOfRunGetForm(Mongoose::Request &request, Mongoose::StreamResponse &response)
//     {
//       LOG4CXX_DEBUG( dqmMainLogger , "Received GET: /" + m_pRunControl->getRunControlName() + "/SOR" );
//
//       if( m_pRunControl->isRunning() )
//       {
//         Run *pRun = m_pRunControl->isRunning() ? this->getCurrentRun() : 0;
//         response << this->configureRunStatusHtml( pRun );
//         return;
//       }
//
//       std::string fileName = std::string(DQMCore_DIR) + "/web/run-ctrl-sor-form.html";
//       std::ifstream in(fileName.c_str(), std::ios::in);
//       std::string contents;
//
//       if ( in )
//       {
//         in.seekg(0, std::ios::end);
//         contents.resize(in.tellg());
//         in.seekg(0, std::ios::beg);
//         in.read(&contents[0], contents.size());
//         in.close();
//
//         DQM4HEP::replace( contents , "RUN_CONTROL_NAME" , m_pRunControl->getRunControlName() );
//       }
//       else
//         contents = "Problem !!";
//
//       response << contents;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::mongooseEndCurrentRunGetForm(Mongoose::Request &request, Mongoose::StreamResponse &response)
//     {
//       LOG4CXX_DEBUG( dqmMainLogger , "Received GET: /" + m_pRunControl->getRunControlName() + "/EOR" );
//
//       if( ! m_pRunControl->isRunning() )
//       {
//         Run *pRun = m_pRunControl->isRunning() ? this->getCurrentRun() : 0;
//         response << this->configureRunStatusHtml( pRun );
//         return;
//       }
//
//       std::string fileName = std::string(DQMCore_DIR) + "/web/run-ctrl-eor-form.html";
//       std::ifstream in(fileName.c_str(), std::ios::in);
//       std::string contents;
//
//       if ( in )
//       {
//         in.seekg(0, std::ios::end);
//         contents.resize(in.tellg());
//         in.seekg(0, std::ios::beg);
//         in.read(&contents[0], contents.size());
//         in.close();
//
//         DQM4HEP::replace( contents , "RUN_CONTROL_NAME" , m_pRunControl->getRunControlName() );
//       }
//       else
//         contents = "Problem !!";
//
//       response << contents;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::mongooseStartOfRun(Mongoose::Request &request, Mongoose::StreamResponse &response)
//     {
//       LOG4CXX_DEBUG( dqmMainLogger , "Received POST: /" + m_pRunControl->getRunControlName() + "/SOR" );
//
//       std::map<std::string, std::string> variables(request.getAllVariable());
//       std::map<std::string, std::string> urlVariables;
//       request.getVars(urlVariables);
//
//       Run *pRun = new Run();
//       std::string password;
//       bool consistent = false;
//
//       for(auto iter = variables.begin(), endIter = variables.end() ; endIter != iter ; ++iter)
//       {
//         if( iter->first == "run" )
//         {
//           int runNumber = 0;
//           DQM4HEP::stringToType( iter->second , runNumber );
//           pRun->setRunNumber( runNumber );
//           consistent = true;
//           continue;
//         }
//
//         if( iter->first == "description" )
//         {
//           pRun->setDescription( iter->second );
//           continue;
//         }
//
//         if( iter->first == "detectorName" )
//         {
//           pRun->setDetectorName( iter->second );
//           continue;
//         }
//
//         if( iter->first == "password" )
//         {
//           password = iter->second;
//           continue;
//         }
//
//         pRun->setParameter( iter->first , iter->second );
//       }
//
//       if( ! consistent )
//       {
//         delete pRun;
//         response << "ERROR: Run number required to start a new run !!<br/>"<< std::endl;
//         return;
//       }
//
//       try
//       {
//         THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->startNewRun(pRun, password));
//       }
//       catch( StatusCodeException &exception )
//       {
//         response << "Couldn't start new run ! What :" << exception.toString()<< std::endl;
//         return;
//       }
//
//       response << this->configureRunStatusHtml( pRun );
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::mongooseEndOfRun(Mongoose::Request &request, Mongoose::StreamResponse &response)
//     {
//       LOG4CXX_DEBUG( dqmMainLogger , "Received POST: /" + m_pRunControl->getRunControlName() + "/EOR" );
//
//       if( ! m_pRunControl->isRunning() )
//       {
//         // return run status (not running)
//         Run *pRun = m_pRunControl->isRunning() ? this->getCurrentRun() : 0;
//         response << this->configureRunStatusHtml( pRun );
//         return;
//       }
//
//       std::map<std::string, std::string> variables(request.getAllVariable());
//
//       std::string password;
//       Run *pRun = this->getCurrentRun();
//       int runNumber = pRun->getRunNumber();
//
//       for(auto iter = variables.begin(), endIter = variables.end() ; endIter != iter ; ++iter)
//       {
//         if( iter->first == "password" )
//         {
//           password = iter->second;
//           continue;
//         }
//
//         pRun->setParameter( iter->first , iter->second );
//       }
//
//       try
//       {
//         THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->endCurrentRun(password));
//       }
//       catch( StatusCodeException &exception )
//       {
//         response << "Couldn't stop current run ! What :" << exception.toString()<< std::endl;
//         return;
//       }
//
//       Run *pTmpRun = m_pRunControl->isRunning() ? pRun : 0;
//       response << this->configureRunStatusHtml( pTmpRun );
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::mongooseRunStatus(Mongoose::Request &request, Mongoose::StreamResponse &response)
//     {
//       LOG4CXX_DEBUG( dqmMainLogger , "Received GET: /" + m_pRunControl->getRunControlName() + "/STATUS" );
//
//       Run *pRun = m_pRunControl->isRunning() ? this->getCurrentRun() : 0;
//       response << this->configureRunStatusHtml( pRun );
//       return;
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     std::string RunControlService::configureRunStatusHtml(const Run *const pRun) const
//     {
//       std::string fileName = std::string(DQMCore_DIR) + "/web/run-ctrl-status.html";
//       std::ifstream in(fileName.c_str(), std::ios::in);
//       std::string contents;
//
//       if ( in )
//       {
//         in.seekg(0, std::ios::end);
//         contents.resize(in.tellg());
//         in.seekg(0, std::ios::beg);
//         in.read(&contents[0], contents.size());
//         in.close();
//
//         StringVector parameterKeys;
//         std::string startTime, elapsedTime, description, detectorName;
//         std::string status, statusColor;
//         int runNumber;
//
//         if( pRun )
//         {
//           parameterKeys = pRun->getParameterKeys();
//           CoreTool::timeToHMS( std::chrono::system_clock::to_time_t(pRun->getStartTime()), startTime );
//           CoreTool::timeToHMS( time(0) - std::chrono::system_clock::to_time_t(pRun->getStartTime()), elapsedTime );
//
//           runNumber = pRun->getRunNumber();
//           description = pRun->getDescription();
//           detectorName = pRun->getDetectorName();
//           status = "RUNNING";
//           statusColor = "green";
//         }
//         else
//         {
//           startTime = "-";
//           elapsedTime = "-";
//           runNumber = 0;
//           description = "-";
//           detectorName = "-";
//           status = "NOT RUNNING";
//           statusColor = "red";
//         }
//
//         DQM4HEP::replace( contents , "RUN_NUMBER", runNumber );
//         DQM4HEP::replace( contents , "START_TIME", startTime );
//         DQM4HEP::replace( contents , "ELLAPSED_TIME", elapsedTime );
//         DQM4HEP::replace( contents , "DESCRIPTION", description );
//         DQM4HEP::replace( contents , "DETECTOR_NAME", detectorName );
//
//         DQM4HEP::replace( contents , "RUN_STATUS", status );
//         DQM4HEP::replace( contents , "STATUS_COLOR", statusColor );
//
//         DQM4HEP::replace( contents , "RUN_CONTROL_NAME" , m_pRunControl->getRunControlName() );
//
//         std::string javascriptParameters = "{";
//
//         for( auto iter = parameterKeys.begin(), endIter = parameterKeys.end() ;
//             endIter != iter ; ++iter)
//         {
//           std::string jsParameter("'${KEY}' : '${VALUE}'");
//
//           std::string parameter;
//           pRun->getParameter( *iter , parameter );
//
//           DQM4HEP::replace( jsParameter , "KEY" , *iter );
//           DQM4HEP::replace( jsParameter , "VALUE" , parameter );
//
//           std::string coma = ( iter == parameterKeys.begin() ) ? "" : " , ";
//
//           javascriptParameters += (coma + jsParameter);
//         }
//
//         javascriptParameters += " }";
//
//         DQM4HEP::replace( contents , "PARAMETER_LIST", javascriptParameters );
//
//         return contents;
//       }
//
//       return "Invalid operation on server side";
//     }
//
// #endif
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::configureInBuffer( char *pBuffer, xdrstream::xdr_size_t bufferSize )
//     {
//       if( ! m_pInBuffer )
//         m_pInBuffer = new xdrstream::BufferDevice( pBuffer, bufferSize , false );
//       else
//         m_pInBuffer->setBuffer( pBuffer, bufferSize , false );
//
//       m_pInBuffer->setOwner(false);
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::handleCurrentRunRpc(DimRpc *pRpc)
//     {
//       if( ! this->isRunning() )
//         return;
//
//       try
//       {
//         // if running send the current run
//         // else send back an empty run
//         if(m_pRunControl->isRunning())
//         {
//           // stream
//           m_pOutBuffer->reset();
//
//           if( xdrstream::XDR_SUCCESS != m_pRunControl->getCurrentRun()->stream( xdrstream::XDR_WRITE_STREAM , m_pOutBuffer ) )
//             throw StatusCodeException(STATUS_CODE_FAILURE);
//
//           // send back current run
//           pRpc->setData( (void *) m_pOutBuffer->getBuffer() , m_pOutBuffer->getPosition() );
//         }
//         else
//         {
//           Run emptyRun;
//
//           // stream
//           m_pOutBuffer->reset();
//
//           if( xdrstream::XDR_SUCCESS != emptyRun.stream( xdrstream::XDR_WRITE_STREAM , m_pOutBuffer ) )
//             throw StatusCodeException(STATUS_CODE_FAILURE);
//
//           // send back current run
//           pRpc->setData( (void *) m_pOutBuffer->getBuffer() , m_pOutBuffer->getPosition() );
//         }
//       }
//       catch(StatusCodeException &exception)
//       {
//         LOG4CXX_ERROR( dqmMainLogger , "Couldn't send back current run : " << exception.toString() );
//       }
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::handleStartNewRunRpc(DimRpc *pRpc)
//     {
//       if( ! this->isRunning() )
//         return;
//
//       RpcResponse response;
//       response.m_ok = false;
//
//       if( m_pRunControl->isRunning() )
//       {
//         sprintf(response.m_pMessage, "Already running !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_ERROR( dqmMainLogger , "Couldn't start a new run : Already running !");
//         return;
//       }
//
//       try
//       {
//         char *pBuffer = (char *) pRpc->getData();
//         xdrstream::xdr_size_t bufferSize = pRpc->getSize();
//
//         if( NULL == pBuffer || 0 == bufferSize )
//         {
//           sprintf(response.m_pMessage, "Invalid incoming buffer to start a new run !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't start a new run : Couldn't decode incoming buffer !");
//           return;
//         }
//
//         this->configureInBuffer( pBuffer, bufferSize );
//
//         std::string password;
//         if( xdrstream::XDR_SUCCESS != m_pInBuffer->read( &password ) )
//         {
//           sprintf(response.m_pMessage, "Couldn't read password !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't start a new run : Couldn't decode password !");
//           return;
//         }
//
//         if( ! m_pRunControl->checkPassword( password ) )
//         {
//           sprintf(response.m_pMessage, "Invalid password !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't start a new run : Invalid password !");
//           return;
//         }
//
//         Run *pRun = new Run();
//
//         if( xdrstream::XDR_SUCCESS != pRun->stream( xdrstream::XDR_READ_STREAM , m_pInBuffer ) )
//         {
//           delete pRun;
//           sprintf(response.m_pMessage, "Couldn't read run info !");
//           std::cout << sizeof(RpcResponse) << std::endl;
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't start a new run : Couldn't decode incoming run info !");
//           return;
//         }
//
//         THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->startNewRun(pRun, password));
//
//         response.m_ok = true;
//         sprintf(response.m_pMessage, "Started !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_INFO( dqmMainLogger , "Starting new run : \n" << *this->getCurrentRun() );
//       }
//       catch(StatusCodeException &exception)
//       {
//         sprintf(response.m_pMessage, "Couldn't start new run ! Invalid operation on server side !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_ERROR( dqmMainLogger , "Couldn't start new run from rpc : " << exception.toString() );
//       }
//     }
//
//     //-------------------------------------------------------------------------------------------------
//
//     void RunControlService::handleEndCurrentRunRpc(DimRpc *pRpc)
//     {
//       if( ! this->isRunning() )
//         return;
//
//       RpcResponse response;
//       response.m_ok = false;
//
//       if( ! m_pRunControl->isRunning() )
//       {
//         sprintf(response.m_pMessage, "Not running !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_ERROR( dqmMainLogger , "Couldn't end current new run : Not running !");
//         return;
//       }
//
//       try
//       {
//         char *pBuffer = (char *) pRpc->getData();
//         xdrstream::xdr_size_t bufferSize = pRpc->getSize();
//
//         if( NULL == pBuffer || 0 == bufferSize )
//         {
//           sprintf(response.m_pMessage, "Invalid incoming buffer to stop the current run !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't end current new run : Invalid incoming buffer !");
//           return;
//         }
//
//         this->configureInBuffer( pBuffer, bufferSize );
//
//         std::string password;
//         if( xdrstream::XDR_SUCCESS != m_pInBuffer->read( &password ) )
//         {
//           sprintf(response.m_pMessage, "Couldn't read password !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't end current new run : Couldn't read password !");
//           return;
//         }
//
//         if( ! m_pRunControl->checkPassword( password ) )
//         {
//           sprintf(response.m_pMessage, "Invalid password !");
//           pRpc->setData( & response , sizeof(RpcResponse) );
//           LOG4CXX_ERROR( dqmMainLogger , "Couldn't end current new run : Invalid password !");
//           return;
//         }
//
//         THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->endCurrentRun(password));
//
//         response.m_ok = true;
//         sprintf(response.m_pMessage, "Stopped !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_INFO( dqmMainLogger , "Current run stopped !");
//       }
//       catch(StatusCodeException &exception)
//       {
//         sprintf(response.m_pMessage, "Couldn't stop current run ! Invalid operation on server side !");
//         pRpc->setData( & response , sizeof(RpcResponse) );
//         LOG4CXX_ERROR( dqmMainLogger , "Couldn't stop current run from rpc : " << exception.toString() );
//       }
//     }
//
//   }
//
// }
