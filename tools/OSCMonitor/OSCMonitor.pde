import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;
final String fAddressPattern = "/sendungsbewusstsein";

void setup() {
    size(1024, 768);
    oscP5 = new OscP5(this, 7000);
    myRemoteLocation = new NetAddress("127.0.0.1", 7001);
}

void draw() {
    background(0);
}

void mousePressed() {
    OscMessage myMessage = new OscMessage(fAddressPattern);
    myMessage.add(123);
    myMessage.add(0.0);
    oscP5.send(myMessage, myRemoteLocation);

    print("+++ sent message. ");
    print("addrpattern: "+myMessage.addrPattern());
    println(" typetag: "+myMessage.typetag());
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
