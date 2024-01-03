mkdir ../bin/
g++ -c src/*.cpp -llibsdl2 -Iinclude/ -I/usr/include/SDL2 -I/usr/include/bullet -llibbullet -std=c++17 -fpermissive -fPIC -static
g++ -shared -o ../bin/SPF.so *.o