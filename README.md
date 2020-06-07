# WebServer

This repository contains a web server done in language C.

## Requirements
- Unix-like operating system.
- GCC compiler.

## Installation

To run the server you just need to clone this repository, go into the folder that contains the file called **webserver.c** and type the following command in your terminal.
```bash
gcc -o  webserver webserver.c
./webserver
```

## Important considerations
This web server can only attend requests for a resource called TC2025.html (it was created only with demonstrative purposes), if any other request is sent to it, you will get a 404 error page.
