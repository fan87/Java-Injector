/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class me_fan87_javainjector_NativeInstrumentation */

#ifndef _Included_me_fan87_javainjector_NativeInstrumentation
#define _Included_me_fan87_javainjector_NativeInstrumentation
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    isModifiableClass0
 * Signature: (Ljava/lang/Class;)Z
 */
JNIEXPORT jboolean JNICALL Java_me_fan87_javainjector_NativeInstrumentation_isModifiableClass0
  (JNIEnv *, jclass, jclass);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    setHasRetransformableTransformers
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_setHasRetransformableTransformers
  (JNIEnv *, jclass, jboolean);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    retransformClasses0
 * Signature: ([Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_retransformClasses0
  (JNIEnv *, jclass, jobjectArray);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    redefineClasses0
 * Signature: ([Ljava/lang/instrument/ClassDefinition;)V
 */
JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_redefineClasses0
  (JNIEnv *, jclass, jobjectArray);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    getAllLoadedClasses0
 * Signature: ()[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL Java_me_fan87_javainjector_NativeInstrumentation_getAllLoadedClasses0
  (JNIEnv *, jclass);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    getInitiatedClasses0
 * Signature: (Ljava/lang/ClassLoader;)[Ljava/lang/Class;
 */
JNIEXPORT jobjectArray JNICALL Java_me_fan87_javainjector_NativeInstrumentation_getInitiatedClasses0
  (JNIEnv *, jclass, jobject);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    getObjectSize0
 * Signature: (Ljava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL Java_me_fan87_javainjector_NativeInstrumentation_getObjectSize0
  (JNIEnv *, jclass, jobject);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    appendToClassLoaderSearch0
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_appendToClassLoaderSearch0
  (JNIEnv *, jclass, jstring, jboolean);

/*
 * Class:     me_fan87_javainjector_NativeInstrumentation
 * Method:    setNativeMethodPrefixes
 * Signature: ([Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_setNativeMethodPrefixes
  (JNIEnv *, jclass, jobjectArray, jboolean);

JNIEXPORT void JNICALL Java_me_fan87_javainjector_NativeInstrumentation_init
        (JNIEnv *, jclass);
#ifdef __cplusplus
}
#endif
#endif