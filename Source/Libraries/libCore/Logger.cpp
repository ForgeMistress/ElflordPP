////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Logger
//
//  Outputs a log to the ostream passed to it.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Logger.h"

OPEN_NAMESPACE(Firestorm);

Logger Logger::DEBUG_LOGGER(std::cout);
Logger Logger::WARN_LOGGER(std::cout);
Logger Logger::ERROR_LOGGER(std::cerr);

std::mutex Logger::_s_allLock;

Logger::Logger(std::ostream& ostream)
: _ostream(ostream)
{
}

CLOSE_NAMESPACE(Firestorm);