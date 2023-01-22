//
// Created by fan87 on 1/23/23.
//

#ifndef NATIVE_AGENT_LINUX_TYPE_CONVERT_H
#define NATIVE_AGENT_LINUX_TYPE_CONVERT_H

//
// Created by fan87 on 1/23/23.
//

#include "type_convert.h"
#include "jni.h"
#include <cstring>
#include <cstdlib>
#include <string>


char* parseSignature(char *signature);

jvalue* objectsToJvalues(JNIEnv* env, char* signature, jobject* args, jsize length);

char getReturnType(char* signature);

void init(JNIEnv *env);

jobject getField(JNIEnv *env, jfieldID id, jobject classInstance, char *signature);

jobject getStaticField(JNIEnv *env, jfieldID id, jclass clazz, char *signature);

void setField(JNIEnv *env, jfieldID id, jobject classInstance, jobject value, char* signature);

void setStaticField(JNIEnv *env, jfieldID id, jclass clazz, jobject value, char* signature);

jobject callMethod(JNIEnv *env, jmethodID id, jobject classInstance, jobject* objects, jsize length, char* signature);

jobject callStaticMethod(JNIEnv *env, jmethodID id, jclass clazz, jobject* objects, jsize length, char* signature);


#endif //NATIVE_AGENT_LINUX_TYPE_CONVERT_H
