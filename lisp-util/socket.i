%module socket
%{
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <stdio.h>
%}

%include <sys/socket.h>
%include <netinet/in.h>
%include <arpa/inet.h>
%include <unistd.h>
%include <stdio.h>
