#pragma once
#include <jni.h>
#include <jvmti.h>
#include <mutex>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

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
