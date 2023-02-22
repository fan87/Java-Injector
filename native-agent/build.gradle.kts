plugins {
    id("maven-publish")
    id("java")
}

group = "com.github.fan87.Java-Injector"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    implementation(rootProject.project(":common"))
}

publishing {
    publications {
        register<MavenPublication>("mavenJava") {
            from(components.getByName("java"))
        }
    }
}