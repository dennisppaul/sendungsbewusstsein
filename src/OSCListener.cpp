#include "OSCListener.h"

void OSCListener::ProcessMessage(const osc::ReceivedMessage &msg,
                                 const IpEndpointName &remoteEndpoint) {
    (void) remoteEndpoint;  // suppress unused parameter warning
    try {
        fManager->process(msg);
    } catch (osc::Exception &e) {
#ifdef DEBUG_OSC
        cerr << "@OSC error in OSC listener: " << msg.AddressPattern() << "" << e.what();
#endif
    }
}
