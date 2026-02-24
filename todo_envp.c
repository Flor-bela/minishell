#include <stdio.h>
// TO DO:

int main(int argc, char* argv[], char **envp) 
{
	(void)argc;
	(void)argv;
	
	// Para el envp:
	// Necesitamos hacer una copia con malloc de **envp del main y así después ya modificamos lo que queiramos
	// Así que pasamos la copia de ese envp a nuestro programa, no usamos el **envp directamente de main
	// built in export -> y el nombre de la variable y ahi ya cambiamos la variable (borramos el antiguo)
	
	while (*envp != NULL)
	{
		printf("%s\n", *envp++);
	}
	return 0;
}

