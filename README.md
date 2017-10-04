# spy
A spy framework.

## Getting Started
This is a spy framework that allows to inject your dlls into a host process and also execute the code in your application.

## Overview
[![Spy Framework Overview](https://github.com/VincentPT/spy/blob/master/doc/images/SpyFramework.PNG)](https://github.com/VincentPT/spy)

For the main purpose of visualize debuging in a project in development stage, the spy engine was created.
Besides that, we can also use the Framework for bug fixing while the host process is running, spying or even hacking.

### Prerequisites
* Windows (only support for Window)
* Visual studio
* CMake 3.2 or above

### Install development environment
Nothing special. CMake for generating project files and Visual studio for coding.
    
## Build.
There are two created script files to generate project for Visual studio 2015 placed in folder ./src/buid
In case you use another version of Visual Studio, just edit the file as you want or create new files.

The script configure.bat is generate project for x64 platform and configure-x86.bat for x86 platform.
After run the script, the project files will be generated in the same foler.
Open the solution file and build the solution(if compile error, build again), then build INSTALL project.

After build project in Visual Studio, the binary files will be copied to sub directory inside ./src/buid/bin folder.
    
## Run
In a sub directory inside ./src/buid/bin.
Run albino-mouse.exe first. This is just a sample host application, the spy dlls will be injected to this process.

Second, choose a sample spy client application bellow and run it in command line with following syntax.
    `<spy client application.exe> albino-mouse.exe`

* dynamic-functions.exe
* predefined-functions-1.exe
* predefined-functions-2.exe
* predefined-functions-3.exe

you can choose another host process to spy the check out the result will be printed on the console.

Example of running predefined-functions-3.exe with albino-mouse.exe(for release binaries)
    
albino-mouse.exe's console:
```
>albino-mouse.exe
generated array: {10,8,6,3,-2,10,4,10,}
press any key to exit
Invalid function address
Command 1 loaded!
Command 2 loaded!
deallocated buffer:00672F98
deallocated buffer:00671E58
deallocated buffer:00675B78
current process path: E:\Projects\spy\src\buid\bin\release\albino-mouse.exe
deallocated buffer:00671E58
find min elemen in range[0, 6] from array {10,8,6,3,-2,10,4,10,}
```

predefined-functions-3.exe's console:
```
> predefined-functions-3.exe albino-mouse.exe
monitor process albino-mouse.exe started!
getModulePath return: modulePath = E:\Projects\spy\src\buid\bin\release\spylib3.dll
getModulePath return: hModule = 0x50F00000
getModuleData return:
    1 <--> 50f01490
    2 <--> 50f01330
number of loaded function 2
custom commands begin at 0
injected process path: E:\Projects\spy\src\buid\bin\release\albino-mouse.exe
Now, try to find a min element in a range from an array in the remote process
input a range [min, max]
min:0
max:6
min element in range [0, 6] is 3
uninjected module from remote process spylib3.dll
stop monitor process albino-mouse.exe successfully
```
## Usage
![Spy Framework](https://github.com/VincentPT/spy/blob/master/doc/images/SpyFramework2.PNG)
    
The best way to learn how to work with the spy framework in your code is look at samples. That is more detail and
clear than explain by words.
This section will talk about general ideas to use the framework.

Look at above picture. You can see, the spy framework is included on both side, client side(your application)
and host side(a host application which your code will be injected to).

The framework can be split into two parts:
    * The engine are spy client class and spy-engine.dll.
    * The user specific part: User Spy client class and User Spy Lib.dll.

The spy client and Spy engine are designed to respond all common spy tasks while User Spy Client and
User Spy lib are designed to implement specific tasks for your application.

Each part have their own reponsibility base on its side.
Client side works with your application and take reponsibility to forward client request to spy-engine run in host process.
Host side works with host process, it can access host memory and code. It base on the input and also combine with
host's context to process the data and return to spy client.

Overall, there are some main steps to write your spy application using the framework.

The main steps of programming spy application using spy framework is.
1. Write your own spy client class inherit from SpyClient class with your specific API.
2. Write your own spy lib dll project which will executes your specific on host process and return data to your spy client APIs.
3. Write the code to inject the spy-engine.dll and loads your APIs in spy lib dll into host process.
4. Write the code the call the spy client API and process the return data.

### Write Your Spy Client class.
There are two main parts for a user spy client class is:
1. Injection API.
2. Application specific APIs.

* Injection API is a placed you can write the code to inject the spy-engine.dll, your spy lib.
Here, you should specify the full path of spy-engine.dll in case it does not present
at the same folder of host application or system default shared library foders.
And one important things is, if your spy lib or even your modified spy-engine.dll use 3rd party dlls,
they may also must to inject here as dependency dlls.

The engine will inject the dependency dlls first, then spy-engine.
Note that, If any dependency dll is missing in the list, the spy dlls cannot be injected.

Reference: ./src/samples/predefined-functions-3/UserSpyClient.cpp, API UserSpyClient::inject().

* Application specific APIs are APIs that use common API of spy client class with your specific data structure
and handle the output data.
Reference: ./src/samples/predefined-functions-3/UserSpyClient.cpp, API UserSpyClient::getInjectedProcessPath().

### Write your own spy lib dll.
There are two types of spy lib dll, predefined commands lib and dynamic commands lib.
You can choose one of them or both to implement your spy libs in the same application.

1. predefined commands lib.
this a shared library that must have two export APIs.
getPredefinedFunctionCount and loadPredefinedFunctions.

API getPredefinedFunctionCount must return number of your commands in the spy lib while loadPredefinedFunctions
map your command ids with your process function.

the spy-engine.dll will inject your spy lib into host process and call two APIs in your spy lib
to load the predefined commands to the engine.

Reference: ./src/spylib3/spylib.cpp

2. dynamic commands lib.
this is a shared library that have export APIs with any name that you want as long as they can be access by API
GetProcAddress.
Reference: ./user-spy-lib/spylib.cpp

The differences between this types are:
* predefined commands lib have a more complex implementation to setup the spy commands but it is easy to injected
the spy lib in client side while dynamic commands lib have a easier implementation but in client side, it is more
complext to inject the spy commands and manage them.

* predefined commands lib map spy functions to static command ids.
* dynamic commands lib not map any thing, it is just supplied the spy functions. The client side will have reponsibility
to map the functions(access by name) to dynamic command ids. And client side must manage these dynamic ids to access the
spy functions.

Go to next sections to learn how to execute injected functions in spy lib in client side.

### Inject the spy-engine and spy libs.
1. For predefined commands lib.
* inject spy-engine.dll and all dependencies by API SpyClient::inject.
* inject spy lib and load predefined comamnds by SpyClient::loadPredefinedFunctions
* store comamnd id base to access the injected command in future.
Reference: ./src/samples/predefined-functions-3/main.cpp

2. For dynamic commands lib.
* inject spy-engine.dll and all dependencies by API SpyClient::inject.
* prepare function name array.
* inject spy lib and load dynamic comamnds by SpyClient::loadDynamicFunctions
* store and manage dynamic comamnd ids return by loadDynamicFunctions to access the injected command in future.
Reference:  ./src/samples/dynamic-functions/main.cpp
            ./src/samples/dynamic-functions/UserSpyClient.cpp

### Execute injected functions.
Generaly, the injected functions can be executed via spy-engine by supplying corresponding dynamic command id with
the injected function and arguments supplied by the client side must same as spy lib.
there are two utility functions that can be used are.
executeCommandAndFreeCustomData and executeCommand.

* For predefined commands lib, the dynamic command can be computed by store the command id base returned by API
SpyClient::loadPredefinedFunctions by formular: <static command id> + <command id base>

Reference: ./src/samples/predefined-functions-3/UserSpyClient.cpp

* For dynamic commands lib, you should map the function name with the dynamic comamnd id return by API 
SpyClient::loadDynamicFunctions, the get the dynamic comamnd id by using function name via mapper.

Reference: ./src/samples/dynamic-functions/UserSpyClient.cpp

## Best practice.
For visualize debuging purpose. Check and see the [Buzz](https://github.com/VincentPT/buzz) application.

for more details of executing injected function by spy client check the section #1 in [Known Issues] bellow.

## Known Issues
1. Passing incompatible arguments with spy lib to the spy client.
    this is cause of wrong calling convention will lead to crash issues in the host application.

    In order to avoid that, follow bellow rules.

    **spy lib and spy client argument type mapping table**

    Spy lib | Spy client
    ------- | ----------
    l-value | l-value
    r-value | pointer
    poiner | pointer

    **support data type in x86 platform:**
    * all r-value and pointer data types
    * any l-value data type that size of type is not greater than 4 bytes.
    * not support l-value of float, double and class data type.

    **support data type in x64 platform:**
    * all r-value and pointer data types
    * any l-value data type that size of type is not greater than 8 bytes.
    * not support l-value of float and double and class data type.

    In case you want to use a data type that the framework does not support, change it to r-value type or pointer type.

    **The spy-engine supports maxium 8 arguments in an API.**
    In case you want to use more than 8 argument, just push them in to a structure.

    _**Note that, if you use pointer in spy client, you must enure that data that the pointer point to must be available in
    host process.**_

    for more detail prefer to:
     ./src/spylib2/spylib.cpp, API: showArguments
     ./src/samples/predefined-functions-2/UserSpyClient.cpp, API: showArguments

2. Use debug spy dlls in to inject to a release build mode host process and vice versa.
This may lead to a crash issue.

3. Inject dll while the host application is paused by debugging in Visual Studio.
It's ok for Win32 host application. But for console host application the spy application will hang up.
It's is blocked until the host process resume.

4. The spy lib is uninjected from host process while the spy-engine don't know.
For some reason(may be external factors or using the spy engine in incorrect way) the engine don't know
the spy lib was already uninjected from the host process and still execute command by requests from spy client.
This is lead to execute a code that is no longer belong to the process and the host process will be crashed.
