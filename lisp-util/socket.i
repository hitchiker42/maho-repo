%module socket
%{
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
%}

%include <sys/socket.h>
%include <netinet/in.h>
%include <arpa/inet.h>
