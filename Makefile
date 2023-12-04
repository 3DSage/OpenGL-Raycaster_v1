EXE = 3DSage_Raycaster_v1
CFLAGS = -O2
LDFLAGS = -lglut -lGLU -lGL -lm

$(EXE):	3DSage_Raycaster_v1.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f $(EXE)
