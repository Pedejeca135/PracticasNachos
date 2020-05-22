// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"

#define TransferSize 	10 	// make it small, just to be difficult

//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

void
Copy(char *from, char *to)
{
    FILE *fp;
    OpenFile* openFile;
    int amountRead, fileLength;
    char *buffer;

// Open UNIX file
    if ((fp = fopen(from, "r")) == NULL) {	 
	printf("Copy: couldn't open input file %s\n", from);
	return;
    }

// Figure out length of UNIX file
    fseek(fp, 0, 2);		
    fileLength = ftell(fp);
    fseek(fp, 0, 0);

// Create a Nachos file of the same length
    DEBUG('f', "Copying file %s, size %d, to file %s\n", from, fileLength, to);
    if (!fileSystem->Create(to, fileLength)) {	 // Create Nachos file
	printf("Copy: couldn't create output file %s\n", to);
	fclose(fp);
	return;
    }
    
    openFile = fileSystem->Open(to);
    ASSERT(openFile != NULL);
    
// Copy the data in TransferSize chunks
    buffer = new char[TransferSize];
    while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
	openFile->Write(buffer, amountRead);	
    delete [] buffer;

// Close the UNIX and the Nachos files
    delete openFile;
    fclose(fp);
}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void
Print(char *name)
{
    OpenFile *openFile;    
    int i, amountRead;
    char *buffer;

    if ((openFile = fileSystem->Open(name)) == NULL) {
	printf("Print: unable to open file %s\n", name);
	return;
    }
    
    buffer = new char[TransferSize];
    while ((amountRead = openFile->Read(buffer, TransferSize)) > 0)
	for (i = 0; i < amountRead; i++)
	    printf("%c", buffer[i]);
    delete [] buffer;

    delete openFile;		// close the Nachos file
    return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 5000))

static void 
FileWrite()
{
    OpenFile *openFile;    
    int i, numBytes;

    printf("Sequential write of %d byte file, in %d byte chunks\n", 
	FileSize, ContentSize);
    if (!fileSystem->Create(FileName, 0)) {
      printf("Perf test: can't create %s\n", FileName);
      return;
    }
    openFile = fileSystem->Open(FileName);
    if (openFile == NULL) {
	printf("Perf test: unable to open %s\n", FileName);
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Write(Contents, ContentSize);
	if (numBytes < 10) {
	    printf("Perf test: unable to write %s\n", FileName);
	    delete openFile;
	    return;
	}
    }
    delete openFile;	// close file
}

static void 
FileRead()
{
    OpenFile *openFile;    
    char *buffer = new char[ContentSize];
    int i, numBytes;

    printf("Sequential read of %d byte file, in %d byte chunks\n", 
	FileSize, ContentSize);

    if ((openFile = fileSystem->Open(FileName)) == NULL) {
	printf("Perf test: unable to open file %s\n", FileName);
	delete [] buffer;
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Read(buffer, ContentSize);
	if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
	    printf("Perf test: unable to read %s\n", FileName);
	    delete openFile;
	    delete [] buffer;
	    return;
	}
    }
    delete [] buffer;
    delete openFile;	// close file
}

