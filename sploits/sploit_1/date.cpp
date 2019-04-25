#include <cstdlib>
#include <unistd.h>

int main()
{
	execlp("/snap/bin/gnome-calculator", "/snap/bin/gnome-calculator", nullptr);
	return 0;
}
