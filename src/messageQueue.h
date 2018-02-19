//
//  messageQueue.h
//  dreamonway
//
//  Created by Kunal Parihar on 15/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

//#ifndef messageQueue_h
//#define messageQueue_h


//#endif /* messageQueue_h */
#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <pthread.h>
#include <queue>

using namespace std;

template <typename T>
class messageQueue{
private:
    queue<T> items; /*!< basic data buffer for storing items */
    pthread_mutex_t qMutex;/*!< internal mutex for handling queries */
    pthread_cond_t cMutex; /*!< conditional variable for handling mutexes */
public:
    messageQueue(); //!< ctor statically initializes mutex and conditional variable
    //!< utility for adding items to the queue
    /*!
     \param item to insert as argument
     */
    void addItem(T item);
    //!< removes an item from a queue (in FIFO order)
    /*!
     \result returns an item from the queue (the first item to be inserted)
     */
    T remove();
};

template <typename T>
messageQueue<T>::messageQueue()
{
    pthread_mutex_init(&qMutex, NULL);
    pthread_cond_init(&cMutex, NULL);
}

template <typename T>
void messageQueue<T>::addItem(T item)
{
    pthread_mutex_lock(&qMutex);
    items.push(item);
    pthread_cond_signal(&cMutex);
    pthread_mutex_unlock(&qMutex);
}

template <typename T>
T messageQueue<T>::remove()
{
    pthread_mutex_lock(&qMutex);
    while (items.size() == 0)
    {
        pthread_cond_wait(&cMutex, &qMutex);
    }
    T item = items.front();
    items.pop();
    pthread_mutex_unlock(&qMutex);
    return item;
}

#endif
