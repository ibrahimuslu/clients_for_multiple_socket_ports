
all:
	echo "Building all clients"
	@make client1
	@make client2	

client1:
	echo "Building client1"
	@mkdir -p build
	@gcc -o build/client1 client1.c

client2:
	echo "Building client2"
	@mkdir -p build
	@gcc -o build/client2 client2.c

clean:
	echo "Cleaning up"
	@rm -rf build
