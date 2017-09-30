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
    Nothing special. CMake for generate project files and Visual studio for coding.
    
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
    `
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
    `
    
    predefined-functions-3.exe's console:
    `
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
    `
## Usage
[![Spy Framework](https://github.com/VincentPT/spy/blob/master/doc/images/SpyFramework2.PNG)]
    
    Look at above picture. You can see, the spy framework is included on both side, client side(your application) and host side(a host application which your code will be injected to).
    
    The framework can be split into two parts:
        * The engine are spy client class and spy-engine.dll, they are corressponding together.
        * The user specific part: User Spy client class and User Spy Lib.dll. They are also corressponding together.
    
    The spy client and Spy engine are designed to respond all common spy tasks while User Spy Client and User Spy lib are designed to implement specific tasks for your application.
    
    So, in the most cases, you need to modify the user specific part.
    Ofcourse, incase the the core engine is not match your requirements, you also need to modify it.
    
    
    The main steps of programming spy application using spy framework is.
    1. Write your own spy client class inherit from SpyClient class with your specific API.
    2. Write your own spy lib dll project which will executes your specific on host process and return data to your spy client APIs.
    3. Write the code to inject the spy-engine.dll and loads your APIs in spy lib dll into host process.
    4. Write the code the call the spy client API and process the return data.
    
    
    
## Known Issues
