import me.fan87.javainjector.NativeInstrumentation;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.tree.AbstractInsnNode;
import org.objectweb.asm.tree.ClassNode;
import org.objectweb.asm.tree.LdcInsnNode;
import org.objectweb.asm.tree.MethodNode;

import java.lang.instrument.ClassDefinition;
import java.lang.instrument.ClassFileTransformer;
import java.lang.instrument.IllegalClassFormatException;
import java.lang.instrument.Instrumentation;
import java.security.ProtectionDomain;

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
