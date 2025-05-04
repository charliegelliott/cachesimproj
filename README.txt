
My Environment:

    OS: Ubuntu 22.04.5 (WSL)
    C++ Compiler: g++ (GCC) version 11.4.0

Compilation

    1. Open your terminal application IN the folder named "cachesim"
    2. Compile using g++ using the following syntax:
        g++ -std=c++17 cachesim.cpp -02 -o cachesim
        (-std=c++17 enables C++17 stnrd features)
        (-02 applies aptimization lvl 2)

Execution

The simulator reads a trace file names "test.trace" and write output to "test.result"

    1. To run my custom "block.trace" file, run these commands:
        cp block.trace test.trace
        ./cachesim
        
        (for clarity you can also copy these results to "block.result" using 'mv test.result block.result' for comparison)

    2. To run my custom "associative.trace" file, run these commands:
        cp associative.trace test.trace
        ./cachesim

        (for clarity you can also copy these results again to "assoc.result" by executing 'mv test.result assoc.result' for comparison)

    OPTIONAL

    If you would like to compare the 2 generated files, you can follow these steps:
        1. After step 1 in Execution, run:
            'mv test.result block.result'

        2. After the above step and step 2 in Execution, run:
            'mv test.result assoc.result'

        3. Now you can compare the two using the command:
            'diff block.result assoc.result'

            
