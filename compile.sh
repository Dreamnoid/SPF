mkdir bin/
cd SPF
g++ -c src/*.cpp -Iinclude/ -I/usr/include/SDL2 -std=c++17 -fpermissive -fPIC -static
cd ..
g++ -shared -o bin/SPF.so SPF/*.o -lSDL2 -rpath,'$ORIGIN'/lib
dotnet build SPFSharp/SPFSharp_linux.csproj -f net7.0
