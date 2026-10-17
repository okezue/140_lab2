# Lab 2: Sorry to Interrupt!

## Part 2: Push Button

Code is in `part2/main.c`.

**How do external interrupts work and how does the ISR handle button presses?**
The user button B1 is on PC13. SYSCFG_EXTICR4 routes PC13 to EXTI line 13, and RTSR1 makes that line trigger on a rising edge, which is when the button is pressed since B1 reads high while held. When the edge happens EXTI sets bit 13 in PR1, and because the line is unmasked in IMR1 it signals the NVIC. EXTI lines 10 to 15 share one interrupt, EXTI15_10 (IRQ 40), which is enabled with bit 8 of NVIC_ISER1. The NVIC then makes the CPU jump to `EXTI15_10_IRQHandler`, which clears the pending bit and flips PB7 in the GPIOB ODR to toggle the blue LED.

**What happens to the main loop?**
It is interrupted. The CPU saves its registers on the stack, runs the ISR, then restores them and continues the loop from exactly where it stopped. Here the loop is empty so nothing visible happens to it.

**When does the ISR get executed?**
Right after the edge is detected and the NVIC accepts the interrupt, which takes only a few clock cycles. It does not wait for the main loop to check anything.

**What would happen if you did not clear the EXTI pending flag inside the ISR?**
The pending bit would stay set, so as soon as the ISR returned the NVIC would see the interrupt still pending and enter the ISR again. The CPU would be stuck running the handler over and over, the LED would toggle constantly, and the main loop would never run again.

Video: [button.mov](part2/button.mov)

## Part 3: Timer

Code is in `part3/main.c`.

**How does a timer interrupt work?**
TIM4 has a counter that goes up by one every tick of its counter clock. When it reaches the value in the auto reload register (ARR) it goes back to 0 and creates an update event, which sets the UIF flag in TIM4_SR. Since UIE is set in TIM4_DIER, the update event also sends an interrupt request to the NVIC (TIM4 is IRQ 30, bit 30 of NVIC_ISER0), and the CPU runs `TIM4_IRQHandler`. The handler clears UIF and moves to the next LED.

**What function is counting time passed?**
The TIM4 counter (TIM4_CNT) counts time in hardware, driven by the clock through the prescaler. No code runs to count.

**What function triggers the ISR?**
The update event when the counter overflows past ARR. That sets UIF, and with UIE enabled the NVIC calls `TIM4_IRQHandler`.

**How is the timer interrupt different from an external interrupt?**
An external interrupt comes from a signal outside the chip, like the button, so it happens whenever that event happens. A timer interrupt comes from an internal peripheral counting the clock, so it is periodic and predictable. It also goes straight from TIM4 to the NVIC without using EXTI or SYSCFG.

**Timer frequency**

After reset the system clock is the MSI oscillator at 4 MHz (RCC_CR reads `0x63`, so MSIRANGE is 6), and RCC_CFGR is `0`, so the AHB and APB1 prescalers are both 1. TIM4 is on APB1, so its clock is 4 MHz.

The counter frequency is the timer clock divided by PSC + 1. With PSC = 3999:

f_CNT = 4,000,000 / (3999 + 1) = 1000 Hz

so the counter ticks once every 1 ms. The interrupt rate is the counter frequency divided by ARR + 1. With ARR = 999:

f_UPDATE = 1000 / (999 + 1) = 1 Hz

That gives one interrupt per second, so each LED stays on for 1 second.

Video: [timer.mov](part3/timer.mov)

## Part 4: Sorry to Interrupt!

Code is in `part4/main.c`.

The timer runs the same way as in Part 3. EXTI13 is set to trigger on both edges by setting bit 13 in RTSR1 and FTSR1. On either edge the EXTI ISR reads PC13 from the GPIOC IDR and stores it in `h`, so `h` is 1 while the button is held and 0 once it is released. The timer ISR still fires every second and clears its flag, but it returns early without changing the LED while `h` is 1. Reading the pin instead of flipping a flag keeps `h` correct even if the button bounces.

Video: [hold.mov](part4/hold.mov)
