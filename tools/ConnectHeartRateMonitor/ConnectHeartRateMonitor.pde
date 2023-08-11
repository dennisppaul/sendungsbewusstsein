import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress fRemoteLocation;
final String SB_ADDRESS_PATTERN = "/sendungsbewusstsein/";
final int CMD_SCAN_FOR_DEVICES  = 0;
final int CMD_CONNECT_DEVICE    = 1;
final int CMD_DISCONNECT_DEVICE = 2;
final int CMD_GET_DEVICE_NAME   = 5;
final int CMD_GET_FEATURE_NAME  = 7;

final int INFO_DEVICE                    = 0;
final int INFO_CHARACTERISTIC_WITH_VALUE = 3;
final int INFO_FEATURE_WITH_VALUE        = 4;

final int ALL_DEVICES                    = -1;

void setup() {
    size(1024, 768);
    oscP5 = new OscP5(this, 7000);
    fRemoteLocation = new NetAddress("127.0.0.1", 7001);
}

void draw() {
    background(50);
}

void print_message(OscMessage mMessage) {
    print("+++ sent message. ");
    print("addrpattern: " + mMessage.addrPattern());
    println(" typetag: " + mMessage.typetag());
}

void scan_for_devices() {
    OscMessage mMessage = new OscMessage(SB_ADDRESS_PATTERN);
    mMessage.add(CMD_SCAN_FOR_DEVICES);
    mMessage.add(5000); // duration_in_milliseconds
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
}

void connect_device() {
    OscMessage mMessage = new OscMessage(SB_ADDRESS_PATTERN);
    mMessage.add(CMD_CONNECT_DEVICE);
    mMessage.add("WHOOP"); // device_name
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
}


void disconnect_device() {
    OscMessage mMessage = new OscMessage(SB_ADDRESS_PATTERN);
    mMessage.add(CMD_DISCONNECT_DEVICE);
    mMessage.add(ALL_DEVICES); // device_index ( -1 = ALL_DEVICES )
    oscP5.send(mMessage, fRemoteLocation);
    print_message(mMessage);
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

void oscEvent(OscMessage message) {
    if (message.addrPattern().equals(SB_ADDRESS_PATTERN)) {
        int mInfoCommand =  message.get(0).intValue();
        if (mInfoCommand == INFO_FEATURE_WITH_VALUE) {
            // from `DOCUMENTATION.md`:
            // 
            // info ....... : send_feature_with_value(device_index,
            //                                        characteristic_index,
            //                                        (feature_index/feature_name),
            //                                        value)
            // typetag .... : iii(i/s)f
            float mValue = message.get(4).floatValue();
            println("value: " + mValue);
        } else {
            print("### received 'sendungsbewusstsein' message ");
            print("with address pattern: '"+message.addrPattern());
            print("' + typetag: '"+message.typetag());
            println("' + command: " + message.get(0).intValue());
        }
    }
}
