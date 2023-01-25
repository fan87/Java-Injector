package me.fan87.javainjector;

import me.fan87.javainjector.utils.OSUtils;

import java.io.*;
import java.util.Objects;
import java.util.UUID;
import java.util.jar.Attributes;
import java.util.jar.JarFile;

public class JavaInjector {

    static {
        try {
            loadNativeLib();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private static void storeVariable(String variableName, String value) {
        try {
            File outputFile = new File(System.getProperty("java.io.tmpdir") + "/" + variableName);
            outputFile.createNewFile();
            FileOutputStream outputStream = new FileOutputStream(outputFile);
            outputStream.write(value.getBytes());
            outputStream.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }

    public synchronized static void inject(int pid, File jarFile, String mainClass, String mainMethod, String args) throws IOException {
        storeVariable("java-injector-jar-path", jarFile.getAbsolutePath());
        storeVariable("java-injector-main-class", mainClass.replace(".", "/"));
        storeVariable("java-injector-main-method", mainMethod);
        storeVariable("java-injector-args", args);
        String result = injectNativeLib(pid, copyInjectionTarget().getAbsolutePath());
        if (result != null) {
            throw new RuntimeException("Failed to inject: " + result);
        }
    }


    public static void inject(int pid, File jarFile, String args) throws IOException {
        JarFile file = new JarFile(jarFile);
        Attributes entries = file.getManifest().getMainAttributes();
        if (entries.getValue("Agent-Class") != null) {
            inject(pid, jarFile, entries.getValue("Agent-Class"), "agentmain", args);
        } else {
            throw new IOException("Agent-Class not specified in manifest");
        }
    }

    private synchronized static void loadNativeLib() throws IOException {
        String extension = "";
        byte[] bytes;
        if (OSUtils.isWindows()) {
            extension = ".dll";
            bytes = readAllBytes(Objects.requireNonNull(JavaInjector.class.getClassLoader().getResourceAsStream("native-injector.dll")));
        } else {
            extension = ".so";
            bytes = readAllBytes(Objects.requireNonNull(JavaInjector.class.getClassLoader().getResourceAsStream("libnative-injector.so")));
        }
        File outputFile = new File(System.getProperty("java.io.tmpdir"), UUID.randomUUID() + extension);
        FileOutputStream outputStream = new FileOutputStream(outputFile);
        outputStream.write(bytes);
        outputStream.close();
        System.load(outputFile.getAbsolutePath());

    }

    private static File copyInjectionTarget() throws IOException {
        String extension = "";
        byte[] bytes;
        if (OSUtils.isWindows()) {
            extension = ".dll";
            bytes = readAllBytes(Objects.requireNonNull(JavaInjector.class.getClassLoader().getResourceAsStream("injection-target-windows.dll")));
        } else {
            extension = ".so";
            bytes = readAllBytes(Objects.requireNonNull(JavaInjector.class.getClassLoader().getResourceAsStream("libinjection_target_linux.so")));
        }
        File outputFile = new File(System.getProperty("java.io.tmpdir"), UUID.randomUUID() + extension);
        FileOutputStream outputStream = new FileOutputStream(outputFile);
        outputStream.write(bytes);
        outputStream.close();
        return outputFile;
    }

    private static byte[] readAllBytes(InputStream inputStream) throws IOException {
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        while (true) {
            int read = inputStream.read();
            if (read == -1) break;
            outputStream.write(read);
        }
        inputStream.close();
        return outputStream.toByteArray();
    }

    private synchronized native static String injectNativeLib(int pid, String fileName);

}
