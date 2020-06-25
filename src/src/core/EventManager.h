/*
* Copyright (c) 2020 Hunesion Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; 
* either version 2.1 of the License, or (at your option) any later version.
* See the file COPYING included with this distribution for more information.
* https://github.com/HuneOpenUp/ossFileTransferClient/blob/master/COPYING
*/


#ifndef _FTC_CORE_EVENT_MANAGER_H_
#define _FTC_CORE_EVENT_MANAGER_H_
#include <functional>
#include <map>
#include <list>
#include "macro.h"


BEGIN_FTC_CORE

class Event ; 
class EventListener ; 
class EventManager; 

typedef const std::function<void(Event*)> EVENT_CALLBACK; 

class Event {
public: 
    static Event *create(const char *eventName, void* _userData = nullptr); 
    static Event *create(const std::string &eventName , void* _userData = nullptr); 
    virtual ~Event(); 
    const std::string &getName(){ return _name; }
    void *getUserData() {return _userData; }
    int incRefCount(){
        ++(this->_refCount); 
        return this->_refCount; 
    }
    void decRefCount(){
        --(this->_refCount); 
        if (this->_refCount <= 0) delete this; 
    }

    

private: 
    Event(const std::string &eventName, void *userData); 
    

    std::string _name ; 
    void *_userData ; 
    int _refCount ; 

}; 

class EventListener{
public : 
    static EventListener *create(void *target , const char *eventName , const EVENT_CALLBACK &callback); 
    static EventListener *create(void *target, const std::string &eventName , const EVENT_CALLBACK &callback); 
    virtual ~EventListener(); 
    const std::string &getEventName(){return this->_eventName; }
    void *getTarget(){return this->_target; }
    const EVENT_CALLBACK &getCallback(){return this->_callback;}
    int getRefCount(){ return this->_refCount ; }
    int incRefCount(){ ++(this->_refCount); return this->_refCount ; }
    void decRefCount(){ 
        --(this->_refCount) ; 
        if (this->_refCount <= 0) delete this ; 
    }

private : 
    EventListener(void *target , const std::string &eventName , const EVENT_CALLBACK &callback); 
    
    std::string _eventName ; 
    void* _target ;  
    std::function<void(Event *event)> _callback ; 
    int _refCount ; 
}; 


class EventManager{
public: 
    static EventManager *getInstance(); 
    static void destroyInstance(); 
    bool addEventListener(void *target , const char *eventName , const EVENT_CALLBACK &callback);
    bool dispatchEvent(const char *eventName , void *userData = nullptr ) ;
    bool dispatchEventAsync(const char *eventName, void *userData = nullptr , void (*release)(void*) = nullptr);  
    void setAsyncEventCallback(bool (*asynchEventCallback)(const char *eventName , void *userData , void (*release)(void*))); 
    bool removeEventListener(void *target, const char *eventName = nullptr); 
    void dispose(); 

protected:
    EventManager(); 
    ~EventManager(); 
    bool hasEventListener(void *target, const char *eventName); 
    bool addEventListenerToEventMap(EventListener *listener); 
    bool addEventListenerToTargetMap(EventListener *listener); 
    bool removeEventListenerFromEventMap(EventListener *listener) ; 
    bool removeEventListenerFromEventMap(const char *eventName, void *target); 
    bool removeEventListenerFromTargetMap(void *target, const char *eventName); 
    

private:
    static EventManager *s_Instance ; 
    std::map<std::string , std::list<EventListener *>> _eventMap ; 
    std::map<void *, std::list<EventListener*>> _targetMap ; 
    bool (*_asyncEventCallback)(const char *eventName , void *userData , void (*release)(void*)); 
}; 
END_FTC_CORE
#endif /* _FTC_CORE_EVENT_MANAGER_H_ */