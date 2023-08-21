// TODO not working ATM

import oscP5.*;
import netP5.*;

final String SB_ADDRESS_PATTERN = "/sendungsbewusstsein";
final String CHARACTERISTIC = "cycling_power_measurement";
final String CHARACTERISTIC_TYPE_TAG = "issf";
final String FEATURE_CRANK = "crank_revolutions";
final String FEATURE_POWER = "power";

OscP5 oscP5;
NetAddress myRemoteLocation;
float mCrank = 0;
float mPower = 0;
float x = 0;

void setup() {
    size(1024, 768);
    background(255);
    oscP5 = new OscP5(this, 7000);
    myRemoteLocation = new NetAddress("127.0.0.1", 7001);
}

void draw() {
    x+= 1.0 / frameRate * 10.0;
    if (x > width) {
        x = 0;
        background(255);
    }

    noFill();
    stroke(0, 127, 255);
    point(x, map(mCrank, 0, 120, height - 10, 10));
    stroke(255, 127, 0);
    point(x, map(mPower, 0, 400, height - 10, 10));
}

void handle_cycling_power_features(int index, String feature, float value) {
    if (feature.equalsIgnoreCase(FEATURE_CRANK)) {
        mCrank = value;
    } else if (feature.equalsIgnoreCase(FEATURE_POWER)) {
        mPower = value;
    }
}

void oscEvent(OscMessage message) {
    if (message.addrPattern().startsWith(SB_ADDRESS_PATTERN)) {
        if (message.checkTypetag(CHARACTERISTIC_TYPE_TAG)) {
            final String mCharacteristic = message.get(1).stringValue();
            if (mCharacteristic.equalsIgnoreCase(CHARACTERISTIC)) {
                final int mIndex = message.get(0).intValue();
                final String mFeature = message.get(2).stringValue();
                final float mValue = message.get(3).floatValue();
                handle_cycling_power_features(mIndex, mFeature, mValue);
            }
        }
    }
}
