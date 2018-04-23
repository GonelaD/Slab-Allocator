CS3523 - Operating Systems 2
Programming Assignment 1:

---------------------------------------------------------------------------------------------------

INSTRUCTIONS:


    
1. Open the terminal and go to the directory where libmymem.hpp, libmymem.cpp and memutil.cpp files are stored.
        If stored on Desktop, type “cd Desktop/”
        
2. Compile the source file, libmymem.cpp using 
                               
                                “ g++ -c libmymem.cpp”

                    (this will create libmymem.o file)


3. Then generate the library by running following commands,


                    g++ -std=c++11  -Wall -Werror -fpic -c libmymem.o -I . libmymem.cpp

                    g++ -shared -o libmymem.so libmymem.o


4. Now generate the utility by running command,
                
                    g++  -std=c++11 -I . -L . -Wall -o memutil memutil.cpp -l mymem -pthread


5. Run the generated utility using following commands.
                
                    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:


                    ./memutil