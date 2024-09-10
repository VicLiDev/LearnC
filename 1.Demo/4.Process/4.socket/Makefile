all:
	@gcc 1.client.c -o 1.client
	@gcc 1.server.c -o 1.server
	@gcc 2.socket-client.c -o 2.socket-client
	@gcc 2.socket-server.c -o 2.socket-server
	@gcc 3.socket-inet.c   -o 3.socket-inet

test1:
	@./1.server &
	@./1.client

test2:
	@./2.socket-server ./socket &
	@sleep 1 && ./2.socket-client ./socket "hello world"
	@sleep 1 && ./2.socket-client ./socket "This is a test"
	@sleep 1 && ./2.socket-client ./socket "quit"

test3:
	@./3.socket-inet www.baidu.com

clean:
	@-rm 1.client 1.server
	@-rm 2.socket-client 2.socket-server
	@-rm 3.socket-inet
