#include "../lib/hw.h"

//my header files
#include "kernellib.h"
#include "syscall_c.h"
#include "sleeper.h"

void userMainFirst();
void userMainSecond();
int main() {
	__init_system();
	userMainFirst();
	userMainSecond();

	return 0;
}
