#ifndef ReadWriteMutex_h
#define ReadWriteMutex_h

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>

class ReadWriteMutex : boost::noncopyable
{
    public:
        ReadWriteMutex();
        // local class has access to ReadWriteMutex private members, as required
        class ScopedReadLock : boost::noncopyable
        {
            public:
                ScopedReadLock(ReadWriteMutex& rwLock) : m_rwLock(rwLock)
                {
                    m_rwLock.acquireReadLock();
                }
                ~ScopedReadLock()
                {
                    m_rwLock.releaseReadLock();
                }
            private:
                ReadWriteMutex& m_rwLock;
        };
        class ScopedWriteLock : boost::noncopyable
        {
            public:
                ScopedWriteLock(ReadWriteMutex& rwLock) : m_rwLock(rwLock)
                {
                    m_rwLock.acquireWriteLock();
                }
                ~ScopedWriteLock()
                {
                    m_rwLock.releaseWriteLock();
                }
            private:
                ReadWriteMutex& m_rwLock;
        };

    private: // data
        boost::mutex m_mutex;
        unsigned int m_readers;
        boost::condition m_noReaders;
        unsigned int m_pendingWriters;
        bool m_currentWriter;
        boost::condition m_writerFinished;

    private: // internal locking functions
        void acquireReadLock();
        void releaseReadLock();
        // this function is currently not exception-safe:
        // if the wait calls throw, m_pendingWriter can be left in an inconsistent state
        void acquireWriteLock();
        void releaseWriteLock();
};

#endif
