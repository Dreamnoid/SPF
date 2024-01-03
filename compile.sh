mkdir bin/
cd SPF
g++ -c src/*.cpp -Iinclude/ -I/usr/include/SDL2 -I/usr/include/bullet -std=c++17 -fpermissive -fPIC -static
cd ..
g++ -shared -o bin/SPF.so SPF/*.o -lSDL2 -lSDL2_mixer -llibbullet
dotnet build SPFSharp/SPFSharp_linux.csproj -f net7.0
