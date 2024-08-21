gcc -c ./implementation/ticket_machine.c
gcc -c ./implementation/ticket_office.c
gcc -c ./implementation/customer_support.c
gcc -c ./implementation/security_check.c
gcc -c ./implementation/ticket_gate.c
gcc -c ./implementation/rngs.c
gcc -c ./implementation/utility_function.c
gcc -c ./implementation/ticket_purchased.c
gcc -c simulation.c
gcc simulation.o ticket_machine.o ticket_office.o customer_support.o security_check.o ticket_gate.o rngs.o utility_function.o ticket_purchased.o -o simulation -lm -Wall -Wextra
./simulation 10 5 5 3 15 2
rm simulation
rm simulation.o
rm ticket_machine.o
rm ticket_office.o
rm customer_support.o
rm security_check.o
rm ticket_gate.o 
rm rngs.o
rm utility_function.o