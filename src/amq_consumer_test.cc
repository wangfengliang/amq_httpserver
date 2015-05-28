
#include <signal.h>
#include <iostream>
#include <stdio.h>

#include "amq_factory.h"

SimpleAMPFactory* pFactory = NULL;

void SigRoutine(int iSignNum) 
{ 
    fprintf(stderr, "capture SIGHUP signal, signal number is %d\n",iSignNum); 
    if(pFactory)
        pFactory->close();
} 

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

    std::cout << "=====================================================\n";
    std::cout << "Starting the example:" << std::endl;
    std::cout << "-----------------------------------------------------\n";

    std::string brokerURI = "failover:(tcp://192.168.1.53:61616,tcp://192.168.1.54:61616)?timeout=3000&reconnectSupported=true&randomize=false&priorityBackup=true&priorityURIs=tcp://192.168.1.53:61616";
    //std::string brokerURI = "failover:(tcp://192.168.1.53:61616)?timeout=3000&reconnectSupported=true&randomize=false&priorityBackup=true&priorityURIs=tcp://192.168.1.53:61616";

    //============================================================
    // This is the Destination Name and URI options.  Use this to
    // customize where the consumer listens, to have the consumer
    // use a topic or queue set the 'useTopics' flag.
    //============================================================
    std::string destURI_producer = "TEST.FOO";
    std::string destURI_consumer = "TEST.FOO?consumer.prefetchSize=1"; //?consumer.prefetchSize=1";
    
    int ret = 0;
    SimpleAMPFactory factory(brokerURI, destURI_producer, destURI_consumer, "test", "test", false, true);
    factory.initializeLibrary();
    factory.start();
    factory.sendTextMessage("hello, wolrd!");
    factory.sendTextMessage("5.28!");

    std::string data;
    ret = factory.recvTextMessage(data);
    std::cout<<" ret="<<ret<<" data="<<data<<std::endl;
    ret = factory.recvTextMessage(data);
    std::cout<<" ret="<<ret<<" data="<<data<<std::endl;
    factory.close();
    factory.shutdownLibrary();
    

    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\n";
}
