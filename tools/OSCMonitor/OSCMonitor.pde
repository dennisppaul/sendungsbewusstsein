import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress fRemoteLocation;
final String fAddressPattern = "/sendungsbewusstsein";

void setup() {
    size(1024, 768);
    oscP5 = new OscP5(this, 7000);
    fRemoteLocation = new NetAddress("127.0.0.1", 7001);
}

void draw() {
    background(0);
}

void mousePressed() {
    OscMessage mMessage = new OscMessage(fAddressPattern);
    mMessage.setAddrPattern("sendungsbewusstsein");
    mMessage.add("hello");
    mMessage.add(1.2f);
    mMessage.add(3.4d);
    mMessage.add(123);
    mMessage.add(true);
    mMessage.add('a');
    mMessage.add((long)(456));
    mMessage.add((byte)(100));
    oscP5.send(mMessage, fRemoteLocation);

    print("+++ sent message. ");
    print("addrpattern: "+mMessage.addrPattern());
    println(" typetag: "+mMessage.typetag());
}

void oscEvent(OscMessage theOscMessage) {
    if (theOscMessage.addrPattern().startsWith(fAddressPattern)) {
        print("### received message. ");
        print("addrpattern: "+theOscMessage.addrPattern());
        println(" typetag: "+theOscMessage.typetag());
    } else {
        println("### received unknown osc message.");
    }
    //if (theOscMessage.checkTypetag("i")) {
    //    println("received .... : " + theOscMessage.get(0).intValue() +
    //        "(" + "" + nf(hour(), 2)+ ":" +nf( minute(), 2)+ ":" + nf(second(), 2) + ")");
    //}
}
