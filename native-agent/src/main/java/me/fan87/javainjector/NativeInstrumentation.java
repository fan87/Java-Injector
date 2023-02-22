package me.fan87.javainjector;


import me.fan87.javainjector.utils.OSUtils;

import java.io.*;
import java.lang.reflect.Method;
import java.lang.reflect.AccessibleObject;
import java.lang.instrument.ClassFileTransformer;
import java.lang.instrument.ClassDefinition;
import java.lang.instrument.Instrumentation;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.ProtectionDomain;
import java.util.*;
import java.util.jar.JarFile;

public class NativeInstrumentation implements Instrumentation {
    private final TransformerManager mTransformerManager;
    private TransformerManager mRetransfomableTransformerManager;
    private final boolean mEnvironmentSupportsRedefineClasses;
    private final boolean mEnvironmentSupportsNativeMethodPrefix;

    public NativeInstrumentation() {
        mTransformerManager = new TransformerManager(false);
        managers.add(mTransformerManager);
        mRetransfomableTransformerManager = null;
        mEnvironmentSupportsRedefineClasses = true;
        mEnvironmentSupportsNativeMethodPrefix = true;
    }

    public void
    addTransformer(ClassFileTransformer transformer) {
        addTransformer(transformer, false);
    }

    public synchronized void
    addTransformer(ClassFileTransformer transformer, boolean canRetransform) {
        if (transformer == null) {
            throw new NullPointerException("null passed as 'transformer' in addTransformer");
        }
        if (canRetransform) {
            if (!isRetransformClassesSupported()) {
                throw new UnsupportedOperationException(
                        "adding retransformable transformers is not supported in this environment");
            }
            if (mRetransfomableTransformerManager == null) {
                mRetransfomableTransformerManager = new TransformerManager(true);
                retransformManagers.add(mRetransfomableTransformerManager);
            }
            mRetransfomableTransformerManager.addTransformer(transformer);
            if (mRetransfomableTransformerManager.getTransformerCount() == 1) {
                setHasRetransformableTransformers(true);
            }
        } else {
            mTransformerManager.addTransformer(transformer);
        }
    }

    public synchronized boolean
    removeTransformer(ClassFileTransformer transformer) {
        if (transformer == null) {
            throw new NullPointerException("null passed as 'transformer' in removeTransformer");
        }
        TransformerManager mgr = findTransformerManager(transformer);
        if (mgr != null) {
            mgr.removeTransformer(transformer);
            if (mgr.isRetransformable() && mgr.getTransformerCount() == 0) {
                setHasRetransformableTransformers(false);
            }
            return true;
        }
        return false;
    }

    public boolean
    isModifiableClass(Class<?> theClass) {
        if (theClass == null) {
            throw new NullPointerException(
                    "null passed as 'theClass' in isModifiableClass");
        }
        return isModifiableClass0(theClass);
    }

    public boolean
    isRetransformClassesSupported() {
        return true;
    }

    public void
    retransformClasses(Class<?>... classes) {
        if (!isRetransformClassesSupported()) {
            throw new UnsupportedOperationException(
                    "retransformClasses is not supported in this environment");
        }
        retransformClasses0(classes);
    }

    public boolean
    isRedefineClassesSupported() {
        return mEnvironmentSupportsRedefineClasses;
    }

    public void
    redefineClasses(ClassDefinition... definitions)
            throws ClassNotFoundException {
        if (!isRedefineClassesSupported()) {
            throw new UnsupportedOperationException("redefineClasses is not supported in this environment");
        }
        if (definitions == null) {
            throw new NullPointerException("null passed as 'definitions' in redefineClasses");
        }
        for (int i = 0; i < definitions.length; ++i) {
            if (definitions[i] == null) {
                throw new NullPointerException("element of 'definitions' is null in redefineClasses");
            }
        }
        if (definitions.length == 0) {
            return; // short-circuit if there are no changes requested
        }

        redefineClasses0(definitions);
    }

    @SuppressWarnings("rawtypes")
    public Class[]
    getAllLoadedClasses() {
        return getAllLoadedClasses0();
    }

    @SuppressWarnings("rawtypes")
    public Class[]
    getInitiatedClasses(ClassLoader loader) {
        return getInitiatedClasses0(loader);
    }

    public long
    getObjectSize(Object objectToSize) {
        if (objectToSize == null) {
            throw new NullPointerException("null passed as 'objectToSize' in getObjectSize");
        }
        return getObjectSize0(objectToSize);
    }

