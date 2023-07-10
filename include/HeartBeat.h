#pragma once

#include <chrono>
#include <thread>

#include "OscManager.h"

using namespace std;

class HeartBeat {
public:
    explicit HeartBeat(OscManager &osc_manager) : fOscManager(osc_manager) {
        fIsSending       = false;
        fIsRunning       = true;
        fFrequencyMillis = 1000;
        fThread          = thread(&HeartBeat::heartbeat_thread, this);
    }

    ~HeartBeat() {
        fIsRunning = false;
        fThread.detach();
    }

    void start() {
        fIsSending = true;
    }

    void stop() {
        fIsSending = false;
    }

    void set_frequency(int frequency_millis) {
        fFrequencyMillis = frequency_millis;
    }

    void send_heartbeat() {
        fOscManager.send("heartbeat", (float)fFrequencyMillis);
    }


private:
    const OscManager &fOscManager;
    bool             fIsRunning;
    bool             fIsSending;
    int              fFrequencyMillis;
    thread           fThread;

    void heartbeat_thread() {
        while (fIsRunning) {
            if (fIsSending) {
                cout << "heart beat" << endl;
                send_heartbeat();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(fFrequencyMillis));
        }
    }
};