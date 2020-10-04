For first time sctp socket programming in a machine install libsctp

sudo apt install libsctp-dev

Compile with linking sctp library
gcc SCTP_SERVER.c -Wall -g -lsctp -o sctp_server