    public void
    appendToBootstrapClassLoaderSearch(JarFile jarfile) {
        appendToClassLoaderSearch0(jarfile.getName(), true);
    }

    public void
    appendToSystemClassLoaderSearch(JarFile jarfile) {
        appendToClassLoaderSearch0(jarfile.getName(), false);
    }

    public boolean
    isNativeMethodPrefixSupported() {
        return mEnvironmentSupportsNativeMethodPrefix;
    }

    public synchronized void
    setNativeMethodPrefix(ClassFileTransformer transformer, String prefix) {
        if (!isNativeMethodPrefixSupported()) {
            throw new UnsupportedOperationException(
                    "setNativeMethodPrefix is not supported in this environment");
        }
        if (transformer == null) {
            throw new NullPointerException(
                    "null passed as 'transformer' in setNativeMethodPrefix");
        }
        TransformerManager mgr = findTransformerManager(transformer);
        if (mgr == null) {
            throw new IllegalArgumentException(
                    "transformer not registered in setNativeMethodPrefix");
        }
        mgr.setNativeMethodPrefix(transformer, prefix);
        String[] prefixes = mgr.getNativeMethodPrefixes();
        setNativeMethodPrefixes(prefixes, mgr.isRetransformable());
    }

    private TransformerManager
    findTransformerManager(ClassFileTransformer transformer) {
        if (mTransformerManager.includesTransformer(transformer)) {
            return mTransformerManager;
        }
        if (mRetransfomableTransformerManager != null &&
                mRetransfomableTransformerManager.includesTransformer(transformer)) {
            return mRetransfomableTransformerManager;
        }
        return null;
    }


    /*
     *  Natives
     */

    public static native void init();
    private static native boolean
    isModifiableClass0(Class<?> theClass);

    private static native void
    setHasRetransformableTransformers(boolean has);

    private static native void
    retransformClasses0(Class<?>[] classes);

    private static native void
    redefineClasses0(ClassDefinition[] definitions)
            throws ClassNotFoundException;

    @SuppressWarnings("rawtypes")
    private static native Class[]
    getAllLoadedClasses0();

    @SuppressWarnings("rawtypes")
    private static native Class[]
    getInitiatedClasses0(ClassLoader loader);

    private static native long
    getObjectSize0(Object objectToSize);

    private static native void
    appendToClassLoaderSearch0(String jarfile, boolean bootLoader);

    private static native void
    setNativeMethodPrefixes(String[] prefixes, boolean isRetransformable);

    public static native Object getField(Class<?> clazz, Object classInstance, String fieldName);
    public static native Object getStaticField(Class<?> clazz, String fieldName);
    public static native void setField(Class<?> clazz, Object classInstance, String fieldName, Object value);
    public static native void setStaticField(Class<?> clazz, String fieldName, Object value);
    public static native Object invokeMethodS(Class<?> clazz, Object classInstance, String methodName, String signature, Object... args);
    public static native Object invokeStaticMethodS(Class<?> clazz, String methodName, String signature, Object... args);

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


    private synchronized static void loadNativeLib() throws IOException {
        String extension = "";
        byte[] bytes;
        if (OSUtils.isWindows()) {
            extension = ".dll";
            bytes = readAllBytes(Objects.requireNonNull(NativeInstrumentation.class.getClassLoader().getResourceAsStream("native-agent-native.dll")));
        } else {
            extension = ".so";
            bytes = readAllBytes(Objects.requireNonNull(NativeInstrumentation.class.getClassLoader().getResourceAsStream("libnative-agent-native.so")));
        }
        File outputFile = new File(System.getProperty("java.io.tmpdir"), UUID.randomUUID() + extension);
        FileOutputStream outputStream = new FileOutputStream(outputFile);
        outputStream.write(bytes);
        outputStream.close();
        System.load(outputFile.getAbsolutePath());

    }

