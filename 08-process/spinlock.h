// mutual exclusion lock for smp

#pragma once

typedef unsigned int spinlock;

void acquire(spinlock *lk);
void release(spinlock *lk);
