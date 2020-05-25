// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

/*************************************+
Practica 2
**************************************/
OpenFile *exeFile;
OpenFile *swapOpenFile;

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

/******************************************
Practica1
*******************************************/
//funcion para encontrar el indice de la ultima ocurrencia de un caracter dado.
int strlo(char* array, char caracter)
{

    for(int i = strlen(array)-1 ; i>-1;i--)
    {
        if(array[i] == caracter)
        {
            return i;
        }
    }
    return -1;

}

char* strsub(char* array, int indexOffset , int lenght)
{
    char res[lenght]= "";
    for(int i = 0; i < lenght ; i++)
    {
        res[i] = array[indexOffset + i] ;
    }
    printf("retornara %s\n", res );
    return res;
}

AddrSpace::AddrSpace(OpenFile *executable, char* filename)
{

    /************************************************
    Practica 1: para crear el archivo de intercambio
    ******************************************************/
    char swapPath [strlen(filename)+4] = "";
    strcat(swapPath,filename);
    strcat(swapPath,".swp");
    //printf("%s\n",swapPath);

    if(!fileSystem->Create(swapPath,executable->Length()-40))
    {
        printf("\nNo se pudo crear el archivo de intercambio %s\n", swapPath );
    }
    else
    {        
        machine->swapFileName = new char[strlen(swapPath)];
        strcpy(machine->swapFileName,swapPath);
        //printf("\n%s |-| Tam original::::: %d \n",swapPath, strlen(swapPath));
        //printf("\n%s |-| Tam otro::::: %d \n",machine->swapFileName, strlen(machine->swapFileName));
        /*OpenFile **/swapOpenFile = fileSystem->Open(swapPath);

        if(swapOpenFile == NULL)
        {
            printf("\nEl archivo de intercambio no existe\n");
        }
        else
        {
            char *aux;
            aux = new char[executable->Length()-40];

            int auxInt1 = executable->ReadAt(aux,executable->Length()-40,40);

            if(auxInt1 > 0)
            {
                int auxInt2 = swapOpenFile->Write(aux,executable->Length()-40);
                delete swapOpenFile;
                if(auxInt2 <= 0)
                {
                    printf("\nNo se pudo escribir en el archivo de intercambio\n");
                }
            }
            else
            {
                printf("\nNo se pudo hacer lectura del ejecutable\n");
            }
        }
    }    


    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    
    numPages = divRoundUp(size, PageSize);
    
    /**************************************************
    Practica 0 : 
    ***************************************************/
    printf("\nTamaño del proceso: %d Bytes.\n",size);//imprime el tamaño del proceso.
    printf("\nNumero de paginas para el proceso: %d.",numPages);//imprime el numero de paginas necesarias para cargar el proceso.
    
    size = numPages * PageSize;

    /***********************************
    Practica 2
    *************************************/
/*
if(numPages > NumPhysPages) // Para evitar que se impriman las tablas en los procesos que usen mas paginas que las que se tienen.
{
    printf("\n\nEl tamaño del proceso excede las paginas disponibles( %d ), y no se ha implementado memoria virtual por lo que no puede ser cargado.\n\n",NumPhysPages);
}
*/
    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",numPages, size);

    //Practica 0. para imprimir la tabla de paginas.
    printf("\n\nTabla de paginas:\n");
    printf("Indice \tNo.Marco\tBit Validez\n");

    // first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    //pageTable = new TranslationEntry[NumPhysPages];
    /*********************************************
    Practica 2
    **********************************************/
    for (i = 0; i < numPages; i++) 
    {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	//pageTable[i].physicalPage = i;
	pageTable[i].valid = FALSE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    //Practica0.
    printf("%d \t %d \t\t %d\n",pageTable[i].virtualPage,pageTable[i].physicalPage,pageTable[i].valid);//imprimir la informacion de la pagina actual con el indice i.
    }

    //Practica0
    //printf("\nMapeo de direcciones logicas:\n");
    //printf("Dirección lógica \t No.Pagina(p) \t Desplazamiento(d) \t Dirección Fisica\t\n");


// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
   // bzero(machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
       // executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			//noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }

}





//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

/****************************************
Practica2
****************************************/

bool 
AddrSpace::swapIn(int vpn)
{
    //file_name se agrego a machine
    printf("Haciendo swaping de %s\n",machine->swapFileName );
    OpenFile *swp = fileSystem->Open(machine->swapFileName);   //abrimos el archivo
    if(swp == NULL)
    {
        printf("\nswabFile no abrio\n");
        return false;
    }
    else
    {
        int direccionBaseDeMarco = stats->contadorMarco * PageSize;
        printf("Escribiendo en la direccion %d de la memoria principal\nTamaño de escritura: %d\nDesde la direccion %d del archivo de intercambio.\n",direccionBaseDeMarco,PageSize,vpn * PageSize);
        swp->ReadAt(&(machine->mainMemory[direccionBaseDeMarco]),PageSize,vpn * PageSize);
        stats->numDiskReads++;
    }
    delete swp; //cerramos el archivo
    return true;
}

/***************************************+
Practica 3
**************************************/


bool
AddrSpace::swapOut()
{

    

    int indicePagina = -1;

    for(int i = 0 ; i < numPages ; i++)
    {
        if(pageTable[i].physicalPage == stats->contadorMarco && pageTable[i].valid == TRUE)
        {
            indicePagina = i ;
            break;
        }
    }

    if(indicePagina <= -1)
    {
        printf("Pagina no encontrada\n");
    }
    else
    {
        printf("Haciendo swapOut a %s\n",machine->swapFileName );
        if(pageTable[indicePagina].dirty)//la pagina esta sucia.
        {

            OpenFile *swp = fileSystem->Open(machine->swapFileName);   //abrimos el archivo
            int direccionBaseDeMarco = stats->contadorMarco * PageSize;
            if(swp == NULL)
            {
                printf("\nswabFile no abrio\n");
                return false;
            }
            else
            {
                printf("Escribiendo en la direccion %d de la memoria principal\nTamaño de escritura: %d\nDesde la direccion %d del archivo de intercambio.\n",direccionBaseDeMarco,PageSize,indicePagina* PageSize);
                swp->WriteAt(&(machine->mainMemory[direccionBaseDeMarco]), PageSize, indicePagina* PageSize);
                stats->numDiskWrites++;
            }
            delete swp; //cerramos el archivo

        }

        pageTable[indicePagina].valid = FALSE;
        pageTable[indicePagina].dirty = FALSE;

        return true;
    }
    return false;    
}