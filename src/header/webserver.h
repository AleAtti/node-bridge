/*
Description:      A small webserver to serve the web interface and handle configuration
Resources:  https://www.gnu.org/software/libmicrohttpd/manual/libmicrohttpd.html#index-MHD_005fstart_005fdaemon
            https://www.gnu.org/software/libmicrohttpd/

*/
#ifndef WEBSERVER_H
#define WEBSERVER_H

/*  

*/
int start_webserver(int server_port);

#endif