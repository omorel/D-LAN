/**
  * D-LAN - A decentralized LAN file sharing software.
  * Copyright (C) 2010-2011 Greg Burri <greg.burri@gmail.com>
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
  
#ifndef CORE_CORE_SERVICE_H
#define CORE_CORE_SERVICE_H

#include <QCoreApplication>

#include <Libs/qtservice/src/qtservice.h>

#include <Common/Uncopyable.h>

#include <ConsoleReader.h>
#include <Core.h>

namespace CoreSpace
{
   class CoreService : public QObject, public QtService<QCoreApplication>, Common::Uncopyable
   {
      Q_OBJECT
   public:
      CoreService(int argc, char** argv);
      virtual ~CoreService() {};

   protected:
      void start();
      void stop();

   private slots:
      void treatUserInput(QString);

   private:
      Core core;
      ConsoleReader consoleReader;
   };
}
#endif
