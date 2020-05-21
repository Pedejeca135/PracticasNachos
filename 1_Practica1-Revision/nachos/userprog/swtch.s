        .text
        .align 2
        .globl ThreadRoot
ThreadRoot:
        pushl %ebp
        movl %esp,%ebp
        pushl %edx
        call *%ecx
        call *%esi
        call *%edi
        # NOT REACHED
        movl %ebp,%esp
        popl %ebp
        ret
        .comm _eax_save,4
        .globl SWITCH
SWITCH:
        movl %eax,_eax_save # save the value of eax
        movl 4(%esp),%eax # move pointer to t1 into eax
        movl %ebx,8(%eax) # save registers
        movl %ecx,12(%eax)
        movl %edx,16(%eax)
        movl %esi,24(%eax)
        movl %edi,28(%eax)
        movl %ebp,20(%eax)
        movl %esp,0(%eax) # save stack pointer
        movl _eax_save,%ebx # get the saved value of eax
        movl %ebx,4(%eax) # store it
        movl 0(%esp),%ebx # get return address from stack into ebx
        movl %ebx,32(%eax) # save it into the pc storage
        movl 8(%esp),%eax # move pointer to t2 into eax
        movl 4(%eax),%ebx # get new value for eax into ebx
        movl %ebx,_eax_save # save it
        movl 8(%eax),%ebx # retore old registers
        movl 12(%eax),%ecx
        movl 16(%eax),%edx
        movl 24(%eax),%esi
        movl 28(%eax),%edi
        movl 20(%eax),%ebp
        movl 0(%eax),%esp # restore stack pointer
        movl 32(%eax),%eax # restore return address into eax
        movl %eax,4(%esp) # copy over the ret address on the stack
        movl _eax_save,%eax
        ret
