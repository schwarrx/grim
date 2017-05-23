/*******************************************************
 * Author - S Nelaturi
 * Testing arrayfire based cross correlation
 ********************************************************/

#include <stdio.h>
#include <arrayfire.h>
#include <cstdio>
#include <cstdlib>
#include <assert.h> 

#include "dfm.hpp"
#include "options.hpp"


using namespace std;

//int fftmain();
//int mcmain(int argc, char *argv[]);
void runGraphicsTest(int argc, char **argv);

int main(int argc, char *argv[]) {

    try {
        //fftmain();
        runGraphicsTest(argc, argv);

        std::string binvoxFile;
        int device;

        bool argsOk = processCommandLine(argc, argv, binvoxFile, device);
        if (!argsOk) {
            return (1);
        }

        dfmAnalysis(binvoxFile, device);

        // visualize the result
        //visualize(result, 1, 0.1);

    } catch (af::exception& e) {
        fprintf(stderr, "%s\n", e.what());
        throw;
    }

#ifdef WIN32 // pause in Windows
    if (!(argc == 2 && argv[1][0] == '-')) {
        printf("hit [enter]...");
        fflush(stdout);
        getchar();
    }
#endif
    return (0);
}
