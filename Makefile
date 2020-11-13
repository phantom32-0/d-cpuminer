build:
        gcc main.c -Wall -lcrypto -O3 -o d-cpuminer
        @echo d-cpuminer built, you can now run ./d-cpuminer
clean:
        rm -f *.o d-cpuminer
