#include <htc.h>
#include "piclang.h"

char PICLANG_load(char nth)
{
  char size = 0,pos = 0,counter = 0;
  
  for(;pos < 0xff;)
    {
      size = eeprom_read(pos);
      if(counter == nth)
	break;
      pos = pos + size + 6;// move to beginning of next PCB
      counter++;
    }

  if(size == 0)
    return NO_SUCH_PROGRAM;

  curr_process.size = size;
  curr_process.pc = eeprom_read(++pos);
  curr_process.A = eeprom_read(++pos);
  curr_process.W = eeprom_read(++pos);
  curr_process.status = eeprom_read(++pos);
  curr_process.start_address = eeprom_read(++pos);

  PICLANG_quantum = DEFAULT_PICLANG_QUANTUM;
  return SUCCESS;

}

char PICLANG_save()
{
  char pos,status;
  if(curr_process.size == 0)
    return NO_SUCH_PROGRAM;

  pos = curr_process.start_address;
  if(pos - 5 < 0)
    return EEPROM_OVERFLOW;
  
  pos -= 2;
  if(curr_process.status == SUCCESS)
    curr_process.status = SUSPENDED;
  status = curr_process.status;
  eeprom_write(pos,curr_process.status);
  pos--;
  eeprom_write(pos,curr_process.W);
  pos--;
  eeprom_write(pos,curr_process.A);
  pos--;
  eeprom_write(pos,curr_process.pc);

  PICLANG_init();

  return status;

}


void PICLANG_init()
{
  curr_process.size = 0;
  curr_process.pc = 0;
  curr_process.A = 0;
  curr_process.W = 0;
  curr_process.status = SUSPENDED;
  curr_process.start_address = 0;
  PICLANG_quantum = 0;
}

char PICLANG_get_next_byte(){
  return eeprom_read((curr_process.pc++) + curr_process.start_address);
}

#define PICLANG_error(code)  curr_process.status = code

void PICLANG_next()
{
  char command;
  if(curr_process.size == 0)
    return;

  if(PICLANG_quantum == 0)
    {
      PICLANG_save();
      return;
    }
  
  if(curr_process.size < curr_process.pc)
    {
      PICLANG_error(PC_OVERFLOW);
      return;
    }

  command = PICLANG_get_next_byte();
  switch(command)
    {
    case EOP:
      PICLANG_save();
      return;
    case PICLANG_ADDA:
      curr_process.A += PICLANG_get_next_byte();
      return;
    case PICLANG_SUBA:
      curr_process.A -= PICLANG_get_next_byte();
      return;
    case PICLANG_MULTA:
      curr_process.A *= PICLANG_get_next_byte();
      return;
    default:
      PICLANG_error(UNKNOWN_COMMAND);
      return;
    }
}
