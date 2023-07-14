#pragma once

#include <chrono>
#include <thread>

#include "OscSenderReceiver.h"

using namespace std;

class HeartBeat {
public:
    explicit HeartBeat() {
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

    void send_heartbeat() const {
        OscSenderReceiver::instance().send("heartbeat", (float) fFrequencyMillis);
    }

private:
    bool   fIsRunning;
    bool   fIsSending;
    int    fFrequencyMillis;
    thread fThread;

    void heartbeat_thread() const {
        while (fIsRunning) {
            if (fIsSending) {
                send_heartbeat();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(fFrequencyMillis));
        }
    }
};