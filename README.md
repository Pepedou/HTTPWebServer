# HTTPWebServer
A simple HTTP Web Server written in C.

In order to build this program you will need cmake (https://cmake.org).

In the project's root directory, run the following commands:

`mkdir build`<br>
`cd build`<br>
`cmake ..`<br>
`make`<br>
`./WebServer -d/var/www -p8080` //If you want to serve files from /var/www using the port 8080.
