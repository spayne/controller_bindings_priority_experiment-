# controller bindings priority experiment 
This is a test bindings driver and a test application.  Used together they will answer a few - 10 or so - questions I have about how OpenVR resolves conflicts between more than one device component being bound to the same application action. 

# What's currently here

# To build debug version on windows
0. If you haven't already, download the latest openvr release from https://github.com/ValveSoftware/openvr/releases
1. open up a visual studio command prompt so cmake can work properly
2. cd into the source directory (e.g. c:\projects\controller_bindings_priority_experiment)
3. mkdir build (if necessary)
4. cd build
5. del *.*
6. Run the following (replace the path to where you have installed openvr from step 0 above): cmake -DOPENVR_ROOT_DIR=c:\projects\openvr_clean -G "Visual Studio 15 2017 Win64" ..
This will run without error
7. Open the generated Project.sln in Visual Studio 2017
8. Build the project (Ctrl-Alt-F7)
