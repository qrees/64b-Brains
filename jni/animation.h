/*
 * animation.h
 *
 *  Created on: 23-06-2012
 *      Author: Krzysztof
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_


class Animation : public RefCntObject {
protected:
    unsigned int _start;
    unsigned int _deadline;
    unsigned int _delta;
public:
	Animation();
    Animation(unsigned int length);
    virtual ~Animation();
    /**
     * Processes single animation step.
     *
     * @return true if animation is finished and should be removed from animation list
     */
    virtual bool process(unsigned int) = 0;
    virtual bool finished(unsigned int);
};
typedef AutoPtr<Animation> AAnimation;

class EntityAnimation : public Animation {
private:
    ANode _node;
    ANode _start_value;
    ANode _end_value;
public:
    EntityAnimation(ANode, ANode, ANode, unsigned int length);
    EntityAnimation(ANode, ANode, unsigned int length);
    bool process(unsigned int);
};
typedef AutoPtr<EntityAnimation> AEntityAnimation;


#endif /* ANIMATION_H_ */
