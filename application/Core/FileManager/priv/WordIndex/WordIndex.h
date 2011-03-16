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
  
#ifndef FILEMANAGER_WORDINDEX_H
#define FILEMANAGER_WORDINDEX_H

#include <QList>
#include <QSet>
#include <QString>
#include <QChar>
#include <QMutex>

#include <Common/Uncopyable.h>

#include <priv/WordIndex/Node.h>

namespace FM
{
   /**
     * An collection of T indexed by word.
     */
   template<typename T>
   class WordIndex : Common::Uncopyable
   {
      static const int MIN_WORD_SIZE_PARTIAL_MATCH; ///< During a search, the words which have a size below this value must match entirely, for exemple 'of' match "conspiracy of one" and not "offspring".
   public:
      WordIndex();

      void addItem(const QStringList& words, T item);
      void rmItem(const QStringList& words, T item);
      QSet< NodeResult<T> > search(const QStringList& words) const;
      QSet< NodeResult<T> > search(const QString& word) const;

   private:
      Node<T> node;
      mutable QMutex mutex;
   };
}

/***** Definitions *****/
using namespace FM;

template<typename T>
const int WordIndex<T>::MIN_WORD_SIZE_PARTIAL_MATCH(3);

template<typename T>
WordIndex<T>::WordIndex()
{}

template<typename T>
void WordIndex<T>::addItem(const QStringList& words, T item)
{
   QMutexLocker locker(&mutex);

   for (QListIterator<QString> i(words); i.hasNext();)
   {
      const QString& word = i.next();
      Node<T>* currentNode = &this->node;
      for (int j = 0; j < word.size(); j++)
         currentNode = &currentNode->addNode(word[j]);
      currentNode->addItem(item);
   }
}

template<typename T>
void WordIndex<T>::rmItem(const QStringList& words, T item)
{
   QMutexLocker locker(&mutex);

   for (QListIterator<QString> i(words); i.hasNext();)
   {
      const QString& word = i.next();
      QList<Node<T>*> nodes;
      Node<T>* currentNode = &this->node;
      nodes.prepend(currentNode);
      for (int j = 0; j < word.size(); j++)
      {
         if (!(currentNode = currentNode->getNode(word[j])))
            goto nextWord;
         nodes.prepend(currentNode);
      }

      currentNode->rmItem(item);

      if (!currentNode->haveChildren())
      {
         Node<T>* nodeToRemove = 0;
         for (QListIterator<Node<T>*> i(nodes); i.hasNext();)
         {
            Node<T>* n = i.next();
            if (nodeToRemove)
            {
               n->rmNode(nodeToRemove);
               delete nodeToRemove;
            }

            if (n->haveItems() || n->haveChildren())
               break;
            else
               nodeToRemove = n;
         }
      }

      nextWord :;
   }
}

template<typename T>
QSet< NodeResult<T> > WordIndex<T>::search(const QStringList& words) const
{
   QMutexLocker locker(&mutex);

   QSet< NodeResult<T> > result;
   foreach (QString word, words)
   {
      const Node<T>* currentNode = &this->node;
      foreach (QChar letter, word)
      {
         if (!(currentNode = currentNode->getNode(letter)))
            goto nextWord;
      }

      result += currentNode->getItems(word.size() >= MIN_WORD_SIZE_PARTIAL_MATCH);
      nextWord :;
   }
   return result;
}

template<typename T>
QSet< NodeResult<T> > WordIndex<T>::search(const QString& word) const
{
   return this->search(QStringList() << word);
}

#endif
