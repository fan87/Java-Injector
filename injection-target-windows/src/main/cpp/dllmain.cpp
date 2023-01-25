#include <fstream>
#include <string>
#include "dllmain.h"
#include "jni.h"
#include "jvmti.h"

void read_file_content(const char* fileName, char *buffer, int bufferSize) {
    std::ifstream myfile;
    myfile.open(fileName);
    std::string string;
    myfile >> buffer;
    myfile.close();
}

void get_variable(const char* name, char *buffer, int bufferSize) {
    const char* temp = getenv("TEMP");
    char fullPath[100];
    sprintf(fullPath, "%s/%s", temp, name);
    read_file_content(fullPath, buffer, bufferSize);
}

void error(const char* message) {
    fprintf(stderr, "%s", message);
    fflush(stdout);
}


DWORD WINAPI MainThread( LPVOID lpParam ) {

    JavaVM *vm;
    JNIEnv *jni;
    jvmtiEnv *jvmti0;

    jsize size;
    if (JNI_GetCreatedJavaVMs(&vm, 1, &size) != JNI_OK || size != 1) {
        error("Failed to GetCreatedJavaVMs");
        exit(-1);
    }
    jint result = vm->GetEnv((void**) &jni, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        result = vm->AttachCurrentThread((void**) &jni, nullptr);
    }
    if (result != JNI_OK) {
        error("Failed to get Jni");
        exit(-1);
    }
    result = vm->GetEnv((void**) &jvmti0, JVMTI_VERSION_1_0);
    if (result != JNI_OK) {
        error("Failed to get JvmTi");
        exit(-1);
    }

    char jarPath[300];
    char mainClassName[300];
    char mainMethodName[300];
    char args[300];
    get_variable("java-injector-jar-path", jarPath, 300);
    get_variable("java-injector-main-class", mainClassName, 300);
    get_variable("java-injector-main-method", mainMethodName, 300);
    get_variable("java-injector-args", args, 300);
    jvmti0->AddToSystemClassLoaderSearch(jarPath);
    jclass mainClass = jni->FindClass(mainClassName);
    if (mainClass == nullptr) {
        printf("Main class: %s\n", mainClassName);
        fflush(stdout);
        error("Could not find agent main class");
        exit(-1);
    }
    jmethodID main = jni->GetStaticMethodID(mainClass, mainMethodName, "(Ljava/lang/String;Ljava/lang/instrument/Instrumentation;)V");
    if (main == nullptr) {
        error("Could not find agent main method");
        exit(-1);
    }
    jstring argsString = jni->NewStringUTF(args);

    jclass instrumentationClass = jni->FindClass("me/fan87/javainjector/NativeInstrumentation");
    if (instrumentationClass == nullptr) {
        error("Could not find instrumentation class");
        exit(-1);
    }
    jmethodID instrumentationConstructor = jni->GetMethodID(instrumentationClass, "<init>", "()V");
    if (instrumentationConstructor == nullptr) {
        error("Could not find instrumentation constructor");
        exit(-1);
    }
    jobject instrumentation = jni->NewObject(instrumentationClass, instrumentationConstructor);

    jni->CallStaticVoidMethod(mainClass, main, argsString, instrumentation);
    return 0;

}


BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    HMODULE hMod;
    static int *exit_value_addr;

    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}