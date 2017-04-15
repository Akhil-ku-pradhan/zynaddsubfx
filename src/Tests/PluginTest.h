/*
  ZynAddSubFX - a software synthesizer

  PluginTest.h - CxxTest for embedding zyn
  Copyright (C) 2013-2013 Mark McCurry
  Authors: Mark McCurry

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#include <cxxtest/TestSuite.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "../Misc/MiddleWare.h"
#include "../Misc/Master.h"
#include "../Misc/PresetExtractor.h"
#include "../Misc/PresetExtractor.cpp"
#include "../Misc/Util.h"
#include "../globals.h"
#include "../UI/NSM.H"

using namespace std;
using namespace zyn;

SYNTH_T *synth;
NSM_Client *nsm = 0;

char *instance_name=(char*)"";
MiddleWare *middleware;

class PluginTest:public CxxTest::TestSuite
{
    public:
        Config config;
        void setUp() {
            synth = new SYNTH_T;
            synth->buffersize = 256;
            synth->samplerate = 48000;
            synth->alias();

            outL  = new float[1024];
            for(int i = 0; i < synth->buffersize; ++i)
                outL[i] = 0.0f;
            outR = new float[1024];
            for(int i = 0; i < synth->buffersize; ++i)
                outR[i] = 0.0f;

            for(int i = 0; i < 16; ++i)
                master[i] = new Master(*synth, &config);
        }

        void tearDown() {
            for(int i = 0; i < 16; ++i)
                delete master[i];

            delete[] outL;
            delete[] outR;
            delete synth;
        }


        void testInit() {

            for(int x=0; x<100; ++x)
                for(int i=0; i<16; ++i)
                    master[i]->GetAudioOutSamples(rand()%1025,
                            synth->samplerate, outL, outR);
        }

        void testPanic()
        {
            master[0]->setController(0, 0x64, 0);
            master[0]->noteOn(0,64,64);
            master[0]->AudioOut(outL, outR);

            float sum = 0.0f;
            for(int i = 0; i < synth->buffersize; ++i)
                sum += fabs(outL[i]);

            TS_ASSERT_LESS_THAN(0.1f, sum);
        }

        string loadfile(string fname) const
        {
            std::ifstream t(fname.c_str());
            std::string str((std::istreambuf_iterator<char>(t)),
                                     std::istreambuf_iterator<char>());
            return str;
        }


        void testLoadSave(void)
        {
            const string fname = string(SOURCE_DIR) + "/guitar-adnote.xmz";
            const string fdata = loadfile(fname);
            char *result = NULL;
            master[0]->putalldata((char*)fdata.c_str());
            int res = master[0]->getalldata(&result);

            TS_ASSERT_EQUALS((int)(fdata.length()+1), res);
            TS_ASSERT(fdata == result);
        }


    private:
        float *outR, *outL;
        Master *master[16];
};
