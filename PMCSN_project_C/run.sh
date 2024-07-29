cc -c ./implementation/ticket_machine.c
gcc -c ./implementation/ticket_office.c
gcc -c ./implementation/customer_support.c
gcc -c ./implementation/security_check.c
gcc -c ./implementation/ticket_gate.c
gcc -c simulation.c
gcc simulation.o ticket_machine.o ticket_office.o customer_support.o security_check.o ticket_gate.o -o simulation
./simulation