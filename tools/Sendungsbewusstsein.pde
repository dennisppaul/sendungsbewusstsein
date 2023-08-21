public static final String  SB_OSC_ADDRESS_PATTERN = "/sendungsbewusstsein/";
public static final boolean SB_HAS_OSC = true;
public static final boolean SB_HAS_CLI = true;
public static final boolean SB_HAS_WATCHDOG = true;
public static final boolean SB_RUN_AS_CLI = true;

public static final int CMD_SCAN_FOR_DEVICES = 0;
public static final int CMD_CONNECT_DEVICE = 1;
public static final int CMD_DISCONNECT_DEVICE = 2;
public static final int CMD_SUBSCRIBE_TO_CHARACTERISTIC = 3;
public static final int CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC = 4;
public static final int CMD_GET_DEVICE_NAME = 5;
public static final int CMD_GET_CHARACTERISTIC_NAME = 6;
public static final int CMD_GET_FEATURE_NAME = 7;
public static final int CMD_GET_FEATURE_VALUE = 8;
public static final int CMD_SET_FEATURE_VALUE = 9;
public static final int NUM_COMMANDS = 10;

public static final CommandItem[] COMMAND_MAP = {
    new CommandItem(CMD_SCAN_FOR_DEVICES, "scan_for_devices", "ii", null),
    new CommandItem(CMD_CONNECT_DEVICE, "connect_device", "ii", "is"),
    new CommandItem(CMD_DISCONNECT_DEVICE, "disconnect_device", "ii", null),
    new CommandItem(CMD_SUBSCRIBE_TO_CHARACTERISTIC, "subscribe_to_characteristic", "iii", null),
    new CommandItem(CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC, "unsubscribe_from_characteristic", "iii", null),
    new CommandItem(CMD_GET_DEVICE_NAME, "get_device_name", "ii", null),
    new CommandItem(CMD_GET_CHARACTERISTIC_NAME, "get_characteristic_name", "iii", null),
    new CommandItem(CMD_GET_FEATURE_NAME, "get_feature_name", "iiii", null),
    new CommandItem(CMD_GET_FEATURE_VALUE, "get_feature_value", "iiii", "iiis"),
    new CommandItem(CMD_SET_FEATURE_VALUE, "set_feature_value", "iiiif", "iiisf")
};

public static final int ALL_DEVICES = -1;

public static final int INFO_ERROR = -1;
public static final int INFO_DEVICE = 0;
public static final int INFO_DEVICE_WITH_VALUE = 1;
public static final int INFO_CHARACTERISTIC = 2;
public static final int INFO_CHARACTERISTIC_WITH_VALUE = 3;
public static final int INFO_FEATURE_WITH_VALUE = 4;
public static final int INFO_WATCHDOG = 5;
public static final int NUM_INFORMATION = 6;

public static final InformationItem[] INFORMATION_MAP = {
    new InformationItem(INFO_DEVICE, "send_device_information", "iii"),
    new InformationItem(INFO_DEVICE_WITH_VALUE, "send_device_information_with_value", "iiii"),
    new InformationItem(INFO_CHARACTERISTIC, "send_characteristic_information", "iiii"),
    new InformationItem(INFO_CHARACTERISTIC_WITH_VALUE, "send_characteristic_information_with_value", "iiiii"),
    new InformationItem(INFO_FEATURE_WITH_VALUE, "send_feature_with_value", "iiisf"),
    new InformationItem(INFO_WATCHDOG, "send_watchdog", "ii")
};

public static final int ERROR = -1;
public static final int CONNECTED_DEVICE = 0;
public static final int DISCONNECT_DEVICE = 1;
public static final int NUMBER_OF_SUPPORTED_CHARACTERISTICS = 2;
public static final int SUBSCRIBED = 3;
public static final int UNSUBSCRIBED = 4;

static class CommandItem {
    public final int cmd;
    public final String name;
    public final String typetag;
    public final String typetag_alt;

    public CommandItem(int cmd, String name, String typetag, String typetag_alt) {
        this.cmd = cmd;
        this.name = name;
        this.typetag = typetag;
        this.typetag_alt = typetag_alt;
    }
}

static class InformationItem {
    public final int cmd;
    public final String name;
    public final String typetag;

    public InformationItem(int cmd, String name, String typetag) {
        this.cmd = cmd;
        this.name = name;
        this.typetag = typetag;
    }
}
