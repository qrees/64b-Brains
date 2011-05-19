/*
 * smart_ptr.h
 *
 *  Created on: 2011-05-16
 *      Author: qrees
 */

#ifndef SMART_PTR_H_
#define SMART_PTR_H_

class RefCntObject {
public:
    virtual ~RefCntObject() {
    }

    RefCntObject() :
        m_refcnt(0) {
    }

    void addRef() {
        m_refcnt++;
    }

    bool subRef() {
        return (--m_refcnt <= 0);
    }

private:
    int m_refcnt;
};

template<class T>
class AutoPtr {
public:
    AutoPtr(T* ptr = 0) :
        m_ptr(ptr) {
        addRef();
    }

    template<class OTHER>
    AutoPtr(const AutoPtr<OTHER>& other) {
        T* foo = other;
        m_ptr = (T*)(other.m_ptr);
        addRef();
    }
    AutoPtr(const AutoPtr& p) :
        m_ptr(p.m_ptr) {
        addRef();
    }

    ~AutoPtr() {
        subRef();
    }

    AutoPtr& operator=(const AutoPtr& p) {
        return *this = p.m_ptr;
    }

    AutoPtr& operator=(T* ptr) {
        if (m_ptr != ptr) {
            subRef();
            m_ptr = ptr;
            addRef();
        }
        return *this;
    }

    T& operator *() const {
        return *m_ptr;
    }

    T* operator ->() const {
        return m_ptr;
    }

    operator T*() const {
        return m_ptr;
    }


    operator const T*() const {
        return m_ptr;
    }

    operator bool() const {
        return m_ptr != 0;
    }
/*
    T** operator &() {
        return &m_ptr;
    }
*/
private:
    void addRef() {
        if (m_ptr)
            m_ptr->addRef();
    }

    void subRef() {
        if (m_ptr) {
            if (m_ptr->subRef()) {
                delete m_ptr;
                m_ptr = 0;
            }
        }
    }

    T* m_ptr;
};

#endif /* SMART_PTR_H_ */
