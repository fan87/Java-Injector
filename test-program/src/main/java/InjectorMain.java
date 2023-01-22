import me.fan87.javainjector.JavaInjector;

import java.io.File;
import java.io.IOException;

public class InjectorMain {

    public static void main(String[] args) throws IOException {
        if (args.length != 2) {
            System.err.println("Invalid usage! Usage: \"InjectorMain pid agentpath\"");
            return;
        }
        int pid = Integer.parseInt(args[1]);
        String agentPath = args[0];

        JavaInjector.inject(pid, new File(agentPath), "Test Args");
    }

}
