mkdir bin/
g++ -c SPF/src/*.cpp -llibsdl2 -Iinclude/ -I/usr/include/SDL2 -I/usr/include/bullet -llibbullet -std=c++17 -fpermissive -fPIC -static
g++ -shared -o bin/SPF.so SPF/*.o
dotnet build SPFSharp/SPFSharp_linux.csproj -f net7.0