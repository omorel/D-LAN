#include <Tests.h>
using namespace FM;

#include <QtDebug>
#include <QDir>
#include <QStringList>

#include <Protos/common.pb.h>

#include <Exceptions.h>

Tests::Tests()
{
}

void Tests::initTestCase()
{
   this->fileManager = Builder::newFileManager();
}

void Tests::addSharedDirectories()
{
   this->sharedDirs << QDir::currentPath().append("/../../terms");
   this->sharedDirs << QDir::currentPath().append("/asdasdasd"); // This directory doesn't exit.

   try
   {
      this->fileManager->setSharedDirsReadOnly(this->sharedDirs);
   }
   catch (DirsNotFoundException& e)
   {
      foreach (QString path, e.getPaths())
         qDebug() << "This directory hasn't been found : " << path << " (Exception thrown)";
   }

   this->sharedDirs.removeLast(); // Remove the nonexistent directory.
}


void Tests::search()
{
   /**
     * Execute many times the search in parallel
     * with the indexing.
     */
   for (int i = 0; i < 20; i++)
   {
      this->printAmount();
      this->doASearch(false);
   }

   QTest::qSleep(500);

   this->doASearch(true);
   this->printAmount();

   this->sharedDirs << QDir::currentPath().append("/../../aSharedDir");
   this->fileManager->setSharedDirsReadOnly(this->sharedDirs);

   QTest::qSleep(500);

   QString terms("xxxx");
   Protos::Common::FindResult result = this->fileManager->find(terms);
   this->printSearch(terms, result);
}

void Tests::cleanupTestCase()
{
}

void Tests::doASearch(bool checkResult)
{
   QString terms("aaaa bbbb cccc");
   Protos::Common::FindResult result = this->fileManager->find(terms);
   this->printSearch(terms, result);

   if (checkResult)
   {
      quint32 levelResults[] = {
         0, 1, 2, 3, 3, 4, 5, 5
      };
      QList<QString> fileResults[] = {
         QList<QString>() << "aaaa bbbb cccc.txt",
         QList<QString>() << "aaaa bbbb.txt",
         QList<QString>() << "aaaa cccc.txt",
         QList<QString>() << "cccc bbbbbb.txt" << "bbbb cccc.txt",
         QList<QString>() << "aaaa dddddd.txt",
         QList<QString>() << "bbbb dddd.txt" << "bbbb.txt"
      };
      for (int i = 0; i < result.files_size(); i++)
      {
         QVERIFY(result.files(i).level() == levelResults[i]);
         QVERIFY(fileResults[result.files(i).level()].contains(QString(result.files(i).file().file().name().data())));
      }
   }
}

void Tests::printSearch(const QString& terms, const Protos::Common::FindResult& result)
{
   qDebug() << "Search : '" << terms << "'";
   for (int i = 0; i < result.files_size(); i++)
      qDebug() << "[" << result.files(i).level() << "] " << result.files(i).file().file().name().data();
}

void Tests::printAmount()
{
   qDebug() << "Sharing amount : " << this->fileManager->getAmount() << " bytes";
}

