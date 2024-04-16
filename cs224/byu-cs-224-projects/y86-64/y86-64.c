#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE  = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
  *icode = (getByteFromMemory(getPC()) >> 4) & 0x0F;
  *ifun = getByteFromMemory(getPC()) & 0x0F;

  switch(*icode) {
    case HALT:
      *valP = getPC() + 1;
      break;
    case NOP:
      *valP = getPC() + 1;
      break;
    case RRMOVQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valP = getPC() + 2;
      break;
    case IRMOVQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valC = getWordFromMemory(getPC() + 2);
      *valP = getPC() + 10;
      break;
    case RMMOVQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valC = getWordFromMemory(getPC() + 2);
      *valP = getPC() + 10;
      break;
    case MRMOVQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valC = getWordFromMemory(getPC() + 2);
      *valP = getPC() + 10;
      break;
    case OPQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valP = getPC() + 2;
      break;
    case JXX:
      *valC = getWordFromMemory(getPC() + 1);
      *valP = getPC() + 9;
      break;
    case CALL:
      *valC = getWordFromMemory(getPC() + 1);
      *valP = getPC() + 9;
      break;
    case RET:
      *valP = getPC() + 1;
      break;
    case PUSHQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valP = getPC() + 2;
      break;
    case POPQ:
      *rA = (getByteFromMemory(getPC() + 1) >> 4) & 0x0F;
      *rB = getByteFromMemory(getPC() + 1) & 0x0F;
      *valP = getPC() + 2;
      break;
    default:
      printf("Switch in fetch stage went to default.\n");
      break;
  }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
  switch(icode) {
    case RRMOVQ:
      *valA = getRegister(rA);
      break;
    case RMMOVQ:
      *valA = getRegister(rA);
      *valB = getRegister(rB);
      break;
    case MRMOVQ:
      *valB = getRegister(rB);
      break;
    case OPQ:
      *valA = getRegister(rA);
      *valB = getRegister(rB);
      break;
    case CALL:
      *valB = getRegister(RSP);
      break;
    case RET:
      *valA = getRegister(RSP);
      *valB = getRegister(RSP);
      break;
    case PUSHQ:
      *valA = getRegister(rA);
      *valB = getRegister(RSP);
      break;
    case POPQ:
      *valA = getRegister(RSP);
      *valB = getRegister(RSP);
      break;
    default:
      break;
  }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
  switch(icode) {
    case RRMOVQ:
      *valE = 0 + valA;
      break;
    case IRMOVQ:
      *valE = 0 + valC;
      break;
    case RMMOVQ:
      *valE = valB + valC;
      break;
    case MRMOVQ:
      *valE = valB + valC;
      break;
    case OPQ:
      bool sf = 0;
      bool zf = 0;
      bool of = 0;
      
      switch(ifun) {
        case ADD:
          *valE = valB + valA;
          break;
        case SUB:
          *valE = valB - valA;
          break;
        case AND:
          *valE = valB & valA;
          break;
        case XOR:
          *valE = valB ^ valA;
          break;
        default:
          printf("Invalid or unknown operation encountered when trying to execute OPQ.\n");
      }
      
      if (*valE == 0) {
        zf = 1;
      }
      if (*valE < 0) {
        sf = 1;
      }
      if ((valA > 0 && valB > 0 && *valE < 0) || (valA < 0 && valB < 0 && *valE > 0) || (valA < 0 && valB < 0 && *valE == 0)) {
        of = 1;
      }
      if ((valA > 0 && valB < 0 && *valE > 0) || (valA < 0 && valB > 0 && *valE > 0)) {
        of = 1;
      }
      if(ifun == XOR || ifun == AND) {
        of = 0;
      }

      setFlags(sf, zf, of);

      break;
    case JXX:
      *Cnd = Cond(ifun);
      break;
    case CALL:
      *valE = valB + (-8);
      break;
    case RET:
      *valE = valB + 8;
      break;
    case PUSHQ:
      *valE = valB + (-8);
      break;
    case POPQ:
      *valE = valB + 8;
      break;
    default:
      break;
  }
}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
   switch(icode) {
    case RMMOVQ:
      setWordInMemory(valE, valA);
      break;
    case MRMOVQ:
      *valM = getWordFromMemory(valE);
      break;
    case CALL:
      setWordInMemory(valE, valP);
      break;
    case RET:
      *valM = getWordFromMemory(valA);
      break;
    case PUSHQ:
      setWordInMemory(valE, valA);
      break;
    case POPQ:
      *valM = getWordFromMemory(valA);
      break;
    default:
      break;
  }
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM) {
   switch(icode) {
    case RRMOVQ:
      setRegister(rB, valE);
      break;
    case IRMOVQ:
      setRegister(rB, valE);
      break;
    case MRMOVQ:
      setRegister(rA, valM);
      break;
    case OPQ:
      setRegister(rB, valE);
      break;
    case CALL:
      setRegister(RSP, valE);
      break;
    case RET:
      setRegister(RSP, valE);
      break;
    case PUSHQ:
      setRegister(RSP, valE);
      break;
    case POPQ:
      setRegister(RSP, valE);
      setRegister(rA, valM);
      break;
    default:
      break;
  }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
  if(icode == HALT) {
    setStatus(STAT_HLT);
    setPC(valP);
  } else if (icode == RET) {
    setPC(valM);
  } else if (icode == CALL) {
    setPC(valC);
  } else if (icode == JXX) {
    if (Cnd) {
      setPC(valC);
    } else {
      setPC(valP);
    }
  } else {
    setPC(valP);
  }
}

void stepMachine(int stepMode) {
  /* FETCH STAGE */
  int icode = 0, ifun = 0;
  int rA = 0, rB = 0;
  wordType valC = 0;
  wordType valP = 0;
 
  /* DECODE STAGE */
  wordType valA = 0;
  wordType valB = 0;

  /* EXECUTE STAGE */
  wordType valE = 0;
  bool Cnd = 0;

  /* MEMORY STAGE */
  wordType valM = 0;

  fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
  applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  decodeStage(icode, rA, rB, &valA, &valB);
  applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
  applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  memoryStage(icode, valA, valP, valE, &valM);
  applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  writebackStage(icode, rA, rB, valE, valM);
  applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  pcUpdateStage(icode, valC, valP, Cnd, valM);
  applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  incrementCycleCounter();
}

/** 
 * main
 * */
int main(int argc, char **argv) {
  int stepMode = 0;
  FILE *input = parseCommandLine(argc, argv, &stepMode);

  initializeMemory(MAX_MEM_SIZE);
  initializeRegisters();
  loadMemory(input);

  applyStepMode(stepMode);
  while (getStatus() != STAT_HLT) {
    stepMachine(stepMode);
    applyStepMode(stepMode);
#ifdef DEBUG
    printMachineState();
    printf("\n");
#endif
  }
  printMachineState();
  return 0;
}