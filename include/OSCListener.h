#pragma once

#include <iostream>

#include "osc/OscPacketListener.h"

#include "Transceiver.h"

using namespace std;

class OSCListener : public osc::OscPacketListener {

public:
    OSCListener(Transceiver *manager) : fManager(manager) {}

private:
    Transceiver *fManager;

protected:
    void ProcessMessage(const osc::ReceivedMessage &msg, const IpEndpointName &remoteEndpoint) override;
};