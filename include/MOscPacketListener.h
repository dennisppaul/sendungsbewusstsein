#pragma once

#include <iostream>

#include "osc/OscPacketListener.h"

#include "OscManager.h"

using namespace std;

class MOscPacketListener : public osc::OscPacketListener {

public:
    MOscPacketListener(OscManager *manager) : fManager(manager) {}

private:
    OscManager *fManager;

protected:
    void ProcessMessage(const osc::ReceivedMessage &msg, const IpEndpointName &remoteEndpoint) override;
};