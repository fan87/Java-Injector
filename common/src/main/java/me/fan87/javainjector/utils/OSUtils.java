package me.fan87.javainjector.utils;

public class OSUtils {

    public static boolean isWindows() {
        return System.getProperty("os.name").contains("Windows");
    }

}
