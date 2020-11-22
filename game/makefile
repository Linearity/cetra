#CC = gcc
#CFLAGS = -g -std=c99
CC = g++
#CFLAGS = -g -fpermissive -Wall
CFLAGS = -fpermissive -O2

SEALAB_COMMON = ../../../Library/Common/
CETRA = ../src/
FRACTURE = ../../Fracture/
ELTOPO_COMMON = ../../../Misc/eltopo/common
ELTOPO_ELTOPO3D = ../../../Misc/eltopo/eltopo3d

ISEARCH = \
	-I $(SEALAB_COMMON) \
	-I $(CETRA) \
	-I $(FRACTURE) \
	-I $(ELTOPO_COMMON) \
	-I $(ELTOPO_ELTOPO3D)

LSEARCH = -L . -L $(ELTOPO_ELTOPO3D) -L$(FRACTURE) -L $(SEALAB_COMMON)

GAME_O_FILES = \
	barymap.o \
	camera.o \
	collider.o \
	collisions.o \
	damager.o \
	deformation.o \
	deformer.o \
	displayer.o \
	embedding.o \
	event.o \
	fire.o \
	health.o \
	kinematics.o \
	kinmover.o \
	list.o \
	main.o \
	projtracker.o \
	timer.o\
	tt.o \
	vector.o



game: $(GAME_O_FILES) *.h tree/*.h
	$(CC) $(CFLAGS) $(ISEARCH) $(LSEARCH) \
		$(GAME_O_FILES) \
		$(CETRA)/IndexTet.cpp \
		$(CETRA)/TetMesh.cpp \
		-lpthread -lm -lslcommon -lfracture -leltopo_release -lglut -lGLU -lGL -llapack -lblas -o game

main.o: main.c tree/*.h
	$(CC) $(CFLAGS) $(ISEARCH) -c main.c -o main.o
	
deformer.o: deformer.cpp deformer.h
	$(CC) $(CFLAGS) $(ISEARCH) -c deformer.cpp -o deformer.o
	
deformation.o: deformation.cpp deformation.h
	$(CC) $(CFLAGS) $(ISEARCH) -c deformation.cpp -o deformation.o
	
projtracker.o: projtracker.cpp projtracker.h
		$(CC) $(CFLAGS) $(ISEARCH) -c projtracker.cpp -o projtracker.o
	
%.o: %.c %.h
	$(CC) $(CFLAGS) $(ISEARCH) -c $< -o $@
	
clean:
	rm game $(GAME_O_FILES)
