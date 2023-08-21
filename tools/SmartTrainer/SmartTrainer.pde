/*
 * this example demonstrates how to connect to a Whoop smart trainer,
 * then read the power and set the resistence level.
 *
 * note that device, characteristic and feature indices are hard-coded
 * and might be differ on other smart trainers.
 */


import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress fRemoteLocation;

float mPower = 0;
float mResistence = 50;

void setup() {
    size(1024, 768);
    oscP5 = new OscP5(this, 7000);
    fRemoteLocation = new NetAddress("127.0.0.1", 7001);

    textFont(createFont("HelveticaNeue-Bold", 48));
    fill(255);
}

void draw() {
    background(50);
    text("Pwr " + nf(floor(mPower), 4, 0) + "W", 50, 100);
    text("Res " + nf(floor(mResistence), 2, 0) + "%", 50, 140);
}

void print_message(OscMessage mMessage) {
    print("+++ sent message. ");
    print("addrpattern: " + mMessage.addrPattern());
    println(" typetag: " + mMessage.typetag());
}

void scan_for_devices() {
    OscMessage mMessage = new OscMessage(SB_OSC_ADDRESS_PATTERN);
    mMessage.add(CMD_SCAN_FOR_DEVICES);
    mMessage.add(5000); // duration_in_milliseconds
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
}

void connect_device() {
    OscMessage mMessage = new OscMessage(SB_OSC_ADDRESS_PATTERN);
    mMessage.add(CMD_CONNECT_DEVICE);
    mMessage.add("Wahoo"); // device_name
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
}

void disconnect_device() {
    OscMessage mMessage = new OscMessage(SB_OSC_ADDRESS_PATTERN);
    mMessage.add(CMD_DISCONNECT_DEVICE);
    mMessage.add(ALL_DEVICES); // device_index ( -1 = ALL_DEVICES )
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
}

void set_feature_value(float value) {
    // from `DOCUMENTATION.md`:
    //```
    //command .... : set_feature_value(device_index,
    //                                 characteristic_index,
    //                                 (feature_index/feature_name),
    //                                 value)
    //typetag .... : iii(i/s)f
    //example .... : CMD_SET_FEATURE_VALUE,0,1,"power",42
    //```
    OscMessage mMessage = new OscMessage(SB_OSC_ADDRESS_PATTERN);
    mMessage.add(CMD_SET_FEATURE_VALUE);
    mMessage.add(0); // device_index
    mMessage.add(2); // characteristic_index
    mMessage.add("set_target_resistance_level"); // feature_name
    //    mMessage.add(0); // feature_index ( alternative way of accessing features )
    mMessage.add(value); // feature_index
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
    mResistence = value;
}

void mousePressed() {
    float mValue = map(mouseX, 0, width, 0, 100);
    set_feature_value(mValue);
}

void keyPressed() {
    switch(key) {
    case 's':
        scan_for_devices();
        break;
    case 'c':
        connect_device();
        break;
    case 'd':
        disconnect_device();
        break;
    }
}

void handle_send_feature_with_value(OscMessage message) {
    // from `DOCUMENTATION.md`:
    // ```
    // info ....... : send_feature_with_value(device_index,
    //                                        characteristic_index,
    //                                        feature_index,
    //                                        value)
    // typetag .... : iiiif
    // example .... : INFO_FEATURE_WITH_VALUE,0,1,"heartrate",52
    // ```
    if (message.typetag().equals("iiiif")) { // check typetag just to be sure
        int command =  message.get(0).intValue();
        int device_index = message.get(1).intValue();
        int characteristic_index = message.get(2).intValue();
        int feature_index = message.get(3).intValue();
        float value = message.get(4).floatValue();
        if (command == INFO_FEATURE_WITH_VALUE
            && device_index == 0
            && characteristic_index == 0
            && feature_index == 0) {
            mPower = value;
            println("power: " + value);
        }
    }
}

void oscEvent(OscMessage message) {
    if (message.addrPattern().equals(SB_OSC_ADDRESS_PATTERN)) {
        int command =  message.get(0).intValue();
        if (command == INFO_FEATURE_WITH_VALUE) {
            handle_send_feature_with_value(message);
        } else {
            print("### received 'sendungsbewusstsein' message ");
            print("with address pattern: '"+message.addrPattern());
            print("' + typetag: '"+message.typetag());
            println("' + command: " + message.get(0).intValue());
        }
    }
}
