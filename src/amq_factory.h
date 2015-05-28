
#ifndef _AMP_FACTORY_H_
#define _AMP_FACTORY_H_

#include <string>
using namespace std;

class SimpleProducer;
class SimpleSyncConsumer;

class SimpleAMPFactory 
{
public:
    SimpleAMPFactory(const std::string& brokerURI, const std::string& destURI_producer, const std::string& destURI_consumer, const string& username, const string& passwd, bool useTopic=false, bool clientAck = true);
    ~SimpleAMPFactory();

    static void initializeLibrary();
    static void shutdownLibrary();

    int start();
    int close();

    int sendTextMessage(const std::string& text);
    int recvTextMessage(std::string& text);

    int sendBytesMessage(const std::string& text);
    int recvBytesMessage(std::string& text);

private:
    static bool lib_initialized;
    bool is_started;

    std::string brokerURI;
    std::string destURI_producer;
    std::string destURI_consumer;
    std::string userName;
    std::string passwd;
    bool useTopic;
    bool clientAck;

    SimpleProducer* producer;
    SimpleSyncConsumer* consumer;
};

#endif

