#pragma once
#include <../ext/JNI/jni.h>
#include <../ext/JNI/jvmti.h>
#include <mutex>
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

class Lunar {
public:
    JNIEnv* env;
    JavaVM* vm;

    void GetLoadedClasses();
    jclass GetClass(const std::string& className);

private:
    std::unordered_map<std::string, jclass> classes;
    std::mutex mtx;
};

extern std::unique_ptr<Lunar> lc;

