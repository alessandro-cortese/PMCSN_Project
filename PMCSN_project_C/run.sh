gcc -c ./implementation/ticket_machine.c
gcc -c ./implementation/ticket_office.c
gcc -c ./implementation/customer_support.c
gcc -c ./implementation/security_check.c
gcc -c ./implementation/ticket_gate.c
gcc -c ./implementation/rngs.c
gcc -c simulation.c
gcc simulation.o ticket_machine.o ticket_office.o customer_support.o security_check.o ticket_gate.o rngs.o -o simulation -lm
./simulation 2.0 3.0 4.0 5.0 6.0 1
rm simulation
rm simulation.o
rm ticket_machine.o
rm ticket_office.o
rm customer_support.o
rm security_check.o
rm ticket_gate.o 
rm rngs.o