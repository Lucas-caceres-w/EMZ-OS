#ifndef IDT_H
#define IDT_H

struct idt_entry {
  unsigned short base_low;
  unsigned short sel;
  unsigned char always0;
  unsigned char flags;
  unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

struct registers {
  unsigned int ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
} __attribute__((packed));

// Prototipos
void idt_init();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags);

void fault_handler(struct registers *r);

#endif