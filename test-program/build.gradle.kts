plugins {
    id("java")
    id("com.github.johnrengelman.shadow") version "7.1.2"
}

group = "com.github.fan87.Java-Injector"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    implementation(rootProject)
    implementation(project(":native-agent"))
    implementation("org.ow2.asm:asm:9.3")
    implementation("org.ow2.asm:asm-util:9.3")
    implementation("org.ow2.asm:asm-commons:9.3")
}


tasks.shadowJar {
    manifest {
        attributes["Agent-Class"] = "AgentMain"
        attributes["Main-Class"] = "InjectorMain"
    }
}