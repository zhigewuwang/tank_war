SHELL=cmd.exe
cc = gcc


default : init_dir build copy_lib copy_res run

init_dir :
	@echo.
	@echo --init dir
	if not exist bin mkdir bin
	if not exist obj mkdir obj
	if not exist res mkdir res
	if not exist src mkdir src
	if not exist inc mkdir inc



build : 
	@echo.
	@echo --build
	$(cc) -o bin/tank_war.exe src/zgl.c src/tc.c src/zassist.c -I inc -lglew32 -lopengl32 -lglu32 -lgdi32 -lfreetype -static 

debug :
	@echo.
	@echo --debug build
	$(cc) -g -o bin/tank_war.exe src/zgl.c src/tc.c src/zassist.c -I inc -lopengl32 -lglu32 -lglew32 -lgdi32 -lfreetype
	
copy_lib : 
	@echo.
	@echo --copy_lib
	xcopy lib bin /y /e


copy_res : 
	@echo.
	@echo --copy_res
	xcopy res bin /e /y

run : 
	bin/tank_war.exe
