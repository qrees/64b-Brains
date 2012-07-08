/*
 * event.h
 *
 *  Created on: 2011-06-10
 *      Author: qrees
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "smart_ptr.h"

class Event : public RefCntObject {
public:
    Event();
    virtual ~Event();
    virtual void process(RefCntObject &obj) = 0;
};
typedef AutoPtr<Event> AEvent;

#endif /* EVENT_H_ */