    static {
        try {
            loadNativeLib();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /*
     *  Internals
     */


    // Enable or disable Java programming language access checks on a
    // reflected object (for example, a method)
    private static void setAccessible(final AccessibleObject ao, final boolean accessible) {
        AccessController.doPrivileged(new PrivilegedAction<Object>() {
            public Object run() {
                ao.setAccessible(accessible);
                return null;
            }
        });
    }

    // Attempt to load and start an agent
    private void
    loadClassAndStartAgent(String classname,
                           String methodname,
                           String optionsString)
            throws Throwable {

        ClassLoader mainAppLoader = ClassLoader.getSystemClassLoader();
        Class<?> javaAgentClass = mainAppLoader.loadClass(classname);

        Method m = null;
        NoSuchMethodException firstExc = null;
        boolean twoArgAgent = false;

        // The agent class must have a premain or agentmain method that
        // has 1 or 2 arguments. We check in the following order:
        //
        // 1) declared with a signature of (String, Instrumentation)
        // 2) declared with a signature of (String)
        // 3) inherited with a signature of (String, Instrumentation)
        // 4) inherited with a signature of (String)
        //
        // So the declared version of either 1-arg or 2-arg always takes
        // primary precedence over an inherited version. After that, the
        // 2-arg version takes precedence over the 1-arg version.
        //
        // If no method is found then we throw the NoSuchMethodException
        // from the first attempt so that the exception text indicates
        // the lookup failed for the 2-arg method (same as JDK5.0).

        try {
            m = javaAgentClass.getDeclaredMethod(methodname,
                    String.class,
                    Instrumentation.class);
            twoArgAgent = true;
        } catch (NoSuchMethodException x) {
            // remember the NoSuchMethodException
            firstExc = x;
        }

        if (m == null) {
            // now try the declared 1-arg method
            try {
                m = javaAgentClass.getDeclaredMethod(methodname,
                        String.class);
            } catch (NoSuchMethodException x) {
                // ignore this exception because we'll try
                // two arg inheritance next
            }
        }

        if (m == null) {
            // now try the inherited 2-arg method
            try {
                m = javaAgentClass.getMethod(methodname,
                        String.class,
                        Instrumentation.class);
                twoArgAgent = true;
            } catch (NoSuchMethodException x) {
                // ignore this exception because we'll try
                // one arg inheritance next
            }
        }

        if (m == null) {
            // finally try the inherited 1-arg method
            try {
                m = javaAgentClass.getMethod(methodname,
                        String.class);
            } catch (NoSuchMethodException x) {
                // none of the methods exists so we throw the
                // first NoSuchMethodException as per 5.0
                throw firstExc;
            }
        }

        // the premain method should not be required to be public,
        // make it accessible so we can call it
        // Note: The spec says the following:
        //     The agent class must implement a public static premain method...
        setAccessible(m, true);

        // invoke the 1 or 2-arg method
        if (twoArgAgent) {
            m.invoke(null, optionsString, this);
        } else {
            m.invoke(null, optionsString);
        }

        // don't let others access a non-public premain method
        setAccessible(m, false);
    }

    // WARNING: the static native code knows the name & signature of this method
    private void
    loadClassAndCallPremain(String classname,
                            String optionsString)
            throws Throwable {

        loadClassAndStartAgent(classname, "premain", optionsString);
    }


    // WARNING: the static native code knows the name & signature of this method
    private void
    loadClassAndCallAgentmain(String classname,
                              String optionsString)
            throws Throwable {

        loadClassAndStartAgent(classname, "agentmain", optionsString);
    }

    private static List<TransformerManager> managers = new ArrayList<>();
    private static List<TransformerManager> retransformManagers = new ArrayList<>();


    public static byte[] tmpTransformOutput = null;

    // WARNING: the static native code knows the name & signature of this method
    private static synchronized void
    transform(ClassLoader loader,
              String classname,
              Class<?> classBeingRedefined,
              ProtectionDomain protectionDomain,
              byte[] classfileBuffer) {
        try {
            byte[] bufferToUse = classfileBuffer;
            boolean modified = false;
            for (TransformerManager manager : retransformManagers) {
                byte[] out = manager.transform(loader,
                        classname,
                        classBeingRedefined,
                        protectionDomain,
                        bufferToUse);
                if (out != null) {
                    bufferToUse = out;
                    modified = true;
                }
            }
            if (classBeingRedefined == null) {
                for (TransformerManager manager : managers) {
                    byte[] out = manager.transform(loader,
                            classname,
                            classBeingRedefined,
                            protectionDomain,
                            bufferToUse);
                    if (out != null) {
                        bufferToUse = out;
                        modified = true;
                    }
                }
            }

            if (modified) {
                tmpTransformOutput = bufferToUse;
            } else {
                tmpTransformOutput = null;
            }
        } catch (Throwable e) {
            e.printStackTrace();
            tmpTransformOutput = null;
        }
    }
}
