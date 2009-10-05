#ifndef PEERMANAGER_IGETENTRIES_H
#define PEERMANAGER_IGETENTRIES_H

#include <QObject>

#include <Protos/core_protocol.pb.h>

namespace PeerManager
{
   class IGetEntries : QObject
   {
      Q_OBJECT
   signals:
      void result(const Protos::Core::GetEntriesResult& result);
   };
}
#endif
