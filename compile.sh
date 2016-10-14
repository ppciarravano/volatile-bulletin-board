gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/VolatileBulletinBoardFunctions.d" -MT"src/VolatileBulletinBoardFunctions.d" -o"src/VolatileBulletinBoardFunctions.o" "src/VolatileBulletinBoardFunctions.c"
 
gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/VolatileBulletinBoard.d" -MT"src/VolatileBulletinBoard.d" -o"src/VolatileBulletinBoard.o" "src/VolatileBulletinBoard.c"

gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/VolatileBulletinBoardUsers.d" -MT"src/VolatileBulletinBoardUsers.d" -o"src/VolatileBulletinBoardUsers.o" "src/VolatileBulletinBoardUsers.c"
 
gcc  -o"VolatileBulletinBoard" ./src/VolatileBulletinBoardFunctions.o ./src/VolatileBulletinBoard.o

gcc  -o"VolatileBulletinBoardUsers" ./src/VolatileBulletinBoardFunctions.o ./src/VolatileBulletinBoardUsers.o

 

