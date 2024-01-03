mkdir bin/
cd SPF
g++ -c src/*.cpp -llibsdl2 -Iinclude/ -I/usr/include/SDL2 -I/usr/include/bullet -llibbullet -std=c++17 -fpermissive -fPIC -static
cd ..
g++ -shared -o bin/SPF.so SPF/*.o
dotnet build SPFSharp/SPFSharp_linux.csproj -f net7.0
