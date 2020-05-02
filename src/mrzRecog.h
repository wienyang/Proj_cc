#pragma once
#include<windows.h>
#include<vector>
#include <fstream>
#include<iostream>
#include"findMRZ.h"
#include"tools.h"
#include"mrz.h"
#include"cardRecog.h"


void* initMrzNet(const char* modelPath);
int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack);