package me.fan87.javainjector;

import java.lang.reflect.Method;

public class JNIBinding {

    public static native Object getField(Class<?> clazz, Object classInstance, String fieldName);

    public static native Object getStaticField(Class<?> clazz, String fieldName);

    public static native void setField(Class<?> clazz, Object classInstance, String fieldName, Object value);

    public static native void setStaticField(Class<?> clazz, String fieldName, Object value);

    public static native Object invokeMethodS(Class<?> clazz, Object classInstance, String methodName, String signature, Object... args);

    public static native Object invokeStaticMethodS(Class<?> clazz, String methodName, String signature, Object... args);


}
