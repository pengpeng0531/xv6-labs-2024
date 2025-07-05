#ifndef SPINLOCK_H
#define SPINLOCK_H
// Mutual exclusion lock.
struct cpu; // 前置声明，避免循环依赖
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
#endif // SPINLOCK_H