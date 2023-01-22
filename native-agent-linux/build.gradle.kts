plugins {
}

group = "me.fan87"
version = "1.0-SNAPSHOT"


fun prepareCmake() {
    exec {
        commandLine("cmake", "-DCMAKE_BUILD_TYPE=Debug", "-G", "Unix Makefiles", "-B", buildDir.absolutePath)
    }
}

if (!buildDir.exists()) {
    prepareCmake()
}

tasks {
    register("build") {
        group = "build"
        doFirst {
            exec {
                environment("JDK_DIR", File(System.getProperty("java.home")).parent.replace("\\", "/"))
                commandLine("cmake", "--build", buildDir.absolutePath, "--target", "native_agent_linux", "-j", "6")
            }
            copy {
                from("build/libnative_agent_linux.so")
                into("build/libs")
            }
        }
    }
    register("clean") {
        group = "build"
        doFirst {
            buildDir.deleteRecursively()
            prepareCmake()
        }
    }
}
