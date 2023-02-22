plugins {
    id("cpp-library")
}

group = "com.github.fan87.Java-Injector"
version = "1.0-SNAPSHOT"



tasks {
}



tasks.withType(CppCompile::class.java).configureEach {

    val include = File(File(System.getProperty("java.home")).parentFile, "include")
    includes.from(include, *include.listFiles()!!)

}
tasks.withType(LinkSharedLibrary::class.java).configureEach {
    val bin = File(File(System.getProperty("java.home")).parentFile, "lib")
    val libraries = ArrayList<File>()
    for (listFile in bin.listFiles()) {
        if (listFile.name.endsWith(".lib")) {
            libraries.add(listFile)
        }
    }
    libs.from(*libraries.toTypedArray())
}

library {
    linkage.set(arrayListOf(Linkage.SHARED))
    source.from("src/main/cpp")
    publicHeaders.from("src/main/public")
    privateHeaders.from("src/main/headers")
}

