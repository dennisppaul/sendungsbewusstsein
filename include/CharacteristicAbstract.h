#pragma once

#include "simpleble/SimpleBLE.h"
#include "Feature.h"

// TODO add distinction between capabilities i.e notify(aka subscribe/unsubscribe), read, and write
// TODO rename to `Characteristic` and add namespace
// TODO move to extra folder

class CharacteristicAbstract {
public:
    CharacteristicAbstract(shared_ptr<SimpleBLE::Peripheral> peripheral,
                           int connected_device_index,
                           int supported_characteristic_index) :
            fPeripheral(std::move(peripheral)),
            fConnectedDeviceIndex(connected_device_index),
            fSupportedCharacteristicIndex(supported_characteristic_index) {}

    virtual ~CharacteristicAbstract() = default;

    virtual void
    init() = 0; /* is called once in the begining and can implement default behaviors i.e subscribing to notifications */

    virtual void
    cleanup() = 0; /* is called once in the end and can implement default behaviors i.e unsubscribing from notifications */

    virtual void subscribe() = 0; /* ... to notification */

    virtual void unsubscribe() = 0;  /* ... from notification */

    virtual void read() = 0;

    virtual void write(SimpleBLE::ByteArray bytes) = 0;

    virtual const char *name() = 0;

    int get_feature_index_from_string(std::string &feature_name) {
        for (int i = 0; i < fFeatures.size(); ++i) {
            if (fFeatures[i].name() == feature_name) {
                return i;
            }
        }
        return NO_DEVICE_FOUND;
    }

    int get_number_of_supported_features() {
        return fFeatures.size();
    }

    Feature *get_feature(int feature_index) {
        if (feature_index < 0 || feature_index >= fFeatures.size()) {
            return nullptr;
        }
        return &fFeatures[feature_index];
    }

    void print_features() {
        if (!fFeatures.empty()) {
            console
                    << "characteristic "
                    << "'"
                    << name()
                    << "'"
                    << "["
                    << fSupportedCharacteristicIndex
                    << "]"
                    << " has features: "
                    << endl;
            for (int i = 0; i < fFeatures.size(); ++i) {
                console
                        << "    feature '"
                        << fFeatures[i].name()
                        << "'"
                        << "["
                        << i
                        << "]"
                        << endl;
            }
        } else {
            console
                    << "characteristic "
                    << "'"
                    << name()
                    << "'"
                    << "["
                    << fSupportedCharacteristicIndex
                    << "]"
                    << " has no features"
                    << endl;
        }
    }

protected:
    shared_ptr<SimpleBLE::Peripheral> fPeripheral;
    vector<Feature>                   fFeatures;
    const int                         fConnectedDeviceIndex;
    const int                         fSupportedCharacteristicIndex;

    void send(const int feature, const float value) const {
        // TODO consider senting not just feature index but also feature name
        Transceiver::instance()->send_feature_with_value(fConnectedDeviceIndex,
                                                         fSupportedCharacteristicIndex,
                                                         feature,
                                                         value);
    }
};
