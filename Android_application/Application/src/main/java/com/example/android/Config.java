package com.example.android;

public class Config {
    public static final int stringCount = 50;
    public static final String OPEN_DOOR = "{openDoor}";
    public static final String READ_TOGGLE_ON = "{readOn}";
    public static final String READ_TOGGLE_OFF = "{readOff}";
    public static final String SYNCH_USER = "{syncUser}";
    public static final String HISTORY = "{history}";
    public static final String WRITE_TOGGLE_OFF = "{writeOff}";
    public static final String INITIAL_AUTH = "{auth}";


    public static String getLastString(String string) {
        int stringLength = string.length();
        StringBuilder stringBuilder = new StringBuilder(string);
        for (int i = 0; i < stringCount - stringLength; i++) {
            stringBuilder.append(" ");
        }
        return stringBuilder.toString();
    }
}
