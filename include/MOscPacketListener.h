#pragma once

#include <iostream>

#include "osc/OscPacketListener.h"

#include "OscSenderReceiver.h"

using namespace std;

class MOscPacketListener : public osc::OscPacketListener {

public:
    MOscPacketListener(OscSenderReceiver *manager) : fManager(manager) {}

private:
    OscSenderReceiver *fManager;

protected:
    void ProcessMessage(const osc::ReceivedMessage &msg, const IpEndpointName &remoteEndpoint) override;
};