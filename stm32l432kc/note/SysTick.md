- All information about SysTick is mentioned in ARM CPU manual, not in the ST
provided manual.

## SysTick Interrupt

- In order to implement an accurate time keeping, ARM's SysTick interrupt is a 
good alternative.

- SysTick is a 24 bit hardware counter.
- It has four registers.
    - `CTRL` - used to enable / disable SysTick
    - `LOAD` - an initial counter value
    - `VAL` - a current counter value, decremented on each clock cycle
    - `CALIB` - calibration register
- Every time `VAL` drops to zero, a *SysTick interrupt* is **generated**.
    - The SysTick interrupt index in the vector table is `15`.
- When a *SysTick interrupt* is generated, one should provide a handler (a 
function) for it so that it can be called.
- The SysTick address is at `0xe000e010` for all Cortex-M4 ARM processor.

## Using SysTick (With Example)

- SysTick counter can trigger interrupt for every a specific period of time.
- SysTick needs to be enabled via `CTRL`.
- SysTick must be enabled via `APB2ENR` in `RCC` as well.
    - The bit position to enable SysTick in `RCC->APB2ENR` will be varied 
    from board to board.
```c
#define BIT(x) (1UL << (x))
static inline void systick_init(uint32_t ticks) {
  if ((ticks - 1) > 0xffffff) return;  // Systick timer is 24 bit
  SYSTICK->LOAD = ticks - 1;
  SYSTICK->VAL = 0;
  SYSTICK->CTRL = BIT(0) | BIT(1) | BIT(2);  // Enable systick
  RCC->APB2ENR |= BIT(14);                   // Enable SYSCFG
}

static volatile uint32_t s_ticks; // volatile is important!!
void SysTick_Handler(void) {
  s_ticks++;
}
```
- By default, Nucleo-F429ZI board runs at 16 MHz. 
- Let's invoke `systick_init(16000000 / 1000)` to configure SysTick
- That means SysTick interrupt will be generated every millisecond.
    - 16 MHz (16000000 cycle per second) means `6.25E-8` second per period.
    - By reducing the clock frequency by a factor of 1000, it will result 16000
    Hz (16000 cycle per second), which means `6.25E-5` second per period, 
    or 0.0625 millisecond per period.
- Also, this means SysTick counter will trigger an interrupt every 16000 cycles.
- The `SYSTICK->LOAD` initial value is 15999.
- It decrements on each cycle by 1.
- When it reaches 0, an interrupt is generated. The firmware code execution gets 
interrupted.
- The associative handler for SysTick interrupt is called to increment `s_tick`.

