/*
 * event.h
 *
 *  Created on: 2011-06-10
 *      Author: qrees
 */

#ifndef EVENT_H_
#define EVENT_H_


class Event : public RefCntObject {
public:
    Event();
    virtual ~Event();
    virtual void process(Scene &scene) = 0;
};
typedef AutoPtr<Event> AEvent;

/** ClickEvent class
 * 
 * Represents single click event.
 */
class ClickEvent : public Event {
private:
    int _x, _y;
    AEntity _mesh;
public:
    /** Constructor for ClickEvent.
     * 
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    ClickEvent(AEntity, int x, int y);
    
    /**
     * Should be called by Scene class when event should be processed.
     * 
     * @param scene Scene that is going to be affected by this event.
     */
    void process(Scene &scene);
};

/**
 * Represents move of the pointing device on the surface (preceded by ClickEvent)
 */
class MoveEvent : public Event {
private:
    int _x, _y;
public:
    /** Constructor for MoveEvent.
     *
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    MoveEvent(int x, int y);

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(Scene &scene);
};

/**
 * Represents lifting of the pointing device.
 */
class UpEvent : public Event {
private:
    int _x, _y;
public:
    /** Constructor for MoveEvent.
     *
     * @param x Horizontal coordinate of click event
     * @param y Vertical coordinate of click event
     */
    UpEvent(int x, int y);

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(Scene &scene);
};

/**
 * Stops processing of event queue for given scene.
 */
class InvalidateScene : public Event {
public:
    /** Constructor for InvalidateScene.
     */
    InvalidateScene();

    /**
     * Should be called by Scene class when event should be processed.
     *
     * @param scene Scene that is going to be affected by this event.
     */
    void process(Scene &scene);
};
#endif /* EVENT_H_ */
