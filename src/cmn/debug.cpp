/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iomanip>
#include <debug.h>

using namespace cmn::utils;

Debug::Debug()
{
  indent = 0;
  newLine = true;
}

Debug::~Debug()
{
  //TODO
}

void Debug::printDebugStream(log_levels l)
{
  log_msg(l, "%s" , stream.str().c_str());
}

void Debug::clearStream()
{
  stream.str("");
}

void Debug::printDebugStreamToFile()
{
  // TODO 
}

void Debug::add(char* data)
{
  if (newLine)
     startNewLine();
  stream << data;
}

void Debug::add(uint8_t data)
{
  if (newLine)
     startNewLine();
  stream << (uint16_t)data;
}

void Debug::add(uint16_t data)
{
  if (newLine)
     startNewLine();
  stream << data;
}

void Debug::add(uint32_t data)
{
  if (newLine)
     startNewLine();
  stream << data;
}

void Debug::add(uint64_t data)
{
  if (newLine)
     startNewLine();
  stream << data;
}

void Debug::addHexByte(uint8_t byte)
{
    if(newLine)
	startNewLine();

    stream << std::hex << " 0x" <<
    std::setw(2) << std::setfill('0') << (int) byte;
}

void Debug::endOfLine()
{
  stream <<"\n";
  newLine = true;
}

void Debug::startNewLine()
{
  uint8_t i;
  for (i = 0; i<= indent; i++)
  {
    stream <<"  ";
  }
  newLine = false;
}

void Debug::incrIndent()
{
  indent++;
}

void Debug::decrIndent()
{
  if (indent > 0)
    indent --;
}

void Debug::setHexOutput()
{
  stream.setf(ios::hex, ios::basefield);
}

void Debug::unsetHexOutput()
{
  stream.unsetf(ios::hex);
}