void
PerformanceTest()
{
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove(FileName)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

/************************************************************************************
Practica 5: implementacion para el manual y el despliegue de informacion de comandos
************************************************************************************/
void Manual()
{
    printf("\n\n°° MANUAL DE COMANDOS PARA EL SISTEMA DE ARCHIVOS °°\n\n");

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s.","-f");
        printf("\n\nSINTAXIS:\n\t./nachos -f ");
        printf("\n\nDESCRIPCION:\n\tFormatea el disco de nachos para ser usado.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
  
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s.","-cp");
        printf("\n\nSINTAXIS:\n\t./nachos -cp Nombre_Archivo_Unix Nombre_Archivo_Nachos.");
        printf("\n\nDESCRIPCION:\n\tCopia un archivo de UNIX a el directorio de nachos como un archivo de nachos.");
        printf("\n\nNOTA:\n\tEl nombre del archivo de nachos no debe exceder los 9 caracteres.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-p");
        printf("\n\nSINTAXIS:\n\t./nachos -p Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tImprime las caracteristicas del archivo indicado.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    
    
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-r");
        printf("\n\nSINTAXIS:\n\t./nachos -r Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tElimina del directorio el archivo indicado.\n\n");     
        printf("\n\n-------------------------------------------------------------------------------\n\n");

   
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-l");
        printf("\n\nSINTAXIS:\n\t./nachos -l");
        printf("\n\nDESCRIPCION:\n\tImprime todos los archivios del directorio de nachos.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-D");
        printf("\n\nSINTAXIS:\n\t./nachos -D");
        printf("\n\nDESCRIPCION:\n\tImprime todo el sistema de archivos actual(instanciado) de nachos.\n\n");            
        printf("\n\n-------------------------------------------------------------------------------\n\n");
   

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-t");
        printf("\n\nSINTAXIS:\n\t./nachos -t");
        printf("\n\nDESCRIPCION:\n\tPrueba que funcione correctamente el sistema de archivos.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    
/*******************************************************************
Practica 5. para las nuevas implementaciones en el sistema de archivos FileSystem.
***********************************************************************/
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-sfd");
        printf("\n\nSINTAXIS:\n\t./nachos -sfd");
        printf("\n\nDESCRIPCION:\n\tImprime todos sectores libres del sistema de archivos.\n\n");         
        printf("\n\n-------------------------------------------------------------------------------\n\n");  


        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-sf");
        printf("\n\nSINTAXIS:\n\t./nachos -sf Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tImprime todos sectores usados por el archivo indicado.\n\n");        
        printf("\n\n-------------------------------------------------------------------------------\n\n");

        
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-rf");
        printf("\n\nSINTAXIS:\n\t./nachos -rf Nombre_Archivo_Nachos Nuevo_Nombre");
        printf("\n\nDESCRIPCION:\n\tCambia el nombre del archivo indicado a el nuevo nombre.");
        printf("\n\nNOTA:\n\tEl nombre del nuevo archivo de nachos no debe exceder los 9 caracteres.\n\n");  
        printf("\n\n-------------------------------------------------------------------------------\n\n");     

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-man");
        printf("\n\nSINTAXIS:\n\t./nachos -man");
        printf("\n\nDESCRIPCION:\n\tDespliega este manual con informacion de todos los comandos\n\tdel sistema de archivos."); 
        printf("\n\n-------------------------------------------------------------------------------\n\n"); 

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-help");
        printf("\n\nSINTAXIS:\n\t./nachos -help Comando.");
        printf("\n\nDESCRIPCION:\n\tDespliega informacion sobre el comando especificado.");
        printf("\n\nNOTA:\n\tEl comando debe existir para el sistema de archivos.\n\n"); 
        printf("\n\n-------------------------------------------------------------------------------\n\n"); 

        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s","-inf");
        printf("\n\nSINTAXIS:\n\t./nachos -info");
        printf("\n\nDESCRIPCION:\n\tDespliega informacion sobre el equipo de trabajo."); 
        printf("\n\n-------------------------------------------------------------------------------\n\n");
            
}

void Help(char* comando)
{
    if (!strcmp(comando, "-f")) {
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s.",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -f ");
        printf("\n\nDESCRIPCION:\n\tFormatea el disco de nachos para ser usado.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
    else if (!strcmp(comando, "-cp")) {
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s.",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -cp Nombre_Archivo_Unix Nombre_Archivo_Nachos.");
        printf("\n\nDESCRIPCION:\n\tCopia un archivo de UNIX a el directorio de nachos como un archivo de nachos.");
        printf("\n\nNOTA:\n\tEl nombre del archivo de nachos no debe exceder los 9 caracteres.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    } else if (!strcmp(comando, "-p")) {  // print a Nachos file
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -p Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tImprime las caracteristicas del archivo indicado.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    } else if (!strcmp(comando, "-r")) {  // remove Nachos file
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -r Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tElimina del directorio el archivo indicado.\n\n");        
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    } else if (!strcmp(comando, "-l")) {  // list Nachos directory
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -l");
        printf("\n\nDESCRIPCION:\n\tImprime todos los archivios del directorio de nachos.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    } else if (!strcmp(comando, "-D")) {  // print entire filesystem
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -D");
        printf("\n\nDESCRIPCION:\n\tImprime todo el sistema de archivos actual(instanciado) de nachos.\n\n");            
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    } else if (!strcmp(comando, "-t")) {  // performance test
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -t");
        printf("\n\nDESCRIPCION:\n\tPrueba que funcione correctamente el sistema de archivos.\n\n");
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
/*******************************************************************
Practica 5. para las nuevas implementaciones en el sistema de archivos FileSystem.
***********************************************************************/
    else if(!strcmp(comando, "-sfd"))  {  //imprime los sectores libres del disco.
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -sfd");
        printf("\n\nDESCRIPCION:\n\tImprime todos sectores libres del sistema de archivos.\n\n");         
        printf("\n\n-------------------------------------------------------------------------------\n\n");  
    }
    else if(!strcmp(comando, "-sf"))  {   // imprime los sectores del archivo especificado.
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -sf Nombre_Archivo_Nachos");
        printf("\n\nDESCRIPCION:\n\tImprime todos sectores usados por el archivo indicado.\n\n");        
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
    else if(!strcmp(comando, "-rf"))  { // cambia el nombre de un archivo.
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -rf Nombre_Archivo_Nachos Nuevo_Nombre");
        printf("\n\nDESCRIPCION:\n\tCambia el nombre del archivo indicado a el nuevo nombre.");
        printf("\n\nNOTA:\n\tEl nombre del nuevo archivo de nachos no debe exceder los 9 caracteres.\n\n");   
        printf("\n\n-------------------------------------------------------------------------------\n\n");      
    }
    else if(!strcmp(comando, "-man"))  { // cambia el nombre de un archivo.
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -man");
        printf("\n\nDESCRIPCION:\n\tDespliega informacion sobre todos los comandos del sistema de archivos.");     
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
    else if(!strcmp(comando, "-help"))  { // cambia el nombre de un archivo.
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -help Comando.");
        printf("\n\nDESCRIPCION:\n\tDespliega informacion sobre el comando especificado.");
        printf("\n\nNOTA:\n\tEl comando debe existir para el sistema de archivos.\n\n");         
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
    else if(!strcmp(comando, "-inf"))  { //info
        printf("\n\n-------------------------------------------------------------------------------\n\n");
        printf("\n\nNOMBRE:\n\t%s",comando);
        printf("\n\nSINTAXIS:\n\t./nachos -info");
        printf("\n\nDESCRIPCION:\n\tDespliega informacion sobre el equipo de trabajo.");        
        printf("\n\n-------------------------------------------------------------------------------\n\n");
    }
    else
    {
        printf("\n\nEl comando %s no fue encontrado en el sistema de archivos.\n",comando);
        printf("\nIntenta con el comando -man para saber cuales comandos estan disponibles y su sintaxis...\n\tSintaxis:  ./nachos -man\n\n");
    }
}

void Info()
{
    printf("\n\n----------------  Informacion del equipo  --------------------------\n\n");
 
    printf("\nIntegrantes:\n");
    printf("\tCantú Olivares Pedro de Jesus.\n");
 
    printf("\nMateria:\n");
    printf("\tSistemas Operativos B\n");

    printf("\nFecha de entrega:\n");
    printf("\t17/Mayo/2020\n");
 
    printf("\nSemestre:\n");
    printf("\t2019-2020-II\n");
 
    printf("\nMaestra:\n");
    printf("\tMarcela Ortiz Hernández\n");
    printf("\n\n--------------------------------------------------------------------\n\n");
}
