#pragma once

#include <chrono>
#include <thread>

#include "OscSenderReceiver.h"

using namespace std;

class Watchdog {
public:
    explicit Watchdog() {
        fIsSending       = false;
        fIsRunning       = true;
        fFrequencyMillis = 1000;
        fThread          = thread(&Watchdog::heartbeat_thread, this);
    }

    ~Watchdog() {
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
        OscSenderReceiver::instance()->send_watchdog(fFrequencyMillis);
    }

private:
    bool   fIsRunning;
    bool   fIsSending;
    int    fFrequencyMillis;
    thread fThread;

    void heartbeat_thread() const {
        while (fIsRunning) {
            if (fIsSending && fFrequencyMillis > 0) {
                send_heartbeat();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(fFrequencyMillis));
        }
    }
};