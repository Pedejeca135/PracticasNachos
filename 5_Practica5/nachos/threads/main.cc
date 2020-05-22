// main.cc 
//	Bootstrap code to initialize the operating system kernel.
//
//	Allows direct calls into internal operating system functions,
//	to simplify debugging and testing.  In practice, the
//	bootstrap code would just initialize data structures,
//	and start a user program to print the login prompt.
//
// 	Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system 
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"


// External functions used by this file

extern void ThreadTest(void), Copy(char *unixFile, char *nachosFile);
extern void Print(char *file), PerformanceTest(void);
extern void StartProcess(char *file), ConsoleTest(char *in, char *out);
extern void MailTest(int networkID);

/************************ Practrica 5********************/
extern void Manual();
extern void Help(char* comando);
extern void Info();

//----------------------------------------------------------------------
// main
// 	Bootstrap the operating system kernel.  
//	
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int
main(int argc, char **argv)
{
    int argCount;			// the number of arguments 
					// for a particular command

    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);
    
#ifdef THREADS
    ThreadTest();
#endif
    bool fileSysNoExiste = false;
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
        if (!strcmp(*argv, "-z"))              // print copyright
            printf (copyright);
		
#ifdef USER_PROGRAM
        if (!strcmp(*argv, "-x")) {        	// run a user program
	    ASSERT(argc > 1);
            StartProcess(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-c")) {      // test the console
	    if (argc == 1)
	        ConsoleTest(NULL, NULL);
	    else {
		ASSERT(argc > 2);
	        ConsoleTest(*(argv + 1), *(argv + 2));
	        argCount = 3;
	    }
	    interrupt->Halt();		// once we start the console, then 
					// Nachos will loop forever waiting 
					// for console input
	}
#endif // USER_PROGRAM
#ifdef FILESYS
	if (!strcmp(*argv, "-cp")) { 		// copy from UNIX to Nachos
		fileSysNoExiste = true;
	    ASSERT(argc > 2);
	    Copy(*(argv + 1), *(argv + 2));
	    argCount = 3;
	} else if (!strcmp(*argv, "-p")) {	// print a Nachos file
		fileSysNoExiste = true;
	    ASSERT(argc > 1);
	    Print(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-r")) {	// remove Nachos file
		fileSysNoExiste = true;
	    ASSERT(argc > 1);
	    fileSystem->Remove(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-l")) {	// list Nachos directory
		fileSysNoExiste = true;
            fileSystem->List();
	} else if (!strcmp(*argv, "-D")) {	// print entire filesystem
		fileSysNoExiste = true;
            fileSystem->Print();
	} else if (!strcmp(*argv, "-t")) {	// performance test
		fileSysNoExiste = true;
            PerformanceTest();
	}
/*******************************************************************
Practica 5. para las nuevas implementaciones en el sistema de archivos FileSystem.
***********************************************************************/
	else if(!strcmp(*argv, "-sfd"))  {	//imprime los sectores libres del disco.
		fileSysNoExiste = true;
			fileSystem->Print_LibresSectores();
	}
	else if(!strcmp(*argv, "-sf"))	{	// imprime los sectores del archivo especificado.
		fileSysNoExiste = true;
		//ASSERT(argc > 1);
		if(argc > 1){
			fileSystem->Print_ArchivoSectores(*(argv + 1));
		}
		else{// si solo hay un argumento, lo hace notar al usuario
			printf("\nNo se especifico ningun nombre de archivo...\n\n");
			printf("Sintaxis correcta: ./nachos -sd nombre_archivo\n");
			interrupt->Halt();
		}
	}
	else if(!strcmp(*argv, "-rf"))	{ // cambia el nombre de un archivo.
		fileSysNoExiste = true;
		if(argc > 2){
			fileSystem->Renombra(*(argv + 1) , *(argv + 2));
		}
		else{
			printf("\nNo se especificaron los nombres de archivo correctamente...\n");
			printf("Sintaxis correcta: ./nachos -rf nombre_archivo nombre_nuevo\n");
			interrupt->Halt();
		}
	}
	else if(!strcmp(*argv, "-inf")){ // imprime informacion del equipo y materia.
		fileSysNoExiste = true;
			Info();
			interrupt->Halt();			
		}
		else if(!strcmp(*argv, "-man")){ // imprime informacion general de los comandos de nachos.
			fileSysNoExiste = true;
			 Manual();
			 interrupt->Halt();
		}
		else if(!strcmp(*argv, "-help")){ //imprime informacion del comando especificado.
			fileSysNoExiste = true;
			if(argc > 1){
				Help(*(argv + 1));
				interrupt->Halt();
			}
			else{
				printf("\nSintaxis incorrecta intenta con: ./nachos -help nombre_del_comando ");
				interrupt->Halt();
			}
		}
		else if(fileSysNoExiste == false && strcmp(*argv, "-f")){
			printf("\n\nEl comando %s no fue encontrado en el sistema de archivos.\n\n",*argv);
        	printf("\nIntenta con el comando -man para saber cuales comandos estan disponibles..\n\tSintaxis:  ./nachos -man\n\n"); 
        	fileSysNoExiste = true;       	
		}

#endif // FILESYS
#ifdef NETWORK
        if (!strcmp(*argv, "-o")) {
	    ASSERT(argc > 1);
            Delay(2); 				// delay for 2 seconds
						// to give the user time to 
						// start up another nachos
            MailTest(atoi(*(argv + 1)));
            argCount = 2;
        }
#endif // NETWORK
    }

    currentThread->Finish();	// NOTE: if the procedure "main" 
				// returns, then the program "nachos"
				// will exit (as any other normal program
				// would).  But there may be other
				// threads on the ready list.  We switch
				// to those threads by saying that the
				// "main" thread is finished, preventing
				// it from returning.
    return(0);			// Not reached...
}

