# MHTTP: Minimal HTTP Server

Ubuntu dependencies:

~~~
$ sudo apt install libevent-2.0-5 libevent-dev cmake
~~~

Build and run:

~~~
$ mkdir build
$ cd build
$ cmake ..
$ make
$ echo '<html><body>Hello World!</body></html>' > index.html
$ ./mhttp
~~~

Done! Try to navigate to localhost:8081 :)

## Options

~~~
MHTTP: Minimal HTTP Server
	-i,--ip  	host ip
	-p,--port	host port
	-u,--path	path for the uri (without last '/')
	-h,--help	help
~~~

#### Example

~~~
$ ./mhttp -i 127.0.0.1 -p 8080 -u /home/user/html_public
~~~

"html_public" must be a folder with index.html or other pages inside.
