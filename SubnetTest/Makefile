Progs   = IpConv SameSubnetP 

all: $(Progs)

IpConv: IpConvertion.cxx
	gcc -o IpConv IpConvertion.cxx -lstdc++

SameSubnetP: SameSubnetTest.cxx 
	gcc -o SameSubnetP SameSubnetTest.cxx  -lstdc++ 

install: SameSubnetP
	sudo echo "Need passwd"
	sudo cp SameSubnetP /usr/local/bin 
	make clean 

clean: 
	rm IpConv SameSubnetP 