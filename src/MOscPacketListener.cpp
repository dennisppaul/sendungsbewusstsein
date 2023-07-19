#include "MOscPacketListener.h"

void MOscPacketListener::ProcessMessage(const osc::ReceivedMessage &msg,
                                        const IpEndpointName &remoteEndpoint) {
    (void) remoteEndpoint;  // suppress unused parameter warning
    try {
        cout << "process: " << msg.AddressPattern() << endl;
        fManager->process(msg);
//            if (addr_pattern_equals(msg, KLANG_OSC_CMD)) {
//                process_KLANG_OSC_CMD(msg);
//            } else if (addr_pattern_equals(msg, KLANG_OSC_DATA)) {
//                process_KLANG_OSC_DATA(msg);
//            } else if (addr_pattern_equals(msg, KLANG_OSC_MIDI_IN)) {
//                process_KLANG_OSC_MIDI_IN(msg);
//            } else if (addr_pattern_equals(msg, KLANG_OSC_SERIAL)) {
//                process_KLANGSTROM_OSC_SERIAL(msg);
//            } else if (addr_pattern_equals(msg, KLANG_OSC_SIM)) {
//                mEmulator.process_receive(msg);
//#ifdef DEBUG_OSC
//                } else {
//                KLANG_LOG("@sdlApp onReceive: %s, %d", msg.AddressPattern(), msg.ArgumentCount());
//#endif
//            }
    } catch (osc::Exception &e) {
#ifdef DEBUG_OSC
        cerr << "@OSC error in OSC listener: " << msg.AddressPattern() << "" << e.what();
#endif
    }
}