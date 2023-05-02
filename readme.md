# 💉 Java Injector
Inject to JNI Enabled JVMs on Linux & Windows, even with `DisableAttachMechanism` enabled.

The repository is not product ready, as there may be bugs, but I'm already using it for products as I can fix bugs really quick.

## Supported Platforms
It currently supports x86 64bits Linux & Windows JVM, you may build the `native-agent-native` & `injection-target-<platform>`
yourself to make it work on 32bit machines, or potentially ARM machines.

It also supports Java 8 and above, although the compiler may not be happy about the use of `ClassTransformer` in some projects
on Java version 9 or above.

## Using as Library
To use it as library, please get it from [Jitpack](https://jitpack.io/#fan87/Java-Injector/v1.0)

You may have to initialize the `NativeInstrumentation` in your Java-agent:
```java
public class AgentMain {

    public static void test() {
        System.out.println("Test Method Called");
    }

    public static void main(String[] args) throws Exception {
        agentmain("Test Args", new NativeInstrumentation());
    }

    public static void agentmain(String agentArgs, Instrumentation instrumentation) throws Exception {
        assert agentArgs.equals("Test Args");
        System.out.println("Hello, World! " + agentArgs);
        NativeInstrumentation.init();
        NativeInstrumentation.invokeStaticMethodS(AgentMain.class, "test", "()V");
        final int[] times = {0};
        final byte[][] data = new byte[1][1];
        instrumentation.addTransformer(new ClassFileTransformer() {
            @Override
            public byte[] transform(ClassLoader classLoader, String s, Class<?> aClass, ProtectionDomain protectionDomain, byte[] bytes) throws IllegalClassFormatException {
                if (s.equals("TestClass")) {
                    times[0]++;
                    if (times[0] > 2) {
                        return null;
                    }
                    ClassNode node = new ClassNode();
                    ClassReader reader = new ClassReader(bytes);
                    reader.accept(node, 0);

                    for (MethodNode method : node.methods) {
                        for (AbstractInsnNode instruction : method.instructions) {
                            if (instruction instanceof LdcInsnNode) {
                                if (aClass == null) {
                                    ((LdcInsnNode) instruction).cst = "Replaced Text!";
                                } else {
                                    ((LdcInsnNode) instruction).cst = "Replaced Text! 2";
                                }
                            }
                        }
                    }

                    ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
                    node.accept(writer);
                    data[0] = writer.toByteArray();
                    return writer.toByteArray();
                }
                return null;
            }
        }, true);

        TestClass.test();
        instrumentation.retransformClasses(TestClass.class);

        TestClass.test();
        ClassNode node = new ClassNode();
        ClassReader reader = new ClassReader(data[0]);
        reader.accept(node, 0);

        for (MethodNode method : node.methods) {
            for (AbstractInsnNode instruction : method.instructions) {
                if (instruction instanceof LdcInsnNode) {
                    ((LdcInsnNode) instruction).cst = "Replaced Text! 3";
                }
            }
        }

        ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
        node.accept(writer);

        instrumentation.redefineClasses(new ClassDefinition(TestClass.class, writer.toByteArray()));
        TestClass.test();

    }

}
```



## Potentials
### Hook into Java methods in runtime
It's possible (And I'm also doing it) to transform Java classes in JVM runtime without injecting/attaching to itself using
`NativeInstrumentation`.

```kt
import me.fan87.javainjector.NativeInstrumentation
import org.objectweb.asm.ClassReader
import org.objectweb.asm.ClassWriter
import org.objectweb.asm.Opcodes
import org.objectweb.asm.Type
import org.objectweb.asm.tree.ClassNode
import org.objectweb.asm.tree.analysis.Analyzer
import org.objectweb.asm.tree.analysis.AnalyzerException
import org.objectweb.asm.tree.analysis.SimpleVerifier
import org.objectweb.asm.util.CheckClassAdapter
import java.io.PrintWriter
import java.lang.instrument.ClassDefinition
import java.lang.instrument.ClassFileTransformer

object TransformManager {

    val classesCache = HashMap<Class<*>, ClassNode>()
    val instrumentation = NativeInstrumentation()

    init {
        try {
            NativeInstrumentation.loadNativeLib()
        } catch (e: Throwable) {
            e.printStackTrace()
        }
        try {
            NativeInstrumentation.init()
        } catch (e: Throwable) {
            e.printStackTrace()
        }
    }

    fun transformClass(classTransformer: ClassTransformer) {
        if (classTransformer.targetClass in classesCache) {
            val classNode = classesCache[classTransformer.targetClass]!!
            classTransformer.transform(classNode)
            var result: ByteArray
            try {
                val classWriter = ClassWriter(ClassWriter.COMPUTE_FRAMES)
                classNode.accept(classWriter)
                result = classWriter.toByteArray()
            } catch (e: Exception) {
                e.printStackTrace()
                val classWriter = ClassWriter(ClassWriter.COMPUTE_MAXS)
                classNode.accept(classWriter)
                result = classWriter.toByteArray()
            }

            CheckClassAdapter.verify(ClassReader(result), TransformManager::class.java.classLoader, false, PrintWriter(System.err, true));

            instrumentation.redefineClasses(ClassDefinition(classTransformer.targetClass, result))
        } else {
            val transformer = ClassFileTransformer { loader, className, classBeingRedefined, protectionDomain, classfileBuffer ->
                try {
                    if (classBeingRedefined == classTransformer.targetClass) {
                        val classNode = ClassNode()
                        val classReader = ClassReader(classfileBuffer)
                        classReader.accept(classNode, ClassReader.EXPAND_FRAMES)
                        classTransformer.transform(classNode)
                        var result: ByteArray
                        try {
                            val classWriter = ClassWriter(ClassWriter.COMPUTE_FRAMES)
                            classNode.accept(classWriter)
                            result = classWriter.toByteArray()
                        } catch (e: Exception) {
                            e.printStackTrace()
                            val classWriter = ClassWriter(ClassWriter.COMPUTE_MAXS)
                            classNode.accept(classWriter)
                            result = classWriter.toByteArray()
                        }
                        CheckClassAdapter.verify(ClassReader(result), TransformManager::class.java.classLoader, false, PrintWriter(System.err, true));
                        classesCache[classBeingRedefined] = classNode
                        result
                    } else {
                        classfileBuffer
                    }
                } catch (e: Throwable) {
                    e.printStackTrace()
                    classfileBuffer
                }
            }
            instrumentation.addTransformer(transformer, true)
            instrumentation.retransformClasses(classTransformer.targetClass)
            instrumentation.removeTransformer(transformer)
        }
    }

    fun verify(classNode: ClassNode, loader: ClassLoader, printWriter: PrintWriter) {

        var syperType = if (classNode.superName == null) null else Type.getObjectType(classNode.superName)
        var methods = classNode.methods

        var interfaces: MutableList<Type> = ArrayList()
        for ( interfaceName:kotlin.String? in classNode.interfaces)
        {
            interfaces.add(Type.getObjectType(interfaceName))
        }

        for ( method:org.objectweb.asm.tree.MethodNode? in methods)
        {
            val verifier = SimpleVerifier(
                Type.getObjectType(classNode.name),
                syperType,
                interfaces,
                classNode.access and Opcodes.ACC_INTERFACE != 0
            )
            val analyzer = Analyzer(verifier)
            if (loader != null) {
                verifier.setClassLoader(loader)
            }
            try {
                analyzer.analyze(classNode.name, method)
            } catch (e: AnalyzerException) {
                e.printStackTrace(printWriter)
            }
        }
        printWriter.flush()
    }

}
```
