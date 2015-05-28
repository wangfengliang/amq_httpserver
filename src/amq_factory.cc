
#include <assert.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>

#include "amq_factory.h"

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;


class SimpleProducer {
public:

    SimpleProducer(const std::string& brokerURI, const std::string& destURI, const string& username, const string& passwd, bool useTopic=false, bool clientAck = true)
      :brokerURI(brokerURI), destURI(destURI), userName(username), passwd(passwd), useTopic(useTopic), clientAck(clientAck) {
        connection = NULL;
        session = NULL;
	destination = NULL;
	producer = NULL;
    }
    virtual ~SimpleProducer() {
	close();
    }

    int start() {
	// Create a ConnectionFactory
        auto_ptr<ActiveMQConnectionFactory> connectionFactory(
            new ActiveMQConnectionFactory( brokerURI, userName, passwd) );

        // Create a Connection
        try{
            connection = connectionFactory->createConnection();
            connection->start();
        } catch( CMSException& e ) {
            e.printStackTrace();
            throw e;
        }

        // Create a Session
        if( clientAck ) {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        // Create the destination (Topic or Queue)
        if( useTopic ) {
            destination = session->createTopic( destURI );
        } else {
            destination = session->createQueue( destURI );
        }

        // Create a MessageProducer from the Session to the Topic or Queue
        producer = session->createProducer( destination );
        //producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );
        producer->setDeliveryMode( DeliveryMode::PERSISTENT );

        threadIdStr = Long::toString( Thread::currentThread()->getId() );
	return 0;
 
    }
    int close() {
        cleanup();
        return 0;
    }
    int cleanup() {
        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        destination = NULL;
    
        try{
            if( producer != NULL ) delete producer;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        producer = NULL;
    
        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch ( CMSException& e ) { e.printStackTrace(); }
    
        try{
            if( session != NULL ) delete session;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        session = NULL;
    
        try{
            if( connection != NULL ) delete connection;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        connection = NULL;
	return 0;
    }

    int sendTextMessage(const std::string& text) 
    {
        try {
            TextMessage* message = session->createTextMessage(text);
            producer->send( message );
            delete message;
            return 0;
        } catch ( CMSException& e ) { 
            e.printStackTrace(); 
        }
        return -1;
    }
    int sendBytesMessage(const std::string& text) 
    {
        try {
            //BytesMessage* message = session->createBytesMessage((const unsigned char*)text.c_str(), text.size());
            BytesMessage* message = session->createBytesMessage();
	    message->writeString(text);
            producer->send( message );
            delete message;
            return 0;
        } catch ( CMSException& e ) { 
            e.printStackTrace(); 
        }
        return -1;
    }

private:
    SimpleProducer( const SimpleProducer& );
    SimpleProducer& operator= ( const SimpleProducer& );

    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    bool useTopic;
    bool clientAck;
    unsigned int numMessages;
    std::string brokerURI;
    std::string destURI;
    std::string threadIdStr;

    std::string userName;
    std::string passwd;
};

class SimpleSyncConsumer {
public:
    SimpleSyncConsumer(  const std::string& brokerURI,
                         const std::string& destURI,
			 const std::string& username="",
			 const std::string& passwd="",
                         bool useTopic = false,
                         bool clientAck = true)
      : brokerURI(brokerURI), destURI(destURI), userName(username),passwd(passwd), useTopic(useTopic), clientAck(clientAck) {
    }
    virtual ~SimpleSyncConsumer() {
        close();
    }

    void start() {
       try {
	    // Create a ConnectionFactory
            auto_ptr<ActiveMQConnectionFactory> connectionFactory(
                new ActiveMQConnectionFactory( brokerURI, userName, passwd) );

            // Create a Connection
            try{
                connection = connectionFactory->createConnection();
                connection->start();
            } catch( CMSException& e ) {
                e.printStackTrace();
                throw e;
            }
   
            // Create a Session
            if( clientAck ) {
                session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
            } else {
                session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
            }
    
            // Create the destination (Topic or Queue)
            if( useTopic ) {
                destination = session->createTopic( destURI );
            } else {
                destination = session->createQueue( destURI );
            }
    
            // Create a MessageConsumer from the Session to the Topic or Queue
            consumer = session->createConsumer( destination );

       } catch (CMSException& e) {
            e.printStackTrace();
       }
    }
    void close() {
         cleanup();
    }
    void cleanup() {
        //*************************************************
        // Always close destination, consumers and producers before
        // you destroy their sessions and connection.
        //*************************************************
    
        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch (CMSException& e) {}
        destination = NULL;
    
        try{
            if( consumer != NULL ) delete consumer;
        }catch (CMSException& e) {}
        consumer = NULL;
    
        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch (CMSException& e) {}
    
        // Now Destroy them
        try{
            if( session != NULL ) delete session;
        }catch (CMSException& e) {}
        session = NULL;
    
        try{
            if( connection != NULL ) delete connection;
        }catch (CMSException& e) {}
        connection = NULL;
    }

    int recvTextMessage(std::string& data) {
        try{
            Message *message = consumer->receive();
            if(!message) { 
                return -1;
            }
            const TextMessage* textMessage = dynamic_cast< const TextMessage* >( message );
            if(!textMessage) {
        	return -1;
            }
            data = textMessage->getText();
            if(clientAck)
                message->acknowledge();
            delete message;
    	    return 0;
        } catch (CMSException& e) {
            e.printStackTrace();
        }
        return -1;
    }

    int recvBytesMessage(std::string& data)
    {
        try{
            Message *message = consumer->receive();
            if(!message) { 
                return -1;
            }
            const BytesMessage* bytesMessage = dynamic_cast< const BytesMessage* >( message );
            if(!bytesMessage) {
        	return -1;
            }
            data = bytesMessage->readString();
            if(clientAck)
                message->acknowledge();
            delete message;
    	    return 0;
        } catch (CMSException& e) {
            e.printStackTrace();
        }
        return -1;
     }

private:

    SimpleSyncConsumer( const SimpleSyncConsumer& );
    SimpleSyncConsumer& operator= ( const SimpleSyncConsumer& );

    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    bool useTopic;
    std::string brokerURI;
    std::string destURI;
    bool clientAck;

    std::string userName;
    std::string passwd;

};


bool SimpleAMPFactory::lib_initialized = false;

SimpleAMPFactory::SimpleAMPFactory(const std::string& brokerURI, const std::string& destURI_producer, const std::string& destURI_consumer, const string& username, const string& passwd, bool useTopic, bool clientAck)
  :brokerURI(brokerURI), destURI_producer(destURI_producer), destURI_consumer(destURI_consumer), userName(username), passwd(passwd), useTopic(useTopic), clientAck(clientAck)
{
    is_started = false;
}

SimpleAMPFactory::~SimpleAMPFactory()
{
}

void SimpleAMPFactory::initializeLibrary()
{
    if(!lib_initialized)
        activemq::library::ActiveMQCPP::initializeLibrary();
    lib_initialized = true;
}

void SimpleAMPFactory::shutdownLibrary()
{
    if(lib_initialized)
        activemq::library::ActiveMQCPP::shutdownLibrary();
    lib_initialized = false;
}

int SimpleAMPFactory::start()
{
    if(!is_started) {
    	producer = new SimpleProducer(brokerURI, destURI_producer, userName, passwd, useTopic, clientAck);
    	consumer = new SimpleSyncConsumer(brokerURI, destURI_consumer, userName, passwd, useTopic, clientAck);
    	assert(producer);
    	assert(consumer);

    	producer->start();
    	consumer->start();
	is_started = true;
    }

}

int SimpleAMPFactory::close()
{
    if(is_started) {
    	producer->close();
   	consumer->close();
    }
    is_started = false;
}

int SimpleAMPFactory::sendTextMessage(const std::string& text)
{
    assert(lib_initialized);
    assert(is_started);
    if(!producer)
        return -1;
    return producer->sendTextMessage(text);
}

int SimpleAMPFactory::recvTextMessage(std::string& text)
{
    assert(lib_initialized);
    assert(is_started);

    if(!consumer)
        return -1;
    return consumer->recvTextMessage(text);
}

int SimpleAMPFactory::sendBytesMessage(const std::string& text)
{
    assert(lib_initialized);
    assert(is_started);

    if(!producer)
        return -1;
    return producer->sendBytesMessage(text);
}

int SimpleAMPFactory::recvBytesMessage(std::string& text)
{
    assert(lib_initialized);
    assert(is_started);

    if(!consumer)
        return -1;
    return consumer->recvBytesMessage(text);
}

