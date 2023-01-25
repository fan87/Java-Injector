rootProject.name = "java-injector"





fun isWindows(): Boolean = "Windows" in System.getProperty("os.name")

include("test-program")
include("test-inject-target")
include("common")
include("native-agent")

if (System.getProperty("me.fan87.buildnative") == "true") {
    include("native-agent-native")
    include("native-injector")

    if (isWindows()) {
        include("injection-target-windows")
    } else {
        include("injection-target-linux")
    }
}